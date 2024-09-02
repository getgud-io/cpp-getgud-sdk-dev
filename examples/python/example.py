from getgudsdk_wrapper import GetGudSdk
import time
import random

def random_string(length):
    result = ''.join(chr(random.randint(97, 122)) for _ in range(length))
    return result

if __name__ == "__main__":
    sdk = GetgudSDK()
    game_guid = sdk.start_game(1, "pk", "aws", "deathmatch")
    match_guid = sdk.start_match(game_guid, "test_map", "deathmatch", "custom_field_value")
    
    action_time_epoch = 1000*int(time.time())
    weapon_guid = random_string(3)
    player_guid = random_string(10)
    sdk.send_attack_action(match_guid, action_time_epoch, player_guid, weapon_guid)

    
    message_time_epoch = 1000*int(time.time())
    player_guid = random_string(10)
    message = "Hello World!"
    sdk.send_chat_message(match_guid, message_time_epoch, player_guid, message)

    reporter_name = random_string(5)
    reporter_type = 1
    reporter_sub_type = 1
    suspected_player_guid = random_string(10)
    tb_type = 1
    tb_sub_type = 1
    tb_time_epoch = 1000*int(time.time())
    suggested_toxicity_score = random.randint(0, 100)
    reported_time_epoch = 1000*int(time.time())
    sdk.send_in_match_report(match_guid, reporter_name, reporter_type, reporter_sub_type, 
                         suspected_player_guid, tb_type, tb_sub_type, 
                         tb_time_epoch, suggested_toxicity_score, reported_time_epoch)
    
    victim_player_guid = random_string(10)
    damage_done = random.randint(10, 50)
    sdk.send_damage_action(match_guid, action_time_epoch, player_guid, victim_player_guid, damage_done, weapon_guid)

    health_gained = random.randint(10, 50)
    sdk.send_heal_action(match_guid, action_time_epoch, player_guid, health_gained)

    character_guid = random_string(3)
    team_id = 1
    initial_health = 100
    position = (random.uniform(-1, 1), random.uniform(-1, 1), random.uniform(-1, 1))
    rotation = (random.uniform(0, 360), random.uniform(0, 360), random.uniform(0, 360))
    sdk.send_spawn_action(match_guid, action_time_epoch, player_guid, character_guid, team_id, initial_health, position, rotation)

    sdk.send_death_action(match_guid, action_time_epoch, player_guid)

    sdk.send_position_action(match_guid, action_time_epoch, player_guid, position, rotation)

    time.sleep(2)
    sdk.mark_end_game(game_guid)
    time.sleep(20)
    sdk.dispose()