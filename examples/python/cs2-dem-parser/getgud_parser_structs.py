import uuid
from enum import Enum

class Position:
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z
    def to_string(self):
        return f'{self.x};{self.y};{self.z}'

class Rotation:
    def __init__(self, yaw, pitch, roll):
        self.yaw = yaw
        self.pitch = pitch
        self.roll = roll
    def to_string(self):
        return f'{self.yaw};{self.pitch};{self.roll}'

class ReportInfo:
    def __init__(self, 
                 match_guid,
                 reporter_name, 
                 reporter_type, 
                 reporter_sub_type, 
                 suspected_player_guid, 
                 tb_type,
                 tb_time_epoch,
                 suggested_toxicity_score,
                 reported_time_epoch
                ):
        self.match_guid = match_guid
        self.reporter_name = reporter_name
        self.reporter_type = reporter_type
        self.reporter_sub_type = reporter_sub_type
        self.suspected_player_guid = suspected_player_guid
        self.tb_type = tb_type
        self.tb_time_epoch = tb_time_epoch
        self.suggested_toxicity_score = suggested_toxicity_score
        self.reported_time_epoch = reported_time_epoch
    
    def to_string(self):
        return f'{self.match_guid};{self.reporter_name};{self.reporter_type};{self.reporter_sub_type};{self.suspected_player_guid};{self.tb_type};{self.tb_time_epoch};{self.suggested_toxicity_score};{self.reported_time_epoch}'

class PlayerInfo:
    def __init__(self, 
                 player_guid, 
                 player_nickname, 
                 player_email, 
                 player_rank, 
                 player_join_date_epoch
                ):
        self.player_guid = player_guid
        self.player_nickname = player_nickname
        self.player_email = player_email
        self.player_rank = player_rank
        self.player_join_date_epoch = player_join_date_epoch
    
    def to_string(self):
        return f'{self.player_guid};{self.player_nickname};{self.player_email};{self.player_rank};{self.player_join_date_epoch}'

class StartGame:
    def __init__(self, title_id, private_key, server_guid, game_mode, server_location):
        self.title_id = title_id
        self.private_key = private_key
        self.server_guid = server_guid
        self.game_mode = game_mode
        self.server_location = server_location
    
    def to_string(self):
        return f'StartGame:{self.title_id};{self.private_key};{self.server_guid};{self.game_mode};{self.server_location}\n'
    
    def call(self, sdk):
        return sdk.start_game(
            self.title_id, 
            self.private_key, 
            self.server_guid,
            self.game_mode,
            self.server_location
        )

class StartMatch:
    def __init__(self, game_guid, match_mode, map_name, custom_field=""):
        self.game_guid = game_guid
        self.match_mode = match_mode
        self.map_name = map_name
        self.custom_field = custom_field
        
    def to_string(self):
        return f'StartMatch:{self.game_guid};{self.match_mode};{self.map_name};{self.custom_field}\n'

    def call(self, sdk):
        return sdk.start_match(
            self.game_guid, 
            self.match_mode, 
            self.map_name,
            self.custom_field
        )

class MarkEndGame:
    def __init__(self, game_guid):
        self.game_guid = game_guid
    
    def to_string(self):
        return f'MarkEndGame:{self.game_guid}\n'

    def call(self, sdk):
        return sdk.mark_end_game(self.game_guid)

class SetMatchWinTeam:
    def __init__(self, match_guid, team_guid):
        self.match_guid = match_guid
        self.team_guid = team_guid

    def to_string(self):
        return f'SetMatchWinTeam:{self.match_guid};{self.team_guid}\n'

    def call(self, sdk):
        return sdk.set_match_win_team(self.match_guid, self.team_guid)

class SendChatMessage:
    def __init__(self, match_guid, message_time_epoch, player_guid, message):
        self.match_guid = match_guid
        self.message_time_epoch = message_time_epoch
        self.player_guid = player_guid
        self.message = message
    
    def to_string(self):
        return f'SendChatMessage:{self.match_guid};{self.message_time_epoch};{self.player_guid};{self.message}\n'
    
    def call(self, sdk):
        return sdk.send_chat_message(
            self.match_guid,
            self.message_time_epoch,
            self.player_guid,
            self.message
        )

class SendInMatchReport:
    def __init__(self, report_info):
        self.report_info = report_info
    
    def to_string(self):
        return f'SendInMatchReport:{self.report_info.to_string()}\n'

    def call(self, sdk):
        return sdk.send_in_match_report(
            self.report_info.match_guid, 
            self.report_info.reporter_name,
            self.report_info.reporter_type,
            self.report_info.reporter_sub_type,
            self.report_info.suspected_player_guid,
            self.report_info.tb_type,
            self.report_info.tb_time_epoch,
            self.report_info.suggested_toxicity_score,
            self.report_info.reported_time_epoch
        )

class SendReports:
    def __init__(self, title_id, private_key, report_info):
        self.title_id = title_id
        self.private_key = private_key
        self.report_info = report_info
    
    def to_string(self):
        return f'SendReports:{self.title_id};{self.private_key};{self.report_info.to_string()}\n'
    
    def call(self, sdk):
        return sdk.send_report(
            self.title_id,
            self.private_key,
            self.report_info.match_guid,
            self.report_info.reporter_name,
            self.report_info.reporter_type,
            self.report_info.reporter_sub_type,
            self.report_info.suspected_player_guid,
            self.report_info.tb_type,
            self.report_info.tb_time_epoch,
            self.report_info.suggested_toxicity_score,
            self.report_info.reported_time_epoch
        )

class UpdatePlayer:
    def __init__(
        self, 
        title_id, 
        private_key, 
        player_guid, 
        player_nickname, 
        player_email,
        player_rank, 
        player_join_date_epoch,
        player_suspect_score, 
        player_reputation,           
        player_status, 
        player_campaign, 
        player_notes, 
        player_device, 
        player_os, 
        player_age, 
        player_gender, 
        player_location, 
        transactions
    ):
        self.title_id = title_id
        self.private_key = private_key
        self.player_guid = player_guid
        self.player_nickname = player_nickname
        self.player_email = player_email
        self.player_rank = player_rank
        self.player_join_date_epoch = player_join_date_epoch
        self.player_suspect_score = player_suspect_score
        self.player_reputation = player_reputation
        self.player_status = player_status
        self.player_campaign = player_campaign
        self.player_notes = player_notes
        self.player_device = player_device
        self.player_os = player_os
        self.player_age = player_age
        self.player_gender = player_gender
        self.player_location = player_location
        self.transactions = transactions

    def to_string(self):
        transaction_strings = [f"{transaction['TransactionGuid']};{transaction['TransactionName']};{transaction['TransactionDateEpoch']};{transaction['TransactionValueUSD']}" for transaction in self.transactions]
        transactions_str = '|'.join(transaction_strings)
        return f'UpdatePlayer:{self.title_id};{self.private_key};{self.player_guid};{self.player_nickname};{self.player_email};{self.player_rank};{self.player_join_date_epoch};{self.player_suspect_score};{self.player_reputation};{self.player_status};{self.player_campaign};{self.player_notes};{self.player_device};{self.player_os};{self.player_age};{self.player_gender};{self.player_location};{transactions_str}\n'

    def call(self, sdk):
        return sdk.update_player(
            self.title_id, 
            self.private_key, 
            self.player_guid, 
            self.player_nickname, 
            self.player_email,
            self.player_rank, 
            self.player_join_date_epoch,
            self.player_suspect_score, 
            self.player_reputation,           
            self.player_status, 
            self.player_campaign, 
            self.player_notes, 
            self.player_device, 
            self.player_os, 
            self.player_age,
            self.player_gender, 
            self.player_location, 
            self.transactions
        )

class AttackActionData:
    def __init__(self, match_guid, action_time_epoch, player_guid, weapon_guid):
        self.match_guid = match_guid
        self.player_guid = str(player_guid)
        self.weapon_guid = str(weapon_guid)
        self.action_time_epoch = action_time_epoch

    def to_string(self):
        return f'A:{self.match_guid};{self.action_time_epoch};{self.player_guid};{self.weapon_guid}\n'

    def call(self, sdk):
        return sdk.send_attack_action(
            self.match_guid,
            self.action_time_epoch,
            self.player_guid,
            self.weapon_guid
        )

class DamageActionData:
    def __init__(self, match_guid, action_time_epoch, attacker_player_guid, victim_player_guid, damage_done, weapon_guid):
        self.match_guid = match_guid
        self.attacker_player_guid = str(attacker_player_guid)
        self.weapon_guid = str(weapon_guid)
        self.victim_player_guid = str(victim_player_guid)
        self.damage_done = damage_done
        self.action_time_epoch = action_time_epoch

    def to_string(self):
        return f'D:{self.match_guid};{self.action_time_epoch};{self.attacker_player_guid};{self.victim_player_guid};{self.damage_done};{self.weapon_guid}\n'

    def call(self, sdk):
        return sdk.send_damage_action(
            self.match_guid,
            self.action_time_epoch,
            self.attacker_player_guid,
            self.victim_player_guid,
            self.damage_done,
            self.weapon_guid
        )

class HealActionData:
    def __init__(self, match_guid, action_time_epoch, player_guid, health_gained):
        self.match_guid = match_guid
        self.player_guid = str(player_guid)
        self.health_gained = health_gained
        self.action_time_epoch = action_time_epoch

    def to_string(self):
        return f'H:{self.match_guid};{self.action_time_epoch};{self.player_guid};{self.health_gained}\n'

    def call(self, sdk):
        return sdk.send_heal_action(
            self.match_guid,
            self.action_time_epoch,
            self.player_guid,
            self.health_gained
        )

class SpawnActionData:
    def __init__(self, match_guid, action_time_epoch, player_guid, character_guid, team_guid, initial_health, position, rotation):
        self.match_guid = match_guid
        self.player_guid = str(player_guid)
        self.character_guid = str(character_guid)
        self.team_guid = str(team_guid)
        self.initial_health = initial_health
        self.position = position
        self.rotation = rotation
        self.action_time_epoch = action_time_epoch

    def to_string(self):
        return f'S:{self.match_guid};{self.action_time_epoch};{self.player_guid};{self.character_guid};{self.team_guid};{self.initial_health};{self.position.to_string()};{self.rotation.to_string()}\n'

    def call(self, sdk):
        return sdk.send_spawn_action(
            self.match_guid,
            self.action_time_epoch,
            self.player_guid,
            self.character_guid,
            self.team_guid,
            self.initial_health,
            (self.position.x, self.position.y, self.position.z),
            (self.rotation.yaw, self.rotation.pitch, self.rotation.roll)
        )

class DeathActionData:
    def __init__(self, match_guid, action_time_epoch, player_guid, attacker_guid):
        self.match_guid = match_guid
        self.action_time_epoch = action_time_epoch
        self.player_guid = player_guid
        self.attacker_guid = attacker_guid

    def to_string(self):
        return f'DeathActionData:{self.match_guid};{self.action_time_epoch};{self.player_guid};{self.attacker_guid}\n'

    def call(self, sdk):
        return sdk.send_death_action(
            self.match_guid,
            self.action_time_epoch,
            self.player_guid,
            self.attacker_guid
        )

class PositionActionData:
    def __init__(self, match_guid, action_time_epoch, player_guid, position, rotation):
        self.match_guid = match_guid
        self.player_guid = str(player_guid)
        self.position = position
        self.rotation = rotation
        self.action_time_epoch = action_time_epoch

    def to_string(self):
        return f'P:{self.match_guid};{self.action_time_epoch};{self.player_guid};{self.position.to_string()};{self.rotation.to_string()}\n'

    def call(self, sdk):
        return sdk.send_position_action(
            self.match_guid,
            self.action_time_epoch,
            self.player_guid,
            (self.position.x, self.position.y, self.position.z),
            (self.rotation.yaw, self.rotation.pitch, self.rotation.roll)
        )
        
class AffectActionData:
    def __init__(self, match_guid, action_time_epoch, player_guid, affect_guid, affect_state):
        self.match_guid = match_guid
        self.action_time_epoch = action_time_epoch
        self.player_guid = str(player_guid)
        self.affect_guid = str(affect_guid)
        self.affect_state = affect_state  # This should be an instance of sdk.AffectState or a string

    def to_string(self):
        return f'AF:{self.match_guid};{self.action_time_epoch};{self.player_guid};{self.affect_guid};{self.affect_state}\n'

    def call(self, sdk):
        affect_state_value = self.affect_state
        if isinstance(self.affect_state, str):
            affect_state_value = sdk.AffectState[self.affect_state].value

        return sdk.send_affect_action(
            self.match_guid,
            self.action_time_epoch,
            self.player_guid,
            self.affect_guid,
            affect_state_value
        )

# Enum for AffectState
class AffectState(Enum):
    Attach = 0
    Activate = 1
    Deactivate = 2
    Detach = 3