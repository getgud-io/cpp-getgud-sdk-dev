from getgudsdk_wrapper import GetgudSDK
import time
import random

def random_string(length):
    return ''.join(chr(random.randint(97, 122)) for _ in range(length))

def test_1(title_id = 1, private_key="pk"):
    sdk = GetgudSDK()
    game_guid = sdk.start_game(title_id, private_key, "aws", "deathmatch", "UK")
    match_guid = sdk.start_match(game_guid, "deathmatch", "test_map", "custom_field_value")
    
    action_time_epoch = int(time.time() * 1000)
    weapon_guid = random_string(3)
    player_guid = random_string(10)
    sdk.send_attack_action(match_guid, action_time_epoch, player_guid, weapon_guid)
    
    message_time_epoch = int(time.time() * 1000)
    player_guid = random_string(10)
    message = "Hello World!"
    sdk.send_chat_message(match_guid, message_time_epoch, player_guid, message)
    
    reporter_name = random_string(5)
    reporter_type = 1
    reporter_sub_type = 1
    suspected_player_guid = random_string(10)
    tb_type = 1
    tb_time_epoch = int(time.time() * 1000)
    suggested_toxicity_score = random.randint(0, 100)
    reported_time_epoch = int(time.time() * 1000)
    sdk.send_in_match_report(match_guid, reporter_name, reporter_type, reporter_sub_type,
                             suspected_player_guid, tb_type,
                             tb_time_epoch, suggested_toxicity_score, reported_time_epoch)
    
    victim_player_guid = random_string(10)
    damage_done = random.randint(10, 50)
    sdk.send_damage_action(match_guid, action_time_epoch, player_guid, victim_player_guid, damage_done, weapon_guid)
    
    health_gained = random.randint(10, 50)
    sdk.send_heal_action(match_guid, action_time_epoch, player_guid, health_gained)
    
    character_guid = random_string(3)
    team_guid = "Team-1"
    initial_health = 100
    position = (random.uniform(-1, 1), random.uniform(-1, 1), random.uniform(-1, 1))
    rotation = (random.uniform(0, 360), random.uniform(0, 360), random.uniform(0, 360))
    sdk.send_spawn_action(match_guid, action_time_epoch, player_guid, character_guid, team_guid, initial_health, position, rotation)
    
    attacker_guid = random_string(10)
    sdk.send_death_action(match_guid, action_time_epoch, player_guid, attacker_guid)
    
    sdk.send_position_action(match_guid, action_time_epoch, player_guid, position, rotation)
    
    # Add an example of send_affect_action
    affect_guid = random_string(5)
    affect_state = sdk.AffectState.Activate
    sdk.send_affect_action(match_guid, action_time_epoch, player_guid, affect_guid, affect_state)
    
    # Add an example of update_player
    title_id = 1
    private_key = "your_private_key"
    player_nickname = "TestPlayer"
    player_email = "test@example.com"
    player_rank = 10
    player_join_date_epoch = int(time.time() * 1000)
    player_suspect_score = "0.5"
    player_reputation = "good"
    player_status = "active"
    player_campaign = "main"
    player_notes = "Test player"
    player_device = "PC"
    player_os = "Windows"
    player_age = 25
    player_gender = "Male"
    player_location = "US"
    transactions = [
        {
            "TransactionGuid": random_string(10),
            "TransactionName": "Purchase",
            "TransactionDateEpoch": int(time.time() * 1000),
            "TransactionValueUSD": 9.99
        }
    ]
    sdk.update_player(title_id, private_key, player_guid, player_nickname, player_email,
                      player_rank, player_join_date_epoch, player_suspect_score, player_reputation,
                      player_status, player_campaign, player_notes, player_device, player_os,
                      player_age, player_gender, player_location, transactions)
    
    # Add an example of set_match_win_team
    winning_team_guid = team_guid  # Using the previously generated team_guid as the winning team
    result = sdk.set_match_win_team(match_guid, winning_team_guid)
    
    time.sleep(2)
    sdk.mark_end_game(game_guid)
    time.sleep(5)
    sdk.dispose()


def test_2(title_id=1, private_key="pk"):
    sdk = GetgudSDK()

    # Start the first game and match
    game_guid = sdk.start_game(title_id, private_key, "aws", "deathmatch", "UK")
    match_guid_1 = sdk.start_match(game_guid, "deathmatch", "test_map_1", "custom_field_value_1")

    player_guid_1 = 'pl1'
    player_guid_2 = 'pl2'
    team_guid_1 = "Team-1"
    team_guid_2 = "Team-2"

    character_guid = random_string(3)
    initial_health = 100
    action_time_epoch = int(time.time() * 1000)
    position = (random.uniform(-1, 1), random.uniform(-1, 1), random.uniform(-1, 1))
    rotation = (random.uniform(0, 360), random.uniform(0, 360), random.uniform(0, 360))

    # Spawn both players in the first match
    sdk.send_spawn_action(match_guid_1, action_time_epoch, player_guid_1, character_guid, team_guid_1, initial_health, position, rotation)
    sdk.send_spawn_action(match_guid_1, action_time_epoch, player_guid_2, character_guid, team_guid_2, initial_health, position, rotation)

    # Generate 20 random weapon GUIDs
    weapon_guids = [random_string(3) for _ in range(20)]

    # Perform 100 attack and damage actions in the first match
    for _ in range(10000):
        action_time_epoch = int(time.time() * 1000)
        weapon_guid = random.choice(weapon_guids)
        damage_done = random.randint(10, 50)

        # Attack actions
        sdk.send_attack_action(match_guid_1, action_time_epoch, player_guid_1, weapon_guid)
        sdk.send_attack_action(match_guid_1, action_time_epoch, player_guid_2, weapon_guid)

        # Damage actions
        sdk.send_damage_action(match_guid_1, action_time_epoch, player_guid_1, player_guid_2, damage_done, weapon_guid)
        sdk.send_damage_action(match_guid_1, action_time_epoch, player_guid_2, player_guid_1, damage_done, weapon_guid)

    action_time_epoch = int(time.time() * 1000)
    sdk.send_death_action(match_guid_1, action_time_epoch, player_guid_1, player_guid_2)

    # End the first match
    time.sleep(2)

    # # Start a second match in the same game
    # match_guid_2 = sdk.start_match(game_guid, "deathmatch", "test_map_2", "custom_field_value_2")

    # # Spawn both players in the second match
    # sdk.send_spawn_action(match_guid_2, action_time_epoch, player_guid_1, character_guid, team_guid_1, initial_health, position, rotation)
    # sdk.send_spawn_action(match_guid_2, action_time_epoch, player_guid_2, character_guid, team_guid_2, initial_health, position, rotation)

    # # Perform 100 attack and damage actions in the second match
    # for _ in range(100):
    #     action_time_epoch = int(time.time() * 1000)
    #     weapon_guid = random.choice(weapon_guids)
    #     damage_done = random.randint(10, 50)

    #     # Attack actions
    #     sdk.send_attack_action(match_guid_2, action_time_epoch, player_guid_1, weapon_guid)
    #     sdk.send_attack_action(match_guid_2, action_time_epoch, player_guid_2, weapon_guid)

    #     # Damage actions
    #     sdk.send_damage_action(match_guid_2, action_time_epoch, player_guid_1, player_guid_2, damage_done, weapon_guid)
    #     sdk.send_damage_action(match_guid_2, action_time_epoch, player_guid_2, player_guid_1, damage_done, weapon_guid)

    # action_time_epoch = int(time.time() * 1000)
    # sdk.send_death_action(match_guid_2, action_time_epoch, player_guid_2, player_guid_1)
    
    # End the second match
    time.sleep(2)
    sdk.mark_end_game(game_guid)

    # Dispose the SDK instance
    time.sleep(5)
    sdk.dispose()
    
if __name__ == "__main__":
    title_id=0
    private_key="test"
    
    # test_1(title_id, private_key)
    test_2(title_id, private_key)