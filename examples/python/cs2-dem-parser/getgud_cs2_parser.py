from demoparser2 import DemoParser  # pylint: disable=E0611
import os
from getgud_parser_structs import *
import shutil
import time
import requests
import pandas as pd
import numpy as np
from typing import Any
from tqdm import tqdm
import re

CHAR_LIMIT = { "small": 20, "big": 36 }

        
class GetgudDemoParser:
    '''
        Adaptation of awpy code for demo parsing using demoparse2 for our case
    '''
    
    def __init__(self, file_path):
        self.parser = DemoParser(file_path)
        
    def remove_nonplay_ticks(self, parsed_df: pd.DataFrame) -> pd.DataFrame:
        """Filter out non-play records from a dataframe.

        Args:
            parsed_df (pd.DataFrame): A dataframe with the columns...

        Returns:
            pd.DataFrame: A dataframe with the non-play records removed.
        """
        # Define the required columns
        required_columns = [
            "is_freeze_period",
            "is_warmup_period",
            "is_terrorist_timeout",
            "is_ct_timeout",
            "is_technical_timeout",
            "is_waiting_for_resume",
            "is_match_started",
            "game_phase",
        ]

        # Filter out the columns that are not present in the DataFrame
        available_columns = [col for col in required_columns if col in parsed_df.columns]

        # Create the filter condition based on available columns
        filter_condition = pd.Series([True] * len(parsed_df))

        # do dropna in case some field is missing
        if "is_freeze_period" in available_columns:
            filter_condition &= ~parsed_df["is_freeze_period"].dropna()
        if "is_warmup_period" in available_columns:
            filter_condition &= ~parsed_df["is_warmup_period"].dropna()
        if "is_terrorist_timeout" in available_columns:
            filter_condition &= ~parsed_df["is_terrorist_timeout"].dropna()
        if "is_ct_timeout" in available_columns:
            filter_condition &= ~parsed_df["is_ct_timeout"].dropna()
        if "is_technical_timeout" in available_columns:
            filter_condition &= ~parsed_df["is_technical_timeout"].dropna()
        if "is_waiting_for_resume" in available_columns:
            filter_condition &= ~parsed_df["is_waiting_for_resume"].dropna()
        if "is_match_started" in available_columns:
            filter_condition &= parsed_df["is_match_started"].dropna()
        if "game_phase" in available_columns:
            filter_condition &= parsed_df["game_phase"].dropna().isin([2, 3])

        # Apply the filter condition
        parsed_df = parsed_df[filter_condition]

        # Drop the available state columns
        return parsed_df.drop(columns=available_columns)


    def parse_grenades(self) -> pd.DataFrame:
        """Parse the grenades of the demofile.

        Returns:
            The grenade trajectories for the demofile.
        """
        grenade_df = self.parser.parse_grenades()
        grenade_df = grenade_df.rename(columns={"name": "thrower"})
        return grenade_df[
            [
                "thrower_steamid",
                "thrower",
                "grenade_type",
                "tick",
                "X",
                "Y",
                "Z",
                "entity_id",
            ]
        ]


    def parse_kills(self, events: dict[str, pd.DataFrame]) -> pd.DataFrame:
        """Parse the kills of the demofile.

        Args:
            events: A dictionary of parsed events.

        Returns:
            The kills for the demofile.
        """
        # Get the kill events
        kill_df = events.get("player_death")
        if kill_df is None:
            # we didn't find player_death in events."
            return pd.DataFrame()

        # Filter out nonplay ticks
        kill_df = self.remove_nonplay_ticks(kill_df)

        # Get only relevant columns
        kill_df = kill_df[
            [
                # Kill Info
                "tick",
                "assistedflash",
                "dmg_health",
                "dmg_armor",
                "attackerblind",
                "headshot",
                "hitgroup",
                "noscope",
                "penetrated",
                "thrusmoke",
                "is_bomb_planted",
                "weapon",
                # Assister
                "assister_X",
                "assister_Y",
                "assister_Z",
                "assister_pitch",
                "assister_yaw",
                "assister_last_place_name",
                "assister_flash_duration",
                "assister_health",
                "assister_armor",
                "assister_current_equip_value",
                "assister_has_defuser",
                "assister_has_helmet",
                "assister_inventory",
                "assister_ping",
                "assister_pitch",
                "assister_team_name",
                "assister_team_clan_name",
                "assister_name",
                "assister_steamid",
                # Attacker
                "attacker_X",
                "attacker_Y",
                "attacker_Z",
                "attacker_pitch",
                "attacker_yaw",
                "attacker_last_place_name",
                "attacker_flash_duration",
                "attacker_health",
                "attacker_armor",
                "attacker_current_equip_value",
                "attacker_has_defuser",
                "attacker_has_helmet",
                "attacker_inventory",
                "attacker_ping",
                "attacker_pitch",
                "attacker_team_name",
                "attacker_team_clan_name",
                "attacker_name",
                "attacker_steamid",
                # Victim
                "user_X",
                "user_Y",
                "user_Z",
                "user_pitch",
                "user_yaw",
                "user_last_place_name",
                "user_flash_duration",
                "user_health",
                "user_armor",
                "user_current_equip_value",
                "user_has_defuser",
                "user_has_helmet",
                "user_inventory",
                "user_ping",
                "user_pitch",
                "user_team_name",
                "user_team_clan_name",
                "user_name",
                "user_steamid"
            ]
        ]

        # Rename columns
        kill_df = kill_df.rename(
            columns={
                "is_bomb_planted": "bomb_planted",
                "assister_team_name": "assister_side",
                "attacker_team_name": "attacker_side",
                "user_team_name": "victim_side",
                "assister_team_clan_name": "assister_clan",
                "attacker_team_clan_name": "attacker_clan",
                "user_team_clan_name": "victim_clan",
                "assister_last_place_name": "assister_place",
                "attacker_last_place_name": "attacker_place",
                "user_last_place_name": "victim_place",
                "assister_has_defuser": "assister_defuser",
                "attacker_has_defuser": "attacker_defuser",
                "user_has_defuser": "victim_defuser",
                "assister_has_helmet": "assister_helmet",
                "attacker_has_helmet": "attacker_helmet",
                "user_has_helmet": "victim_helmet",
                "assister_current_equip_value": "assister_equipment_value",
                "attacker_current_equip_value": "attacker_equipment_value",
                "user_current_equip_value": "victim_equipment_value",
            }
        )
        for col in kill_df.columns:
            if "user_" in col:
                kill_df = kill_df.rename(columns={col: col.replace("user_", "victim_")})


        return kill_df


    def parse_damages(self, events: dict[str, pd.DataFrame]) -> pd.DataFrame:
        """Parse the damages of the demofile.

        Args:
            events: A dictionary of parsed events.

        Returns:
            The damages for the demofile.
        """
        # Get the damage events
        damage_df = events.get("player_hurt")
        if damage_df is None:
            # player_hurt not found in events.
            return pd.DataFrame()

        # Filter out nonplay ticks
        damage_df = self.remove_nonplay_ticks(damage_df)

        # Get only relevant columns
        damage_df = damage_df[
            [
                # Damage info
                "tick",
                "weapon",
                "dmg_armor",
                "dmg_health",
                "hitgroup",
                "is_bomb_planted",
                # Attacker
                "attacker_X",
                "attacker_Y",
                "attacker_Z",
                "attacker_pitch",
                "attacker_yaw",
                "attacker_last_place_name",
                "attacker_health",
                "attacker_armor",
                "attacker_current_equip_value",
                "attacker_has_defuser",
                "attacker_has_helmet",
                "attacker_inventory",
                "attacker_ping",
                "attacker_pitch",
                "attacker_team_name",
                "attacker_team_clan_name",
                "attacker_name",
                "attacker_steamid",
                # Victim
                "user_X",
                "user_Y",
                "user_Z",
                "user_pitch",
                "user_yaw",
                "user_last_place_name",
                "user_health",
                "user_armor",
                "user_current_equip_value",
                "user_has_defuser",
                "user_has_helmet",
                "user_inventory",
                "user_ping",
                "user_pitch",
                "user_team_name",
                "user_team_clan_name",
                "user_name",
                "user_steamid"
            ]
        ]

        # Rename columns
        damage_df = damage_df.rename(
            columns={
                "is_warmup_period": "warmup",
                "is_match_started": "started",
                "is_bomb_planted": "bomb_planted",
                "game_phase": "phase",
                "attacker_team_name": "attacker_side",
                "user_team_name": "victim_side",
                "attacker_team_clan_name": "attacker_clan",
                "user_team_clan_name": "victim_clan",
                "attacker_last_place_name": "attacker_place",
                "user_last_place_name": "victim_place",
                "attacker_has_defuser": "attacker_defuser",
                "user_has_defuser": "victim_defuser",
                "attacker_has_helmet": "attacker_helmet",
                "user_has_helmet": "victim_helmet",
                "attacker_current_equip_value": "attacker_equipment_value",
                "user_current_equip_value": "victim_equipment_value",
            }
        )

        for col in damage_df.columns:
            if "user_" in col:
                damage_df = damage_df.rename(columns={col: col.replace("user_", "victim_")})


        # Create dmg_health_real column
        damage_df["dmg_health_real"] = np.where(
            damage_df["dmg_health"] > damage_df["victim_health"],
            damage_df["victim_health"],
            damage_df["dmg_health"],
        )

        return damage_df


    def parse_bomb(self, events: dict[str, pd.DataFrame]) -> pd.DataFrame:
        """Parse the bomb events of the demofile.

        Args:
            events: A dictionary of parsed events.

        Returns:
            The bomb events for the demofile.
        """
        # Get bomb plants
        bomb_planted = events.get("bomb_planted")
        if bomb_planted is None:
            # bomb_planted not found in events.
            return pd.DataFrame()

        bomb_planted["event"] = "planted"
        bomb_planted = self.remove_nonplay_ticks(bomb_planted)

        # Get bomb defuses
        bomb_defused = events.get("bomb_defused")
        if bomb_defused is None:
            bomb_defused_missing_msg = "bomb_defused not found in events."
            raise KeyError(bomb_defused_missing_msg)

        bomb_defused["event"] = "defused"
        bomb_defused = self.remove_nonplay_ticks(bomb_defused)

        # Get bomb explosions
        bomb_exploded = events.get("bomb_exploded")
        if bomb_exploded is None:
            return pd.DataFrame()

        bomb_exploded["event"] = "exploded"
        bomb_exploded = self.remove_nonplay_ticks(bomb_exploded)
        # Combine all bomb events
        bomb_df = pd.concat([bomb_planted, bomb_defused, bomb_exploded])
        # Rename columns
        for col in bomb_df.columns:
            if "user_" in col:
                bomb_df = bomb_df.rename(columns={col: col.replace("user_", "player_")})
        return bomb_df


    def parse_smokes(self, events: dict[str, pd.DataFrame]) -> pd.DataFrame:
        """Parse the smokes of the demofile.

        Args:
            events: A dictionary of parsed events.

        Returns:
            The smokes for the demofile.

        """
        # Get smoke starts
        smoke_starts = events.get("smokegrenade_detonate")
        if smoke_starts is None:
            # smokegrenade_detonate not found in events.
            return pd.DataFrame()

        smoke_starts = self.remove_nonplay_ticks(smoke_starts)

        # Get smoke ends
        smoke_ends = events.get("smokegrenade_expired")
        if smoke_ends is None:
            # smokegrenade_expired not found in events.
            return pd.DataFrame()

        smoke_ends = self.remove_nonplay_ticks(smoke_ends)

        # Initialize an empty list to store the matched rows
        matched_rows = []

        # Loop through each row in smoke starts
        for _, start_row in smoke_starts.iterrows():
            # Find the corresponding end row
            end_row = smoke_ends[
                (smoke_ends["entityid"] == start_row["entityid"])
                & (smoke_ends["tick"] > start_row["tick"])
            ]
            combined_row = {
                "entity_id": start_row["entityid"],
                "start_tick": start_row["tick"],
                "end_tick": None if end_row.empty else end_row.iloc[0]["tick"],
                "thrower_name": start_row["user_name"],
                "thrower_clan": start_row["user_team_clan_name"],
                "thrower_side": start_row["user_team_name"],
                "thrower_steamid": start_row["user_steamid"],
                "X": start_row["x"],
                "Y": start_row["y"],
                "Z": start_row["z"],
            }
            matched_rows.append(combined_row)
        return pd.DataFrame(matched_rows)


    def parse_infernos(self, events: dict[str, pd.DataFrame]) -> pd.DataFrame:
        """Parse the infernos of the demofile.

        Args:
            events: A dictionary of parsed events.

        Returns:
            The infernos for the demofile.

        """
        # Get inferno starts
        inferno_starts = events.get("inferno_startburn")
        if inferno_starts is None:
            # inferno_startburn not found in events.
            return pd.DataFrame()

        inferno_starts = self.remove_nonplay_ticks(inferno_starts)

        # Get inferno ends
        inferno_ends = events.get("inferno_expire")
        if inferno_ends is None:
            # inferno_expire not found in events.
            return pd.DataFrame()

        inferno_ends = self.remove_nonplay_ticks(inferno_ends)
        # Initialize an empty list to store the matched rows
        matched_rows = []

        # Loop through each row in inferno_starts
        for _, start_row in inferno_starts.iterrows():
            # Find the corresponding end row
            end_row = inferno_ends[
                (inferno_ends["entityid"] == start_row["entityid"])
                & (inferno_ends["tick"] > start_row["tick"])
            ]
            # If a match is found, append the combined data to the matched_rows list
            combined_row = {
                "entity_id": start_row["entityid"],
                "start_tick": start_row["tick"],
                "end_tick": None if end_row.empty else end_row.iloc[0]["tick"],
                "thrower_name": start_row["user_name"],
                "thrower_clan": start_row["user_team_clan_name"],
                "thrower_side": start_row["user_team_name"],
                "thrower_steamid": start_row["user_steamid"],
                "X": start_row["x"],
                "Y": start_row["y"],
                "Z": start_row["z"],
            }
            matched_rows.append(combined_row)
        return pd.DataFrame(matched_rows)


    def parse_weapon_fires(self, events: dict[str, pd.DataFrame]) -> pd.DataFrame:
        """Parse the weapon fires of the demofile.

        Args:
            events: A dictionary of parsed events.

        Returns:
            The weapon fires for the demofile.

        """
        weapon_fires_df = events.get("weapon_fire")
        if weapon_fires_df is None:
            # weapon_fire not found in events.
            return pd.DataFrame()

        weapon_fires_df = self.remove_nonplay_ticks(weapon_fires_df)
        weapon_fires_df = weapon_fires_df[
            [
                "tick",
                "user_name",
                "user_steamid",
                "user_team_name",
                "user_team_clan_name",
                "user_X",
                "user_Y",
                "user_Z",
                "user_yaw",
                "user_pitch",
                "user_last_place_name",
                # "user_is_strafing",
                "user_accuracy_penalty",
                "user_health",
                "user_armor",
                "user_rank",
                "user_zoom_lvl",
                "user_inventory",
                "weapon",
            ]
        ]
        weapon_fires_df.rename(
            columns={
                "user_last_place_name": "player_place",
                "user_team_clan_name": "player_clan",
            }
        )
        for col in weapon_fires_df.columns:
            if "user_" in col:
                weapon_fires_df = weapon_fires_df.rename(
                    columns={col: col.replace("user_", "player_")}
                )
        return weapon_fires_df


    def parse_ticks(self) -> pd.DataFrame:
        """Parse the ticks of the demofile.

        Returns:
            The ticks for the demofile.
        """
        ticks = self.parser.parse_ticks(
            [
                # Key presses
                "FORWARD",
                "LEFT",
                "RIGHT",
                "BACK",
                "FIRE",
                "RIGHTCLICK",
                "RELOAD",
                "INSPECT",
                "USE",
                "ZOOM",
                "SCOREBOARD",
                "WALK",
                # Player
                "team_name",
                "team_clan_name",
                "X",
                "Y",
                "Z",
                "pitch",
                "yaw",
                "last_place_name",
                "is_walking",
                "is_strafing",
                "in_crouch",
                "health",
                "armor",
                "has_defuser",
                "has_helmet",
                "inventory",
                "current_equip_value",
                "active_weapon",
                "rank",
                "ping",
                # Game State
                "is_bomb_planted",
                # State for filtering
                "is_freeze_period",
                "is_warmup_period",
                "is_terrorist_timeout",
                "is_ct_timeout",
                "is_technical_timeout",
                "is_waiting_for_resume",
                "is_match_started",
                "game_phase",
            ]
        )
        ticks = self.remove_nonplay_ticks(ticks)
        return ticks.rename(
            columns={
                "FORWARD": "key_forward",
                "LEFT": "key_left",
                "RIGHT": "key_right",
                "BACK": "key_back",
                "FIRE": "key_fire",
                "RIGHTCLICK": "key_rightclick",
                "RELOAD": "key_reload",
                "INSPECT": "key_inspect",
                "USE": "key_use",
                "ZOOM": "key_zoom",
                "SCOREBOARD": "key_scoreboard",
                "WALK": "key_walk",
                "active_weapon": "weapon",
                "last_place_name": "place",
                "team_name": "side",
                "team_clan_name": "clan",
            }
        )
    
    def parse_demo(self) -> dict[str, Any]:
        header = self.parser.parse_header()
        events = dict(
            self.parser.parse_events(
                self.parser.list_game_events() + ['round_end', 'round_start'],
                player=[
                    "X",
                    "Y",
                    "Z",
                    "last_place_name",
                    "flash_duration",
                    "is_strafing",
                    "accuracy_penalty",
                    "zoom_lvl",
                    "health",
                    "armor",
                    "inventory",
                    "current_equip_value",
                    "rank",
                    "ping",
                    "has_defuser",
                    "has_helmet",
                    "pitch",
                    "yaw",
                    "team_name",
                    "team_clan_name",
                ],
                other=[
                    # Bomb
                    "is_bomb_planted",
                    "which_bomb_zone",
                    # State
                    "is_freeze_period",
                    "is_warmup_period",
                    "is_terrorist_timeout",
                    "is_ct_timeout",
                    "is_technical_timeout",
                    "is_waiting_for_resume",
                    "is_match_started",
                    "game_phase",
                ],
            )
        )
        
        kills = self.parse_kills(events)
        damages = self.parse_damages(events)
        # bomb = self.parse_bomb(events)
        smokes = self.parse_smokes(events)
        infernos = self.parse_infernos(events)
        weapon_fires = self.parse_weapon_fires(events)
        grenades = self.parse_grenades()
        ticks = self.parse_ticks()
        
        return {
            # "parser": parser,
            "header": header,
            "events": events,
            # Parsed from event dictionary
            "kills": kills,
            "damages": damages,
            # "bomb": bomb,
            "smokes": smokes,
            "infernos": infernos,
            "weapon_fires": weapon_fires,
            # Parsed from parser
            "grenades": grenades,
            "ticks": ticks,
        }
    
class GetgudCS2Parser:
    def __init__(self, sdk, dem_file_path, report_players=[]):
        self.sdk = sdk
        self.report_players = report_players
        self.dem_file_path = dem_file_path

        self.title_id = int(os.environ.get("GETGUD_TITLE_ID"))
        self.private_key = os.environ.get(
            "GETGUD_PRIVATE_KEY"
        )

        self.sdk_commands = []
    
    def start(self):
        print('[Parser] Starting demo parsing..')
        demo_data = self.parse_cs2_dem_file()
        if demo_data is None:
            return "skipped"
        print('[Parser] Demo parsed!')
        print('[Parser] Pushing commands into SDK queue...')
        self.game_start_time_in_milliseconds = round(time.time() * 1000)
        game_guid, sdk_commands = self.push_sdk_commands_from_cs2_dem(demo_data)
        print('[Parser] Pushed commands to SDK queue!')
        print('[Parser] Running SDK on queue...')
        self.run_sdk(sdk_commands)
        print('[Parser] Finished running SDK on queue!')
        self.dispose()
        
        return game_guid
        
    
    def parse_cs2_dem_file(self):
        if self.dem_file_path is None:
            return None
        parser = GetgudDemoParser(self.dem_file_path)
        demo_data = parser.parse_demo()
        return demo_data
    
    
    def push_sdk_commands_from_cs2_dem(self, demo_data):
        tick_rate = 64
        
        # no ticks, we will not process the match
        if (demo_data['ticks'].shape[0] == 0):
            return []

        # Define the pattern to keep only the allowed characters
        allowed_pattern = r'[^a-zA-Z0-9\-_]'
        # Start the game session and get the game GUID
        server_location = demo_data['header']['server_name'].replace('Valve Counter-Strike 2 ', '').split()[0]
        if server_location == '': server_location = 'us_default'
        server_location = re.sub(allowed_pattern, '', server_location)
        
        server_guid = demo_data['header']['server_name'].replace(' ', '-')[:CHAR_LIMIT['small']]
        server_guid = re.sub(allowed_pattern, '', server_guid)
        
        game_mode = demo_data['header']['client_name'].replace(' ', '-')[:CHAR_LIMIT['small']]
        game_mode = re.sub(allowed_pattern, '', game_mode)

        
        game_guid = StartGame(
            self.title_id, 
            self.private_key, 
            server_guid,
            game_mode,
            server_location,
        ).call(self.sdk)

        # Set the match start time once for the entire game
        self.match_start_time_in_milliseconds = round(time.time() * 1000)

        sdk_commands = []
        match_guids = []
                
        # Process each match ("round")
        extra_ticks = 128
        for round_num in range(len(demo_data['events']['round_start'])):
            round_start_tick = demo_data['events']['round_start'].iloc[round_num]['tick']
            # for every round except last we take end tick as the start tick of next round
            round_end_tick = demo_data['events']['round_end'].iloc[round_num]['tick'] + extra_ticks
            
            kill_match_data = demo_data['kills'].loc[(demo_data['kills']['tick'] >= round_start_tick) & (demo_data['kills']['tick'] <= round_end_tick)].reset_index(drop = True)
            damage_match_data = demo_data['damages'].loc[(demo_data['damages']['tick'] >= round_start_tick) & (demo_data['damages']['tick'] <= round_end_tick)].reset_index(drop = True)
            weapon_fires_match_data = demo_data['weapon_fires'].loc[(demo_data['weapon_fires']['tick'] >= round_start_tick) & (demo_data['weapon_fires']['tick'] <= round_end_tick)].reset_index(drop = True)
            tick_match_data = demo_data['ticks'].loc[(demo_data['ticks']['tick'] >= round_start_tick) & (demo_data['ticks']['tick'] <= round_end_tick)].reset_index(drop = True)
            spawn_match_data = demo_data['events']['player_spawn'].loc[(demo_data['events']['player_spawn']['tick'] >= round_start_tick) & (demo_data['events']['player_spawn']['tick'] <= round_end_tick)].reset_index(drop = True)
            
            # don't start round/push match if we couldn't find any spawn or tick data
            if (spawn_match_data.shape[0] == 0 or tick_match_data.shape[0] == 0):
                continue
                
            # Start a new match and record its GUID
            match_guid = StartMatch(
                game_guid,
                "5v5",
                demo_data['header']['map_name'].replace(' ', '-')[:CHAR_LIMIT['small']],
                self.dem_file_path.split('/')[-1]
            ).call(self.sdk)
            match_guids.append(match_guid)
            
             # Process kills for this match
            for _, kill_data in kill_match_data.iterrows():
                sdk_commands.append([
                    round(self.game_start_time_in_milliseconds + (kill_data['tick'] / tick_rate)*1000),
                    DeathActionData(
                        match_guid,
                        round(self.game_start_time_in_milliseconds + (kill_data['tick'] / tick_rate)*1000),
                        str(kill_data['victim_steamid']),
                        str(kill_data['attacker_steamid']) if str(kill_data['attacker_steamid']) != '<NA>' else 'PvE' 
                    )
                ])
            
            # Process damages for this match
            for _, damage_row in damage_match_data.iterrows():
                attacker_steamid = damage_row['attacker_steamid']
                victim_steamid = damage_row['victim_steamid']
                damage = damage_row['dmg_health'] + damage_row['dmg_armor']
                weapon = damage_row['weapon'].replace(' ', '-')[:CHAR_LIMIT['small']]
                
                # getgud requires attack and damage guids to be the same
                # we are replacing inferno with molotov because that's how it's called during
                # attack event
                if weapon == 'inferno':
                    weapon = 'molotov'
                
                if pd.isna(attacker_steamid) or attacker_steamid == '' or attacker_steamid is None:
                    attacker_steamid = 'PvE'
                    weapon = 'PvE'
                    
                sdk_commands.append([
                    round(self.game_start_time_in_milliseconds + (damage_row['tick'] / tick_rate) * 1000),
                    DamageActionData(
                        match_guid,
                        round(self.game_start_time_in_milliseconds + (damage_row['tick'] / tick_rate) * 1000),
                        attacker_steamid,
                        victim_steamid,
                        damage,
                        weapon
                    )
                ])

            # Process weapon fires for this match
            for _, fire_row in weapon_fires_match_data.iterrows():
                sdk_commands.append([
                    round(self.game_start_time_in_milliseconds + (fire_row['tick'] / tick_rate) * 1000),
                    AttackActionData(
                        match_guid,
                        round(self.game_start_time_in_milliseconds + (fire_row['tick'] / tick_rate) * 1000),
                        fire_row['player_steamid'],
                        fire_row['weapon'].replace(' ', '').replace('weapon_', '').replace('_silencer', '')[:CHAR_LIMIT['small']]
                    )
                ])
            
            # Send spawn events from spawn event data
            player_match_spawns = spawn_match_data[(spawn_match_data['tick']>=round_start_tick) & (spawn_match_data['tick']<=round_end_tick)]
            
            for _, spawn_row in player_match_spawns.iterrows():
                player_health = 100
                x,y,z = spawn_row['user_X'], spawn_row['user_Y'], spawn_row['user_Z']
                pitch, yaw = spawn_row['user_pitch'], spawn_row['user_yaw']

                
                # we need to grab player health from firs tick of match because 
                # health seems not to be always available on spawn
                for _, tick_row in tick_match_data.iterrows():
                    player_id = str(tick_row['steamid'])
                    if player_id == spawn_row['user_steamid']:
                        player_health = tick_row['health']
                        x,y,z = tick_row['X'], tick_row['Y'], tick_row['Z']
                        pitch, yaw = tick_row['pitch'], tick_row['yaw']
                        break
                    
                    
                player_team = spawn_row['user_team_name'] 
                
                # renaming for consistency with match win team
                if player_team == 'TERRORIST':
                    player_team = 'T'
                
                # it's likely a spectator or a bug, we are not going
                # to spawn this player
                if player_team != 'T' and player_team != 'CT':
                    continue
                
                sdk_commands.append([
                        round(self.match_start_time_in_milliseconds + (spawn_row['tick'] / tick_rate) * 1000),
                        SpawnActionData(
                            match_guid,
                            round(self.match_start_time_in_milliseconds + (spawn_row['tick'] / tick_rate) * 1000),
                            spawn_row['user_steamid'],
                            player_team, # character guid
                            player_team, # team guid
                            player_health, # todo: where to get armor? 
                            Position(x,y,z),
                            Rotation(pitch, yaw, 0),
                        )
                    ])
            
            # Log position for all ticks in the round
            for _, tick_row in tick_match_data.iterrows():
                player_id = tick_row['steamid']
                sdk_commands.append([
                    round(self.match_start_time_in_milliseconds + (tick_row['tick'] / tick_rate) * 1000),
                    PositionActionData(
                        match_guid,
                        round(self.match_start_time_in_milliseconds + (tick_row['tick'] / tick_rate) * 1000),
                        player_id,
                        Position(tick_row['X'], tick_row['Y'], tick_row['Z']),
                        Rotation(tick_row['pitch'], tick_row['yaw'], 0),
                    )
                ])

            round_end_event = demo_data['events']['round_end'].loc[
                (demo_data['events']['round_end']['tick'] > round_start_tick) & 
                (demo_data['events']['round_end']['tick'] <= round_end_tick)
            ]
            
            winning_team = "unknown"
            if round_end_event.shape[0] > 0:
                winning_team = round_end_event.iloc[0].get('winner', "unknown")

            sdk_commands.append([
                round(self.match_start_time_in_milliseconds + (tick_row['tick'] / tick_rate) * 1000),
                SetMatchWinTeam(match_guid, winning_team)
            ])

        # Sort SDK commands by timestamp
        sdk_commands.sort(key=lambda x: x[0])
        
        # Finalize the SDK commands
        sdk_output_commands = []
        for (timestamp, command) in sdk_commands:
            sdk_output_commands.append(command)

        # Generate reports and mark the end of the game
        last_timestamp = sdk_commands[-1][0] if sdk_commands else self.match_start_time_in_milliseconds
        for match_guid in match_guids:
            for report_player_id in self.report_players:
                sdk_output_commands.append(SendInMatchReport(
                    ReportInfo(
                        match_guid,
                        "csstats", 
                        1, 
                        1, 
                        report_player_id, 
                        1,  # 0 - aimbot, 1 - wallhack, 5 - spinbot
                        last_timestamp,
                        100,
                        last_timestamp
                    )
                ))
        
        # Mark the end of the game
        sdk_output_commands.append(MarkEndGame(game_guid))

        return game_guid, sdk_output_commands     
    
    def run_sdk(self, sdk_commands):
        output_sdk_string = ""
        if sdk_commands == []:
            print(f'[Parser] Game from {self.resolved_url} is empty, we will not push it!')
        for command in sdk_commands:
            command.call(self.sdk)
            time.sleep(0.00000125)


    def dispose(self):
        pass
    
   
    
    