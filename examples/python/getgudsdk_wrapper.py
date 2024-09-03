import getgudsdk.lib as getgudsdk
from getgudsdk import ffi
import time
from enum import Enum

class GetgudSDK:
    class AffectState(Enum):
        Attach = 0
        Activate = 1
        Deactivate = 2
        Detach = 3
        
    def __init__(self):
        getgudsdk.init()
        
    def __init_conf_path__(self, config_file_path):
        getgudsdk.init_conf_path(config_file_path.encode('utf-8'))
        
    def __init_conf__(self, config_file, passAsContent):
        getgudsdk.init_conf(config_file.encode('utf-8'), passAsContent)

    def start_game(self, title_id, private_key, server_guid, game_mode, server_location):
        game_info = ffi.new("struct StartGameInfo*")
        game_info.titleId = title_id

        private_key_data = ffi.new("char[]", private_key.encode('utf-8'))
        game_info.privateKey = private_key_data
        game_info.privateKeySize = len(private_key)
        
        server_guid_data = ffi.new("char[]", server_guid.encode('utf-8'))
        game_info.serverGuid = server_guid_data
        game_info.serverGuidSize = len(server_guid)

        game_mode_data = ffi.new("char[]", game_mode.encode('utf-8'))
        game_info.gameMode = game_mode_data
        game_info.gameModeSize = len(game_mode)
        
        
        server_location_data = ffi.new("char[]", server_location.encode('utf-8'))
        game_info.serverLocation = server_location_data
        game_info.serverLocationSize = len(server_location)

        game_guid = ffi.new("char[256]") 
        getgudsdk.StartGame(game_info[0], game_guid)

        return ffi.string(game_guid).decode('utf-8')

    def start_match(self, game_guid, match_mode, map_name, custom_field=""):
        match_info = ffi.new("struct StartMatchInfo*")

        game_guid_data = ffi.new("char[]", game_guid.encode('utf-8'))
        match_info.gameGuid = game_guid_data
        match_info.gameGuidSize = len(game_guid)

        match_mode_data = ffi.new("char[]", match_mode.encode('utf-8'))
        match_info.matchMode = match_mode_data
        match_info.matchModeSize = len(match_mode)

        map_name_data = ffi.new("char[]", map_name.encode('utf-8'))
        match_info.mapName = map_name_data
        match_info.mapNameSize = len(map_name)

        custom_field_data = ffi.new("char[]", custom_field.encode('utf-8'))
        match_info.customField = custom_field_data
        match_info.customFieldSize = len(custom_field)

        match_guid = ffi.new("char[256]")
        result_code = getgudsdk.StartMatch(match_info[0], match_guid)

        return ffi.string(match_guid).decode('utf-8')

    def mark_end_game(self, game_guid):
        return getgudsdk.MarkEndGame(game_guid.encode('utf-8'), len(game_guid))

    def send_in_match_report(self, match_guid, reporter_name, reporter_type, reporter_sub_type, 
                         suspected_player_guid, tb_type, 
                         tb_time_epoch, suggested_toxicity_score, reported_time_epoch):
        report_info = ffi.new("struct ReportInfo*")
        
        match_guid_data = ffi.new("char[]", match_guid.encode('utf-8'))
        report_info.matchGuid = match_guid_data
        report_info.matchGuidSize = len(match_guid)

        reporter_name_data = ffi.new("char[]", reporter_name.encode('utf-8'))
        report_info.reporterName = reporter_name_data
        report_info.reporterNameSize = len(reporter_name)

        report_info.reporterType = reporter_type
        report_info.reporterSubType = reporter_sub_type
        
        suspected_player_data = ffi.new("char[]", suspected_player_guid.encode('utf-8'))
        report_info.suspectedPlayerGuid = suspected_player_data
        report_info.suspectedPlayerGuidSize = len(suspected_player_guid)

        report_info.tbType = tb_type
        report_info.tbTimeEpoch = tb_time_epoch

        report_info.suggestedToxicityScore = suggested_toxicity_score
        report_info.reportedTimeEpoch = reported_time_epoch
        
        result_code = getgudsdk.SendInMatchReport(report_info[0])

        return result_code

    def send_chat_message(self, match_guid, message_time_epoch, player_guid, message):
        message_info = ffi.new("struct ChatMessageInfo*")

        message_info.messageTimeEpoch = message_time_epoch

        match_guid_data = ffi.new("char[]", match_guid.encode('utf-8'))
        message_info.matchGuid = match_guid_data
        message_info.matchGuidSize = len(match_guid)

        player_guid_data = ffi.new("char[]", player_guid.encode('utf-8'))
        message_info.playerGuid = player_guid_data
        message_info.playerGuidSize = len(player_guid)

        message_info_data = ffi.new("char[]", message.encode('utf-8'))
        message_info.message = message_info_data
        message_info.messageSize = len(message)

        result_code = getgudsdk.SendChatMessage(message_info[0])

        return result_code
    
    def send_attack_action(self, match_guid, action_time_epoch, player_guid, weapon_guid):
        base_data = ffi.new("struct BaseActionData*")
        base_data.actionTimeEpoch = action_time_epoch
        match_guid_data = ffi.new("char[]", match_guid.encode('utf-8'))
        base_data.matchGuid = match_guid_data
        base_data.matchGuidSize = len(match_guid)
        player_guid_data = ffi.new("char[]", player_guid.encode('utf-8'))
        base_data.playerGuid = player_guid_data
        base_data.playerGuidSize = len(player_guid)
        
        weapon_guid_data = ffi.new("char[]", weapon_guid.encode('utf-8'))
        weapon_guid_size = len(weapon_guid)

        return getgudsdk.SendAttackAction(base_data[0], weapon_guid_data, weapon_guid_size)

    def send_damage_action(self, match_guid, action_time_epoch, player_guid,
                        victim_player_guid, damage_done, weapon_guid):
        base_data = ffi.new("struct BaseActionData*")
        base_data.actionTimeEpoch = action_time_epoch
        match_guid_data = ffi.new("char[]", match_guid.encode('utf-8'))
        base_data.matchGuid = match_guid_data
        base_data.matchGuidSize = len(match_guid)
        player_guid_data = ffi.new("char[]", player_guid.encode('utf-8'))
        base_data.playerGuid = player_guid_data
        base_data.playerGuidSize = len(player_guid)
        
        victim_player_guid_data = ffi.new("char[]", victim_player_guid.encode('utf-8'))
        victim_player_guid_size = len(victim_player_guid)
        weapon_guid_data = ffi.new("char[]", weapon_guid.encode('utf-8'))
        weapon_guid_size = len(weapon_guid)

        return getgudsdk.SendDamageAction(base_data[0], victim_player_guid_data,
                                        victim_player_guid_size, damage_done,
                                        weapon_guid_data, weapon_guid_size)

    def send_heal_action(self, match_guid, action_time_epoch, player_guid, health_gained):
        base_data = ffi.new("struct BaseActionData*")
        base_data.actionTimeEpoch = action_time_epoch
        match_guid_data = ffi.new("char[]", match_guid.encode('utf-8'))
        base_data.matchGuid = match_guid_data
        base_data.matchGuidSize = len(match_guid)
        player_guid_data = ffi.new("char[]", player_guid.encode('utf-8'))
        base_data.playerGuid = player_guid_data
        base_data.playerGuidSize = len(player_guid)

        return getgudsdk.SendHealAction(base_data[0], health_gained)

    def send_spawn_action(self, match_guid, action_time_epoch, player_guid,
                        character_guid, team_guid, initial_health, position, rotation):
        base_data = ffi.new("struct BaseActionData*")
        base_data.actionTimeEpoch = action_time_epoch
        match_guid_data = ffi.new("char[]", match_guid.encode('utf-8'))
        base_data.matchGuid = match_guid_data
        base_data.matchGuidSize = len(match_guid)
        player_guid_data = ffi.new("char[]", player_guid.encode('utf-8'))
        base_data.playerGuid = player_guid_data
        base_data.playerGuidSize = len(player_guid)

        character_guid_data = ffi.new("char[]", character_guid.encode('utf-8'))
        character_guid_size = len(character_guid)
        
        team_guid_data = ffi.new("char[]", team_guid.encode('utf-8'))
        team_guid_size = len(team_guid)

        position_struct = ffi.new("struct PositionF*", {"X": position[0], "Y": position[1], "Z": position[2]})
        rotation_struct = ffi.new("struct RotationF*", {"Yaw": rotation[0], "Pitch": rotation[1], "Roll": rotation[2]})

        return getgudsdk.SendSpawnAction(base_data[0], character_guid_data, character_guid_size,
                                        team_guid_data, team_guid_size, initial_health, position_struct[0], rotation_struct[0])


    
    def send_death_action(self, match_guid, action_time_epoch, player_guid, attacker_guid):
        base_data = ffi.new("struct BaseActionData*")
        base_data.actionTimeEpoch = action_time_epoch
        match_guid_data = ffi.new("char[]", match_guid.encode('utf-8'))
        base_data.matchGuid = match_guid_data
        base_data.matchGuidSize = len(match_guid)
        player_guid_data = ffi.new("char[]", player_guid.encode('utf-8'))
        base_data.playerGuid = player_guid_data
        base_data.playerGuidSize = len(player_guid)
        
        attacker_guid_data = ffi.new("char[]", attacker_guid.encode('utf-8'))
        attacker_guid_size = len(attacker_guid)

        return getgudsdk.SendDeathAction(base_data[0], attacker_guid_data, attacker_guid_size)

    def send_position_action(self, match_guid, action_time_epoch, player_guid, position, rotation):
        base_data = ffi.new("struct BaseActionData*")
        base_data.actionTimeEpoch = action_time_epoch
        match_guid_data = ffi.new("char[]", match_guid.encode('utf-8'))
        base_data.matchGuid = match_guid_data
        base_data.matchGuidSize = len(match_guid)
        player_guid_data = ffi.new("char[]", player_guid.encode('utf-8'))
        base_data.playerGuid = player_guid_data
        base_data.playerGuidSize = len(player_guid)

        position_struct = ffi.new("struct PositionF*", {"X": position[0], "Y": position[1], "Z": position[2]})
        rotation_struct = ffi.new("struct RotationF*", {"Yaw": rotation[0], "Pitch": rotation[1], "Roll": rotation[2]})

        return getgudsdk.SendPositionAction(base_data[0], position_struct[0], rotation_struct[0])
    
    def send_report(self, title_id, private_key, match_guid, reporter_name, reporter_type, reporter_sub_type, 
                         suspected_player_guid, tb_type, 
                         tb_time_epoch, suggested_toxicity_score, reported_time_epoch):
        
        private_key_data = ffi.new("char[]", private_key.encode('utf-8'))
        privateKeySize = len(private_key)
        
        report_info = ffi.new("struct ReportInfo*")
        
        match_guid_data = ffi.new("char[]", match_guid.encode('utf-8'))
        report_info.matchGuid = match_guid_data
        report_info.matchGuidSize = len(match_guid)

        reporter_name_data = ffi.new("char[]", reporter_name.encode('utf-8'))
        report_info.reporterName = reporter_name_data
        report_info.reporterNameSize = len(reporter_name)

        report_info.reporterType = reporter_type
        report_info.reporterSubType = reporter_sub_type
        
        suspected_player_data = ffi.new("char[]", suspected_player_guid.encode('utf-8'))
        report_info.suspectedPlayerGuid = suspected_player_data
        report_info.suspectedPlayerGuidSize = len(suspected_player_guid)

        report_info.tbType = tb_type
        report_info.tbTimeEpoch = tb_time_epoch

        report_info.suggestedToxicityScore = suggested_toxicity_score
        report_info.reportedTimeEpoch = reported_time_epoch
        
        result_code = getgudsdk.SendReport(title_id, private_key_data, privateKeySize, report_info[0])

        return result_code
    
    def send_affect_action(self, match_guid, action_time_epoch, player_guid, affect_guid, affect_state):
        base_data = ffi.new("struct BaseActionData*")
        base_data.actionTimeEpoch = action_time_epoch

        match_guid_data = ffi.new("char[]", match_guid.encode('utf-8'))
        base_data.matchGuid = match_guid_data
        base_data.matchGuidSize = len(match_guid)

        player_guid_data = ffi.new("char[]", player_guid.encode('utf-8'))
        base_data.playerGuid = player_guid_data
        base_data.playerGuidSize = len(player_guid)

        affect_guid_data = ffi.new("char[]", affect_guid.encode('utf-8'))
        affect_guid_size = len(affect_guid)
        
        # Check if affect_state is an instance of the AffectState enum or a string
        if isinstance(affect_state, str):
            affect_state_value = self.AffectState[affect_state].value
        else:
            affect_state_value = affect_state.value

        result_code = getgudsdk.SendAffectAction(base_data[0], affect_guid_data, affect_guid_size, affect_state_value)

        return result_code
     
    def update_player(
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
        private_key_data = ffi.new("char[]", private_key.encode('utf-8'))
        privateKeySize = len(private_key)
        
        player_info = ffi.new("struct PlayerInfo*")
        
        player_guid_data = ffi.new("char[]", player_guid.encode('utf-8'))
        player_info.playerGuid = player_guid_data
        player_info.playerGuidSize = len(player_guid)

        player_nickname_data = ffi.new("char[]", player_nickname.encode('utf-8'))
        player_info.playerNickname = player_nickname_data
        player_info.playerNicknameSize = len(player_nickname)

        player_email_data = ffi.new("char[]", player_email.encode('utf-8'))
        player_info.playerEmail = player_email_data
        player_info.playerEmailSize = len(player_email)

        player_info.playerRank = player_rank
        player_info.playerJoinDateEpoch = player_join_date_epoch
        
        player_suspect_score_data = ffi.new("char[]", player_suspect_score.encode('utf-8'))
        player_info.playerSuspectScore = player_suspect_score_data
        player_info.playerSuspectScoreSize = len(player_suspect_score)

        player_reputation_data = ffi.new("char[]", player_reputation.encode('utf-8'))
        player_info.playerReputation = player_reputation_data
        player_info.playerReputationSize = len(player_reputation)

        player_status_data = ffi.new("char[]", player_status.encode('utf-8'))
        player_info.playerStatus = player_status_data
        player_info.playerStatusSize = len(player_status)

        player_campaign_data = ffi.new("char[]", player_campaign.encode('utf-8'))
        player_info.PlayerCampaign = player_campaign_data
        player_info.PlayerCampaignSize = len(player_campaign)

        player_notes_data = ffi.new("char[]", player_notes.encode('utf-8'))
        player_info.playerNotes = player_notes_data
        player_info.playerNotesSize = len(player_notes)

        player_device_data = ffi.new("char[]", player_device.encode('utf-8'))
        player_info.playerDevice = player_device_data
        player_info.playerDeviceSize = len(player_device)

        player_os_data = ffi.new("char[]", player_os.encode('utf-8'))
        player_info.playerOS = player_os_data
        player_info.playerOSSize = len(player_os)

        player_info.playerAge = player_age

        player_gender_data = ffi.new("char[]", player_gender.encode('utf-8'))
        player_info.playerGender = player_gender_data
        player_info.playerGenderSize = len(player_gender)

        player_location_data = ffi.new("char[]", player_location.encode('utf-8'))
        player_info.playerLocation = player_location_data
        player_info.playerLocationSize = len(player_location)

        # Handle transactions if provided
        if transactions:
            # Create an array of PlayerTransactions structures
            transaction_array = ffi.new("struct PlayerTransactions[]", len(transactions))
            for i, transaction in enumerate(transactions):
                transaction_guid = ffi.new("char[]", transaction['TransactionGuid'].encode('utf-8'))
                transaction_name = ffi.new("char[]", transaction['TransactionName'].encode('utf-8'))
                
                transaction_array[i].TransactionGuid = transaction_guid
                transaction_array[i].TransactionGuidSize = len(transaction['TransactionGuid'])
                transaction_array[i].TransactionName = transaction_name
                transaction_array[i].TransactionNameSize = len(transaction['TransactionName'])
                transaction_array[i].TransactionDateEpoch = transaction['TransactionDateEpoch']
                transaction_array[i].TransactionValueUSD = transaction['TransactionValueUSD']

            player_info.transactions = transaction_array
            player_info.transactionsSize = len(transactions)
        else:
            player_info.transactions = ffi.NULL
            player_info.transactionsSize = 0
        
        result_code = getgudsdk.UpdatePlayer(title_id, private_key_data, privateKeySize, player_info[0])

        return result_code
    
    def dispose(self):
        getgudsdk.dispose()