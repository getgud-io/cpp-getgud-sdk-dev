# ruff: noqa: N806,N815,N816
import base64
import json
import zlib
from typing import Any, Tuple, Dict
import re
import math
from collections import defaultdict
from itertools import chain

import numpy as np

ATTACK = "A"
DAMAGE = "D"
HEAL = "H"
SPAWN = "S"
DEATH = "DD"
POSITION = "P"
AFFECT = "AF"
DELIMITER = ","
PvE = "PvE"
COMPLETE_POSITION = "F"
PLAYER_MAP = "playerMap"
WEAPON_MAP = "weaponMap"
PLAYER_DELIMITER = "~G~"
WEAPON_DELIMITER = "~W~"
ACTION_TIME_IDX = 0
ACTION_TYPE_IDX = 1
TEAM_GUID_IDX = 4
CHARACTER_GUID_IDX = 3
SPAWN_POSITION_IDX = 6
FIRST_PLAYER_IDX = 2
SECOND_PLAYER_IDX = 3
INITIAL_HEALTH_IDX = 5
DAMAGE_DONE_IDX = 4
POSITION_IDX = 3
DAMAGE_WEAPON_KEY_IDX = 5
ATTACK_WEAPON_KEY_IDX = 3
DMG_IDX_TO_MATCH_NOP_IDX = 7

# Pointer constants for action stream indexes
DAMAGE_ATTACKER_FULL_STREAM_POINTER = -3
DAMAGE_MATCH_NOP_STREAM_POINTER = -2
DAMAGE_VICTIM_FULL_STREAM_POINTER = -1
ATTACK_PLAYER_FULL_STREAM_POINTER = -2
ATTACK_MATCH_NOP_STREAM_POINTER = -1

AFFECT_STATE_STACK_POINTER_JUMPS = 4
AFFECT_STATE_ATTACH = 0
AFFECT_STATE_ACTIVATE = 1
AFFECT_STATE_DEACTIVATE = 2
AFFECT_STATE_DETACH = 3
AFFECT_ID_STACK_POINTER_JUMPS = 3

ATTACK_STACK_POINTER_JUMPS = 4
DAMAGE_STACK_POINTER_JUMPS = 6
HEAL_STACK_POINTER_JUMPS = 4
SPAWN_STACK_POINTER_JUMPS = 7
DEATH_STACK_POINTER_JUMPS = 4
TEAM_ID_STACK_POINTER_JUMPS = 4
POSITION_STACK_POINTER_JUMPS = 4
FIRST_PLAYER_GUID_STACK_POINTER_JUMPS = 2
SECOND_PLAYER_GUID_STACK_POINTER_JUMPS = 3
AFFECT_STACK_POINTER_JUMPS = 5

DAMAGE_ACTION_TO_WEAPON_GUID_POINTER_JUMPS = 5
ATTACK_ACTION_TO_WEAPON_GUID_POINTER_JUMPS = 3

SPAWN_ACTION_TO_POSITION_POINTER_JUMPS = 6
POSITION_ACTION_TO_POSITION_POINTER_JUMPS = 3

actionSPMap = {
    POSITION: POSITION_STACK_POINTER_JUMPS,
    ATTACK: ATTACK_STACK_POINTER_JUMPS,
    DAMAGE: DAMAGE_STACK_POINTER_JUMPS,
    HEAL: HEAL_STACK_POINTER_JUMPS,
    SPAWN: SPAWN_STACK_POINTER_JUMPS,
    DEATH: DEATH_STACK_POINTER_JUMPS,
    AFFECT: AFFECT_STACK_POINTER_JUMPS,
}

PVE_SYMBOL = 'PvE'


class Utils:
    @staticmethod
    def parse_map(map_string: str, delimiter: str) -> Dict[str, str]:
        map_dict = {}
        pairs = re.split(f"{delimiter}|{PLAYER_DELIMITER}|{WEAPON_DELIMITER}", map_string)
        pairs = [p for p in pairs if p]
        for i in range(0, len(pairs), 2):
            if i + 1 < len(pairs):
                map_dict[pairs[i]] = pairs[i + 1]
        return map_dict
        
    @staticmethod
    def parse_map_data(map_string: str, split_by_delimiter: str = "\n") -> list:
        """Parse map data that is in string format.

        Args:
            map_string (str): String representing the map data
            split_by_delimiter (str, optional): Delimiter to split the lines. Defaults to "\n".

        Returns:
            list: List of parsed points
        """
        if not map_string:
            return []
            
        data_points = []
        lines = map_string.strip().split(split_by_delimiter)
        
        for line in lines:
            if not line.strip():
                continue
                
            try:
                # Split the line and convert values to appropriate types
                values = line.strip().split(',')
                if len(values) < 3:  # Need at least x, y, z coordinates
                    continue
                    
                point = [
                    float(values[0]),  # x
                    float(values[1]),  # y
                    float(values[2])   # z
                ]
                
                # Add any additional values if present
                for i in range(3, len(values)):
                    try:
                        # Try to convert to float first, then int, falling back to string
                        try:
                            val = float(values[i])
                            # If it's actually an integer, convert it to int
                            if val.is_integer():
                                val = int(val)
                        except ValueError:
                            val = values[i]
                        point.append(val)
                    except:
                        pass
                
                data_points.append(point)
            except:
                continue
                
        return data_points
    
    @staticmethod
    def new_parse_map(map_string: str, delimiter: str) -> Dict[str, str]:
        
        map_dict = {}
        
        # Split by the passed delimiter
        pairs = map_string.split(delimiter)
        
        # Filter out any empty pairs
        pairs = [p for p in pairs if p]
        
        # Populate the map_dict with key-value pairs
        for i in range(0, len(pairs), 2):
            if i + 1 < len(pairs):
                map_dict[pairs[i]] = pairs[i + 1]
        
        return map_dict

    @staticmethod
    def extract_maps(action_stream: str) -> Tuple[Dict[str, str], Dict[str, str], str]:
        parts = action_stream.split(DELIMITER)
        player_map = {}
        weapon_map = {}
        stream_start = 0

        for i, part in enumerate(parts):
            if PLAYER_MAP in part or WEAPON_MAP in part:
                map_strings = re.split(f"({PLAYER_MAP}|{WEAPON_MAP})", part)
                map_strings = [s for s in map_strings if s]

                for j in range(0, len(map_strings), 2):
                    if j + 1 < len(map_strings):
                        map_type = map_strings[j]
                        map_data = map_strings[j + 1]
                        if map_type == PLAYER_MAP:
                            player_map = Utils.parse_map(map_data, PLAYER_DELIMITER)
                        elif map_type == WEAPON_MAP:
                            weapon_map = Utils.parse_map(map_data, WEAPON_DELIMITER)

                stream_start = i + 1
                if player_map and weapon_map:
                    break
            else:
                break

        return player_map, weapon_map, DELIMITER.join(parts[stream_start:])
    
    @staticmethod
    def new_extract_maps(action_stream: str) -> Tuple[Dict[str, str], Dict[str, str], str, Dict[str, str], Dict[str, str]]:

        first_part, _, remaining_stream = action_stream.partition(DELIMITER)
        
        # Initialize empty maps
        player_map = {}
        weapon_map = {}
        reverse_player_map = {}
        reverse_weapon_map = {}

        # Check if the player map exists in the first part
        if PLAYER_MAP in first_part:

            # Extract and parse the player map
            player_map = Utils.new_parse_map(first_part.split(PLAYER_MAP)[-1], PLAYER_DELIMITER)
            
            # Create reverse player map
            for key, value in player_map.items():
                reverse_player_map[value] = key
            
            # Check if the player map contains a weapon map as a key
            if WEAPON_MAP in player_map:
                weapon_map_data = player_map.pop(WEAPON_MAP)
                weapon_map = Utils.new_parse_map(weapon_map_data, WEAPON_DELIMITER)
                
                # Create reverse weapon map
                for key, value in weapon_map.items():
                    reverse_weapon_map[value] = key
        else: 
            remaining_stream = action_stream

        # Return the player map, weapon map, and the remaining stream, plus the reverse maps
        return player_map, weapon_map, remaining_stream, reverse_player_map, reverse_weapon_map

    @staticmethod
    def parse_action_stream(action_stream: str) -> dict:
        player_map, weapon_map, cleaned_stream = Utils.extract_maps(action_stream)
        
        playerMatchData = {
            "reconstructedMatchesDic": {},
            "reconstructedPlayersDic": {},
            "reconstructedPlayerChatDic": {},
            "matchStatisticsDic": {},
            "reportArray": [],
            "gameSizeInBytes": 0,
        }
        lastActionPerPlayer = {}

        splitStream = cleaned_stream.split(DELIMITER)

        if splitStream[-1] == "":
            splitStream = splitStream[:-1]
        if splitStream[0] == "":
            splitStream = splitStream[1:]
        index = 0
        # we will increment index ourselves in the end of loop
        while index < len(splitStream):
            if splitStream[index] == "":
                index += 1
                continue

            # we have dynamic timestamps, so all timestamps after the first
            # should be dynamically converted
            if index == 0:
                actionTime = int(splitStream[index])
                playerMatchData["matchStatisticsDic"]["matchStartTime"] = actionTime
            else:
                actionTime += int(splitStream[index])

            actionType = splitStream[index + 1]
            actionSPJ = actionSPMap[actionType]

            # grab full action from list
            action = splitStream[index:index + actionSPJ]
            action[0] = actionTime  # replace delta time with full timestamp
            
            playerGuid = str(action[FIRST_PLAYER_GUID_STACK_POINTER_JUMPS])
            playerName = player_map.get(playerGuid, playerGuid)
            action[FIRST_PLAYER_GUID_STACK_POINTER_JUMPS] = playerName

            # Map weapon GUIDs and second player for specific action types
            if actionType == DAMAGE:
                weaponGuid = str(action[DAMAGE_ACTION_TO_WEAPON_GUID_POINTER_JUMPS])
                weaponName = weapon_map.get(weaponGuid, weaponGuid)
                action[DAMAGE_ACTION_TO_WEAPON_GUID_POINTER_JUMPS] = weaponName

                secondPlayerGuid = str(action[SECOND_PLAYER_GUID_STACK_POINTER_JUMPS])
                secondPlayerName = player_map.get(secondPlayerGuid, secondPlayerGuid)
                action[SECOND_PLAYER_GUID_STACK_POINTER_JUMPS] = secondPlayerName
            elif actionType == ATTACK:
                weaponGuid = str(action[ATTACK_ACTION_TO_WEAPON_GUID_POINTER_JUMPS])
                weaponName = weapon_map.get(weaponGuid, weaponGuid)
                action[ATTACK_ACTION_TO_WEAPON_GUID_POINTER_JUMPS] = weaponName
            elif actionType == DEATH:
                secondPlayerGuid = str(action[SECOND_PLAYER_GUID_STACK_POINTER_JUMPS])
                secondPlayerName = player_map.get(secondPlayerGuid, secondPlayerGuid)
                action[SECOND_PLAYER_GUID_STACK_POINTER_JUMPS] = secondPlayerName

            # fill parts of reconstructed dic if they are empty
            if not playerMatchData["reconstructedPlayersDic"].get(playerName, None):
                playerMatchData["reconstructedPlayersDic"][playerName] = {
                    "playerId": None,
                    "teamId": None
                }

            if not playerMatchData["reconstructedPlayersDic"][playerName].get("match", None):
                playerMatchData["reconstructedPlayersDic"][playerName]["match"] = {
                    "actionStream": [],
                }

            if not playerMatchData["matchStatisticsDic"].get("uniquePlayerIds", None):
                playerMatchData["matchStatisticsDic"]["uniquePlayerIds"] = {}
                playerMatchData["matchStatisticsDic"]["uniqueTeamIds"] = {}

            if not playerMatchData["matchStatisticsDic"]["uniquePlayerIds"].get(playerName, None):
                playerMatchData["matchStatisticsDic"]["uniquePlayerIds"][playerName] = playerName

            if actionType != DAMAGE:
                if actionType == SPAWN:
                    teamId = action[TEAM_ID_STACK_POINTER_JUMPS]
                    playerMatchData["reconstructedPlayersDic"][playerName]["teamId"] = teamId
                    if not playerMatchData["matchStatisticsDic"]["uniqueTeamIds"].get(teamId, None):
                        playerMatchData["matchStatisticsDic"]["uniqueTeamIds"][teamId] = teamId

                # remove the player id from the "player action stream",
                # we already know who the player is
                action.pop(FIRST_PLAYER_GUID_STACK_POINTER_JUMPS)

            # position is in delta format so we need to transform them the same way as
            # timestamps
            if actionType == POSITION:
                lastActionData = lastActionPerPlayer.get(playerName)
                if lastActionData is None:
                    lastActionData = {
                        "x": 0,
                        "y": 0,
                        "z": 0,
                        "yaw": 0,
                        "pitch": 0,
                        "roll": 0
                    }
                extended_action_elements = list(map(float, action[-1].split("~"))) + [0] * 6
                newDeltaX, newDeltaY, newDeltaZ, newDeltaYaw, newDeltaPitch, newDeltaRoll = extended_action_elements[:6]
                newX = newDeltaX + lastActionData["x"]
                newY = newDeltaY + lastActionData["y"]
                newZ = newDeltaZ + lastActionData["z"]
                newYaw = newDeltaYaw + lastActionData["yaw"]
                newPitch = newDeltaPitch + lastActionData["pitch"]
                newRoll = newDeltaRoll + lastActionData["roll"]

                action[-1] = "~".join(
                    [f"{item:.6f}" for item in [newX, newY, newZ, newYaw, newPitch, newRoll]]
                )
                lastActionData["x"] = newX
                lastActionData["y"] = newY
                lastActionData["z"] = newZ
                lastActionData["yaw"] = newYaw
                lastActionData["pitch"] = newPitch
                lastActionData["roll"] = newRoll
                lastActionPerPlayer[playerName] = lastActionData

            playerMatchData["reconstructedPlayersDic"][playerName]["match"]["actionStream"].append(action)

            if actionType == DAMAGE:
                secondPlayerName = action[SECOND_PLAYER_GUID_STACK_POINTER_JUMPS]

                if not playerMatchData["reconstructedPlayersDic"].get(secondPlayerName, None):
                    playerMatchData["reconstructedPlayersDic"][secondPlayerName] = {
                        "playerId": None,
                        "teamId": None
                    }

                if not playerMatchData["reconstructedPlayersDic"][secondPlayerName].get("match", None):
                    playerMatchData["reconstructedPlayersDic"][secondPlayerName]["match"] = {
                        "actionStream": [],
                    }
                playerMatchData["reconstructedPlayersDic"][secondPlayerName]["match"]["actionStream"].append(action)

            index += actionSPJ

        return playerMatchData
    
    @staticmethod
    def new_parse_action_stream(action_stream: str, data: dict = None) -> dict:

        index : int = 0
        match_action_count : int = 0
        nop_match_action_count : int = 0
        actionTime : int = None
        actionType: str = None
        actionSPJ: int = None
        player_guid_key: str = None
        second_player_guid_key: str = None
        playerGuid: str = None
        second_player_guid: str = None
        teamGuid: str = None
        extended_action_elements = None
        lastPlayerPosition = None
        lastPlayerPositionDict = {}

        # Default normalization parameters if data isn't provided
        max_x_normalization = 100
        min_x_normalization = 0
        max_y_normalization = 100
        min_y_normalization = 0
        max_z_normalization = 100
        min_z_normalization = 0
        
        # Get normalization parameters from data if available
        if data and "data" in data and "titleModel" in data["data"]:
            normalization_params = data["data"]["titleModel"].get("normalizationParameters", {})
            max_x_normalization = normalization_params.get("maxX", 100)
            min_x_normalization = normalization_params.get("minX", 0)
            max_y_normalization = normalization_params.get("maxY", 100)
            min_y_normalization = normalization_params.get("minY", 0)
            max_z_normalization = normalization_params.get("maxZ", 100)
            min_z_normalization = normalization_params.get("minZ", 0)
        
        # Calculate diagonal length for normalization
        diag_len = float(np.sqrt(
            float((max_x_normalization - min_x_normalization) ** 2) + 
            float((max_y_normalization - min_y_normalization) ** 2) + 
            float((max_z_normalization - min_z_normalization) ** 2)
        ))

        # holds the match's action data, including the actions per player 
        parsed_action_data = {
            "player_map": {},
            "weapon_map": {},
            "reverse_player_map": {},
            "reverse_weapon_map": {},
            "full_action_stream": [],
            "nop_action_stream": [],
            "player_dict": {}
        }

        # extract the player map and weapon map from the action stream, if available
        parsed_action_data["player_map"], parsed_action_data["weapon_map"], cleanActionStream, parsed_action_data["reverse_player_map"], parsed_action_data["reverse_weapon_map"] = Utils.new_extract_maps(action_stream)

        # split the stream using the delimiter in so we can traverse through each element of all actions 
        splitStream = cleanActionStream.split(DELIMITER)
        
        # check if the stream ends and/or begins with a Delimiter, causing an empty action, if so, remove the empty actions
        if splitStream and splitStream[-1] == "": splitStream = splitStream[:-1]
        if splitStream and splitStream[0] == "": splitStream = splitStream[1:]
        splitStreamLength = len(splitStream)
        
        # go over all the splitted elements to recreate actions. We will increment index ourselves in the end of loop
        while index < splitStreamLength:

            # we have dynamic timestamps, so all timestamps after the first
            # should be dynamically converted
            if index == 0: actionTime = int(splitStream[index])
            else: actionTime += int(splitStream[index])

            # extract the action type symbol
            actionType = splitStream[index + 1]

            # extract the number of elements this action contains
            actionSPJ = actionSPMap[actionType]

            # grab full action from the slipt list
            action = splitStream[index:index + actionSPJ]
            action[0] = actionTime  # replace delta time with full timestamp
            
            # grab the player guid that is associated with the action we are iterating over
            player_guid_key = str(action[FIRST_PLAYER_GUID_STACK_POINTER_JUMPS])
            playerGuid = parsed_action_data["player_map"].get(player_guid_key, player_guid_key)

            # fill parts of reconstructed dict if they are empty
            if not parsed_action_data["player_dict"].get(playerGuid, None):
                parsed_action_data["player_dict"][playerGuid] = {
                    "team_guid": None,
                    "player_action_count": 0,
                    "player_full_action_stream": [],
                    "player_nop_action_stream": [],
                }
            
            # construct the full, parsed action stream for the match and the player, containing all his actions, including position actions
            parsed_action_data["full_action_stream"].append(action)
            parsed_action_data["player_dict"][playerGuid]["player_full_action_stream"].append(action)

            # count the number of actions per player and for this match, we use this index as a pointer in the nop streams
            match_action_count += 1
            parsed_action_data["player_dict"][playerGuid]["player_action_count"] += 1

            if actionType == POSITION:
                
                # check if this position is in it's complete format, meaning not in a delta format
                # This will be the case if this is the first position in the packet stream, which are now stitched to one holistic stream 
                if action[-1][-1] == COMPLETE_POSITION:

                    # remove the signal indicating that this is a complete position and save it as is in the last player position dict
                    action[-1] = action[-1][:-1]
                    extended_action_elements = [float(x) for x in action[-1].split("~")]
                    extended_action_elements.extend([0] * (6 - len(extended_action_elements)))
                    newX, newY, newZ, newYaw, newPitch, newRoll = extended_action_elements[:6]
                    lastPlayerPosition = {
                        "x": newX,
                        "y": newY,
                        "z": newZ,
                        "yaw": newYaw,
                        "pitch": newPitch,
                        "roll": newRoll
                        }
                    lastPlayerPositionDict[playerGuid] = lastPlayerPosition

                else:

                    # position is in delta format so we need to transform them the same way as timestamps
                    lastPlayerPosition = lastPlayerPositionDict.get(playerGuid)
                    if lastPlayerPosition is None:
                        lastPlayerPosition = {
                            "x": 0,
                            "y": 0,
                            "z": 0,
                            "yaw": 0,
                            "pitch": 0,
                            "roll": 0
                        }
                    extended_action_elements = [float(x) for x in action[-1].split("~")]
                    extended_action_elements.extend([0] * (6 - len(extended_action_elements)))
                    newDeltaX, newDeltaY, newDeltaZ, newDeltaYaw, newDeltaPitch, newDeltaRoll = extended_action_elements[:6]
                    newX = newDeltaX + lastPlayerPosition["x"]
                    newY = newDeltaY + lastPlayerPosition["y"]
                    newZ = newDeltaZ + lastPlayerPosition["z"]
                    newYaw = newDeltaYaw + lastPlayerPosition["yaw"]
                    newPitch = newDeltaPitch + lastPlayerPosition["pitch"]
                    newRoll = newDeltaRoll + lastPlayerPosition["roll"]

                    action[-1] = "~".join([f"{item:.3f}" for item in [newX, newY, newZ, newYaw, newPitch, newRoll]])
                    lastPlayerPosition["x"] = newX
                    lastPlayerPosition["y"] = newY
                    lastPlayerPosition["z"] = newZ
                    lastPlayerPosition["yaw"] = newYaw
                    lastPlayerPosition["pitch"] = newPitch
                    lastPlayerPosition["roll"] = newRoll
                    lastPlayerPositionDict[playerGuid] = lastPlayerPosition

            # normalize the coordinates in the position and spawn actions
            if actionType == POSITION or actionType == SPAWN:

                # initialize extended_action_elements only if this is a spawn action, if it's position action this was initialized already
                if actionType == SPAWN: 
                    extended_action_elements = [float(x) for x in action[-1].split("~")]
                    extended_action_elements.extend([0] * (6 - len(extended_action_elements)))
                    x, y, z, yaw, pitch, roll = extended_action_elements[:6]
                if actionType == POSITION:
                    x, y, z, yaw, pitch, roll = newX, newY, newZ, newYaw, newPitch, newRoll
                
                # Apply normalization if data is provided
                if data:
                    x_normalized = 100 * (x - min_x_normalization) / (diag_len - 0)
                    y_normalized = 100 * (y - min_y_normalization) / (diag_len - 0)
                    z_normalized = 100 * (z - min_z_normalization) / (diag_len - 0)
                    action[-1] = "~".join([f"{x:.3f}" for x in (x_normalized, y_normalized, z_normalized, yaw, pitch, roll)])

            # build the nop action streams - without positions
            if actionType != POSITION:

                # team guid resides in the spawn action, extract it and associate to the relevant player 
                if actionType == SPAWN:
                    teamGuid = action[TEAM_ID_STACK_POINTER_JUMPS]
                    parsed_action_data["player_dict"][playerGuid]["team_guid"] = teamGuid

                # construct the parsed, partial action stream for the match and the player without position actions
                # in the no position action streams (AKA nop), add the index of this action in the full action streams for fast access
                match_action = action[:]
                match_action.append(match_action_count - 1) # pointer to match full action stream
                nop_match_action_count += 1
                parsed_action_data["nop_action_stream"].append(match_action)
                player_action = action[:]
                player_action.append(parsed_action_data["player_dict"][playerGuid]["player_action_count"] - 1) # pointer to player full action stream
                player_action.append(nop_match_action_count - 1) # pointer to match nop action stream

                # if this is a Damage or Death actions, there is a second player involved -> add a pointer to its player full action stream
                if (actionType == DAMAGE or actionType == DEATH):
                    second_player_guid_key = str(action[SECOND_PLAYER_IDX])
                    second_player_guid = parsed_action_data["player_map"].get(second_player_guid_key, second_player_guid_key)
                    if second_player_guid in parsed_action_data["player_dict"]:
                        player_action.append(parsed_action_data["player_dict"][second_player_guid]["player_action_count"] - 1) # pointer to second player full action stream

                parsed_action_data["player_dict"][playerGuid]["player_nop_action_stream"].append(player_action)

            # jump to the next action
            index += actionSPJ
            
        # Create player alive intervals
        parsed_action_data["player_alive_intervals"] = Utils.create_player_alive_intervals(parsed_action_data)
        
        return parsed_action_data

    @staticmethod
    def deflate_string(obj_str: str) -> str:
        """Compress string."""
        json_bytes = obj_str.encode("utf-8")
        compressed_data = zlib.compress(json_bytes)
        # Encode compressed data to base64 to ensure JSON compatibility
        return base64.b64encode(compressed_data).decode("utf-8")

    @staticmethod
    def inflate_string(encoded_data: str) -> Any:
        """Decompress data into string."""
        compressed_data = base64.b64decode(encoded_data)
        json_bytes = zlib.decompress(compressed_data, zlib.MAX_WBITS | 32)
        return json_bytes.decode("utf-8")

    @staticmethod
    def deflate_json(obj: Any) -> str:
        """Compress json."""
        json_str = json.dumps(obj)
        json_bytes = json_str.encode("utf-8")
        compressed_data = zlib.compress(json_bytes)
        # Encode compressed data to base64 to ensure JSON compatibility
        return base64.b64encode(compressed_data).decode("utf-8")

    @staticmethod
    def inflate_json(encoded_data: str) -> Any:
        """Decompress data into json."""
        compressed_data = base64.b64decode(encoded_data)
        json_bytes = zlib.decompress(compressed_data, zlib.MAX_WBITS | 32)
        json_str = json_bytes.decode("utf-8")
        return json.loads(json_str)

    @staticmethod
    def create_player_alive_intervals(parsed_action_stream_data: dict) -> dict:
        """Creates intervals of when players were alive during the match.
        
        Args:
            parsed_action_stream_data (dict): Parsed action stream data
            
        Returns:
            dict: Dictionary mapping player GUIDs to their alive intervals
        """
        # Initialize a defaultdict to store alive intervals for each player
        player_alive_intervals = defaultdict(list)
        nop_stream_length = 0

        # Iterate through each player's data in the parsed action stream
        for player_guid, player_data in parsed_action_stream_data["player_dict"].items():            
            # Initialize variables to track the current alive interval
            current_interval_start = None
            character_guid = None
            team_guid = None
            initial_health = None
            nop_stream = player_data["player_nop_action_stream"]
            nop_stream_length = len(nop_stream)

            # Iterate through each action in the player's NOP stream
            for action_index, action in enumerate(nop_stream):
                if action[ACTION_TYPE_IDX] == SPAWN:
                    # If the player spawns and we're not tracking an interval, start a new one
                    if current_interval_start is None:
                        current_interval_start = action[ACTION_TIME_IDX]
                        # Extract additional spawn information
                        if len(action) > CHARACTER_GUID_IDX:
                            character_guid = action[CHARACTER_GUID_IDX]
                        if len(action) > TEAM_GUID_IDX:
                            team_guid = action[TEAM_GUID_IDX]
                        if len(action) > INITIAL_HEALTH_IDX:
                            initial_health = float(action[INITIAL_HEALTH_IDX])
                elif action[ACTION_TYPE_IDX] == DEATH or action_index + 1 == nop_stream_length:
                    # If the player dies or this is the last player action and we're tracking an interval, end it and add to the list
                    if current_interval_start is not None:
                        player_alive_intervals[player_guid].append({
                            'start': current_interval_start,
                            'end': action[ACTION_TIME_IDX],
                            'character_guid': character_guid,
                            'team_guid': team_guid,
                            'initial_health': initial_health
                        })
                        current_interval_start = None
                        character_guid = None
                        team_guid = None
                        initial_health = None

        return player_alive_intervals

    @staticmethod
    def is_player_alive(player_guid: str, timestamp: int, player_alive_intervals: dict) -> bool:
        """Checks if a player is alive at a given timestamp based on their alive intervals.

        Args:
            player_guid (str): The player's GUID.
            timestamp (int): The timestamp to check.
            player_alive_intervals (dict): A dictionary containing alive intervals for each player.

        Returns:
            bool: True if the player is alive at the given timestamp, False otherwise.
        """
        # Get the alive intervals for the specified player
        intervals = player_alive_intervals.get(player_guid, [])

        # Check if the timestamp falls within any of the player's alive intervals
        for interval in intervals:
            if interval['start'] <= timestamp <= interval['end']:
                return True

        # If we've checked all intervals and haven't returned True, the player is not alive
        return False
        
    @staticmethod
    def get_player_location(player_guid: str, timestamp: int, start_action_index: int, 
                           parsed_action_stream_data: dict) -> tuple:
        """Finds the player's closest known location to a specific time by searching backward from the start action index.

        Args:
            player_guid (str): GUID of the player.
            timestamp (int): Timestamp (in milliseconds) for which to find the location.
            start_action_index (int): Index pointing to a possible starting point in the full action stream.
            parsed_action_stream_data (dict): The parsed action stream data containing player information.

        Returns:
            tuple: Tuple containing the player's closest known location (X, Y, Z, pitch, yaw, roll) at the provided time.
        """
        # Ensure start_action_index is an integer
        try:
            if isinstance(start_action_index, str):
                start_action_index = int(start_action_index)
        except (ValueError, TypeError):
            return (0,) * 6
        
        # Get the player's full action stream
        if player_guid not in parsed_action_stream_data["player_dict"]:
            return (0,) * 6
            
        action_stream = parsed_action_stream_data["player_dict"][player_guid]["player_full_action_stream"]

        # Start searching backwards from the given start_action_index
        for i in range(min(start_action_index, len(action_stream)-1), -1, -1):
            action = action_stream[i]
            # We are looking for position events ("P") or spawn events ("S")
            if (action[1] == "P" or action[1] == "S") and action[0] <= timestamp:
                return Utils.get_coordinates_from_action(action)

        # If no position was found, return a default location
        return (0,) * 6
        
    @staticmethod
    def get_coordinates_from_action(action: list) -> tuple:
        """Extracts the coordinates from a player's action event.

        Args:
            action (list): List representing a player's action event.

        Returns:
            tuple: Tuple containing the extracted coordinates (X, Y, Z, pitch, yaw, roll).
        """
        action_type = action[ACTION_TYPE_IDX]
        # Last element in action is the coordinates
        if action_type == POSITION:
            try:
                return tuple(map(float, action[POSITION_ACTION_TO_POSITION_POINTER_JUMPS].split("~", maxsplit=6)[:6]))
            except (IndexError, ValueError):
                return (0, 0, 0, 0, 0, 0)
        elif action_type == SPAWN:
            try:
                return tuple(map(float, action[SPAWN_ACTION_TO_POSITION_POINTER_JUMPS].split("~", maxsplit=6)[:6]))
            except (IndexError, ValueError):
                return (0, 0, 0, 0, 0, 0)
        return (0, 0, 0, 0, 0, 0)


if __name__ == "__main__":
    utils = Utils()