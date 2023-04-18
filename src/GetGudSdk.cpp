#include "..\include\GetGudSdk.h"
#include "utils\Utils.h"
#include "shared_buffer\SharedBuffer.h"

namespace GetGudSdk {

	extern SharedBuffer buffer;

	std::string start_game(int title_id, std::string server_name, std::string game_mode)
	{
		std::string game_guid = generate_guid();

		GameData game_data{ game_guid, title_id, server_name, game_mode }; //fill game data

		buffer.push_game(game_data);

		return game_guid;
	}

	std::string start_match(std::string game_guid, std::string match_mode, std::string map_name)
	{
		std::string match_guid = generate_guid();

		MatchData match_data{ match_guid, game_guid, match_mode, map_name }; //fill match data

		buffer.push_match(match_data);

		return match_guid;
	}

	bool end_game(std::string game_guid)
	{
		buffer.push_end_game(game_guid);

		return true;
	}

	//ReportData struct required
	//bool send_in_match_report(std::string match_guid, ReportData report_data);

	//ChatMessageData struct required
	//bool send_chat(std::string match_guid, ChatMessageData chat_message_data);

	bool send_actions(std::deque<BaseActionData*> actions)
	{
		//push a deque buffer
		buffer.push_actions(actions);
		return true;
	}

	bool send_actions(std::vector<BaseActionData*> actions) 
	{
		//copy actions from a vector buffer to the new deque buffer
		std::deque<BaseActionData*> _actions(actions.begin(), actions.end());

		//push deque buffer
		buffer.push_actions(_actions);
		return true;
	}

	bool send_action(BaseActionData* action)
	{
		//push action
		buffer.push_action(action);
		return true;
	}

	bool send_attack_action(std::string match_guid, std::string player_guid, std::string weapon_guid, long action_time_epoch)
	{
		AttackActionData* attack_data = new AttackActionData(
			player_guid, match_guid, action_time_epoch, weapon_guid);
		
		buffer.push_action(attack_data);

		return true;
	}

	bool send_damage_action(std::string match_guid, std::string player_guid, std::string weapon_guid, std::string victim_player_guid, int damage_done, long action_time_epoch)
	{

		DamageActionData* damage_data = new DamageActionData(
			player_guid, match_guid, action_time_epoch, damage_done, victim_player_guid, weapon_guid);

		buffer.push_action(damage_data);

		return true;
	}

	bool send_heal_action(std::string match_guid, std::string player_guid, int health_gained, long action_time_epoch)
	{

		HealActionData* heal_data = new HealActionData(
			player_guid, match_guid, action_time_epoch, health_gained);

		buffer.push_action(heal_data);

		return true;
	}

	bool send_spawn_action(std::string match_guid, std::string player_guid, std::string character_guid,
		int team_id, int initial_health, PositionF position, RotationF rotation, long action_time_epoch)
	{

		SpawnActionData* spawn_data = new SpawnActionData(
			player_guid, match_guid, action_time_epoch, position, rotation, initial_health, team_id, character_guid);

		buffer.push_action(spawn_data);

		return true;
	}

	bool send_death_action(std::string match_guid, std::string player_guid, long action_time_epoch)
	{

		DeathActionData* spawn_data = new DeathActionData(
			player_guid, match_guid, action_time_epoch);

		buffer.push_action(spawn_data);

		return true;
	}

	bool send_position_action(std::string match_guid, std::string player_guid, PositionF position, RotationF rotation, long action_time_epoch)
	{

		PositionActionData* spawn_data = new PositionActionData(
			player_guid, match_guid, action_time_epoch, position, rotation);

		buffer.push_action(spawn_data);

		return true;
	}

	void dispose()
	{
		buffer.dispose();
	}

	namespace Debug //accessible when the configuration is Debug
	{
		std::deque<BaseActionData*> get_buffer()
		{
			std::deque<BaseActionData*> out_buffer;
			buffer.get_actions(&out_buffer);

			return out_buffer;
		}
	}
}