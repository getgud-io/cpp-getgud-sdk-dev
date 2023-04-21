#include "..\include\GetGudSdk.h"
#include "utils\Utils.h"
#include "actions_buffer\ActionsBuffer.h"
#include "game_container\GameContainer.h"

namespace GetGudSdk {

	extern ActionsBuffer actions_buffer;
	extern GameContainer game_container;

	std::string start_game(int title_id, std::string server_name, std::string game_mode)
	{
		std::string game_guid = generate_guid();

		GameData game_data(game_guid, title_id, server_name, game_mode); //fill game data

		bool result = game_container.add_game(game_data);

		return game_guid;
	}

	std::string start_match(std::string game_guid, std::string match_mode, std::string map_name)
	{
		std::string match_guid = generate_guid();

		MatchData match_data(match_guid, match_mode, map_name); //fill match data

		bool result = game_container.add_match(game_guid, match_data);

		return match_guid;
	}

	bool set_end_game(std::string game_guid)
	{
		bool result = game_container.set_end_game(game_guid);

		return result;
	}

	bool send_in_match_report(std::string match_guid, ReportInfo report_info)
	{
		ReportData report_data(report_info);

		bool result = game_container.add_report(match_guid, report_data);

		return result;
	}

	bool send_chat_message(std::string match_guid, std::string player_guid, std::string message)
	{
		ChatMessageData chat_message_data(player_guid, message);

		bool result = game_container.add_chat_message(match_guid, chat_message_data);

		return result;
	}

	bool send_actions(std::deque<BaseActionData*> actions)
	{
		//push a deque buffer
		bool result = actions_buffer.add_actions(actions);
		return result;
	}

	bool send_actions(std::vector<BaseActionData*> actions) 
	{
		//copy actions from a vector buffer to the new deque buffer
		std::deque<BaseActionData*> _actions(actions.begin(), actions.end());

		//push deque buffer
		bool result = actions_buffer.add_actions(_actions);
		return result;
	}

	bool send_action(BaseActionData* action)
	{
		//push action
		bool result = actions_buffer.add_action(action);
		return result;
	}

	bool send_attack_action(std::string match_guid, std::string player_guid, std::string weapon_guid, long action_time_epoch)
	{
		AttackActionData* attack_data = new AttackActionData(
			player_guid, match_guid, action_time_epoch, weapon_guid);
		
		bool result = actions_buffer.add_action(attack_data);

		return result;
	}

	bool send_damage_action(std::string match_guid, std::string player_guid, std::string weapon_guid, std::string victim_player_guid, int damage_done, long action_time_epoch)
	{

		DamageActionData* damage_data = new DamageActionData(
			player_guid, match_guid, action_time_epoch, damage_done, victim_player_guid, weapon_guid);

		bool result = actions_buffer.add_action(damage_data);

		return result;
	}

	bool send_heal_action(std::string match_guid, std::string player_guid, int health_gained, long action_time_epoch)
	{

		HealActionData* heal_data = new HealActionData(
			player_guid, match_guid, action_time_epoch, health_gained);

		bool result = actions_buffer.add_action(heal_data);

		return result;
	}

	bool send_spawn_action(std::string match_guid, std::string player_guid, std::string character_guid,
		int team_id, int initial_health, PositionF position, RotationF rotation, long action_time_epoch)
	{

		SpawnActionData* spawn_data = new SpawnActionData(
			player_guid, match_guid, action_time_epoch, position, rotation, initial_health, team_id, character_guid);

		bool result = actions_buffer.add_action(spawn_data);

		return result;
	}

	bool send_death_action(std::string match_guid, std::string player_guid, long action_time_epoch)
	{

		DeathActionData* spawn_data = new DeathActionData(
			player_guid, match_guid, action_time_epoch);

		bool result = actions_buffer.add_action(spawn_data);

		return result;
	}

	bool send_position_action(std::string match_guid, std::string player_guid, PositionF position, RotationF rotation, long action_time_epoch)
	{

		PositionActionData* spawn_data = new PositionActionData(
			player_guid, match_guid, action_time_epoch, position, rotation);

		bool result = actions_buffer.add_action(spawn_data);

		return result;
	}

	void dispose()
	{
		actions_buffer.dispose();
		game_container.dispose();
	}

	namespace Debug //accessible when the configuration is Debug
	{
		std::deque<BaseActionData*> get_actions_buffer()
		{
			std::deque<BaseActionData*> out_buffer;
			actions_buffer.get_actions(&out_buffer);

			return out_buffer;
		}

		std::deque<GameData>& get_game_container()
		{
			std::deque<GameData> & data = game_container.get_buffer();

			return game_container.get_buffer();
		}

		std::map<std::string, MatchData>* get_matches_buffer(std::string game_guid)
		{
			std::map<std::string, MatchData>* data = game_container.get_matches(game_guid);

			return data;
		}
	}
}