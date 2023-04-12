#pragma once

#include "DamageActionData.h"
#include "SpawnActionData.h"
#include "AttackActionData.h"
#include "PositionActionData.h"
#include "HealActionData.h"
#include "DeathActionData.h"

/*

- start_game(): string
- end_game(): bool
- start_match(): string
- send_in_match_report(string match_guid, ReportData report_data): bool
- send_chat(string match_guid, ChatMessageData chat_message_data): bool
- send_actions(string match_guid, vector<Action> actions): bool
- send_attack_action(string match_guid, string player_guid, string weapon_guid): bool
- send_damage_action(string match_guid, string player_guid, string weapon_guid, string victim_player_guid, int damage_done): bool
- send_heal_action(string match_guid, string player_guid, int health_gained): bool
- send_spawn_action(string match_guid, string player_guid, string character_guid, int team_id, int initial_health, Position position): bool
- send_death_action(string match_guid, string player_guid): bool
- send_position_action(string match_guid, string player_guid, Position position): bool
- cleanup(): void

*/

namespace GG_SDK {

	std::string start_game(int title_id, std::string server_name, std::string game_mode);

	std::string start_match(std::string game_guid, std::string match_mode, std::string map_name);

	bool end_game(std::string game_guid);

	//ReportData struct required
	//bool send_in_match_report(std::string match_guid, ReportData report_data);

	//ChatMessageData struct required
	//bool send_chat(std::string match_guid, ChatMessageData chat_message_data);

	bool send_actions(std::string match_guid, std::vector<BaseActionData> actions);
	bool send_actions(std::string match_guid, BaseActionData action);

	bool send_attack_action(std::string match_guid, std::string player_guid, std::string weapon_guid);

	bool send_damage_action(std::string match_guid, std::string player_guid, std::string weapon_guid, std::string victim_player_guid, int damage_done);

	bool send_heal_action(std::string match_guid, std::string player_guid, int health_gained);

	bool send_spawn_action(std::string match_guid, std::string player_guid, std::string character_guid,
		int team_id, int initial_health, std::vector<float> position);

	bool send_death_action(std::string match_guid, std::string player_guid);

	bool send_position_action(std::string match_guid, std::string player_guid, std::vector<float> position);

	void cleanup();
}