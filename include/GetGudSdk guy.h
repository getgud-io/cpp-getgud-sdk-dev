#pragma once

#include "actions/AttackActionData.h"
#include "actions/DamageActionData.h"
#include "actions/DeathActionData.h"
#include "actions/HealActionData.h"
#include "actions/Helpers.h"
#include "actions/PositionActionData.h"
#include "actions/SpawnActionData.h"

#ifdef _DEBUG
#include "../src/game_container/GameData.h"
#endif

#include <deque>


namespace GetGudSdk {

    void init();
    std::string start_game(int title_id, std::string privateKey, std::string server_name, std::string game_mode);
    std::string start_game(std::string server_name, std::string game_mode); // will take needed params from env variables 
    std::string start_match(std::string game_guid,std::string match_mode, std::string map_name);
    bool mark_end_game(std::string game_guid);
    bool send_in_match_report(std::string match_guid, ReportInfo report_info);
    bool send_chat_message(std::string match_guid, std::string player_guid, std::string message);
    bool send_actions(std::vector<BaseActionData*> actions);
    bool send_action(BaseActionData* action);
    bool send_attack_action(std::string match_guid, std::string player_guid, std::string weapon_guid, long action_time_epoch);
    bool send_damage_action(
        std::string match_guid,
        std::string player_guid,
        std::string weapon_guid,
        std::string victim_player_guid,
        int damage_done,
        long action_time_epoch
    );
    bool send_heal_action(
        std::string match_guid,
        std::string player_guid,
        int health_gained,
        long action_time_epoch
    );
    bool send_spawn_action(
        std::string match_guid,
        std::string player_guid,
        std::string character_guid,
        int team_id,
        int initial_health,
        PositionF position,
        RotationF rotation,
        long action_time_epoch
    );
    bool send_death_action(
        std::string match_guid,
        std::string player_guid,
        long action_time_epoch
    );
    bool send_position_action(
        std::string match_guid,
        std::string player_guid,
        PositionF position,
        RotationF rotation,
        long action_time_epoch
    );
    
    bool send_reports(int titleId, std::string privateKey, std::vector<ReportInfo*> report_vector);
    bool send_reports(std::vector<ReportInfo*> report_vector); // will take title id and PK from env variables 
    bool update_players(int titleId, std::string privateKey, std::vector<PlayerInfo*> player_vector);
    bool update_players(std::vector<PlayerInfo*> player_vector); // will take title id and PK from env variables 
    void dispose();

}  // namespace GetGudSdk