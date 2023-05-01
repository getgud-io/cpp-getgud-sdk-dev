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

/**
 * Define all functions that client can use in Getgud SDK
 **/
namespace GetGudSdk {
    /**
     * init:
     * 
     * Initialize SDK variables and start the first GameSender thread so we can
     * create games and matches and send packets to the Getgud
     *
     **/
    void init();

    /**
     * start_game:
     * @title_id: title_id of the game that is currently starting. Title id can be
     * extracted from create_title request to our api
     * @server_name: server name the game is currently running in
     * @game_mode: Game mode the game is currently using
     *
     * Start new game, so we can attach match to it and form sending packets.
     * The game is ready to take new actions when the match is started.
     **/
    std::string start_game(
        int title_id,
        std::string server_name,
        std::string game_mode
    );

    /**
     * start_match:
     * @game_guid: guid of the match which the game belongs to, game_guid can be
     * extracted from start_game function
     * @match_mode: mode of match that is currently starting
     * @map_name: name of the map of the match
     *
     * Start new match, is called after start_game. When you call start_match you
     * can send_actions which will be attached to specific packets in the match
     **/
    std::string start_match(
        std::string game_guid,
        std::string match_mode,
        std::string map_name
    );

    /**
     * set_end_game: TODO: call end_game
     * @game_guid: guid of the game you want to stop
     *
     * Ends game running, notifies us that the game ended and sends all actions that
     * were not sent to us yet from packet of the closed game
     * so we can save and start processing them.
     **/
    bool set_end_game(std::string game_guid);

    /**
     * send_in_match_report:
     * @match_guid: guid of the match you want to send report for
     * @report_info: report data you are sending. TODO: call report_info
     *
     * Sends us report for the specific match. Reports can be sent from other
     * players in match, anticheats, internal systems. We use this reports to train our algorithms
     * and become better at cheater detection
     **/
    bool send_in_match_report(
        std::string match_guid,
        ReportInfo report_info
    );

    /**
     * send_chat_message:
     * @match_guid: guid of the match you want to send chat message for
     * @player_guid: report data you are sending. TODO: call report_info
     * @message:
     *
     * Sends us message sent in the chat of the match so we can store them along with
     * reports and match actions
     **/
    bool send_chat_message(
        std::string match_guid,
        std::string player_guid,
        std::string message
    );

    /**
     * send_actions:
     * @actions_array: array of all actions that you want to send
     * @size: number of actions you are sending
     *
     * Sends us actions that took place in the specific match of a game so we can process them
     * and generate insights and analytics
     **/
    bool send_actions(
        BaseActionData** actions_array,
        int size
    );

    /**
     * send_actions:
     * @actions: vector of all actions that you want to send
     *
     * Sends us actions that took place in the specific match of a game so we can process them
     * and generate insights and analytics
     **/
    bool send_actions(std::vector<BaseActionData*> actions);

    /**
     * send_actions:
     * @actions: deque of all actions that you want to send
     *
     * Sends us actions that took place in the specific match of a game so we can process them
     * and generate insights and analytics
     **/
    bool send_actions(std::deque<BaseActionData*> actions);

    /**
     * send_action:
     * @action: ONE action that you want to send to us
     *
     * Sends us action that took place in the specific match of a game so we can process it
     * and generate insights and analytics
     **/
    bool send_action(BaseActionData* action);

    /**
     * send_attack_action:
     * Use AttackActionData.h for reference
     *
     * Sends us ONE Attack Action that took place in the specific match of a game so we
     * can process it and generate insights and analytics
     **/
    bool send_attack_action(
        std::string match_guid,
        std::string player_guid,
        std::string weapon_guid,
        long action_time_epoch
    );

    /**
     * send_damage_action:
     * Use DamageActionData.h for reference
     *
     * Sends us ONE Damage Action that took place in the specific match of a game so we
     * can process it and generate insights and analytics
     **/
    bool send_damage_action(
        std::string match_guid,
        std::string player_guid,
        std::string weapon_guid,
        std::string victim_player_guid,
        int damage_done,
        long action_time_epoch
    );

    /**
     * send_heal_action:
     * Use HealActionData.h for reference
     *
     * Sends us ONE Heal Action that took place in the specific match of a game so we
     * can process it and generate insights and analytics
     **/
    bool send_heal_action(
        std::string match_guid,
        std::string player_guid,
        int health_gained,
        long action_time_epoch
    );

    /**
     * send_spawn_action:
     * Use SpawnActionData.h for reference
     *
     * Sends us ONE Spawn Action that took place in the specific match of a game so we
     * can process it and generate insights and analytics
     **/
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

    /**
     * send_death_action:
     * Use DeathActionData.h for reference
     *
     * Sends us ONE Death Action that took place in the specific match of a game so we
     * can process it and generate insights and analytics
     **/
    bool send_death_action(
        std::string match_guid,
        std::string player_guid,
        long action_time_epoch
    );

    /**
     * send_position_action:
     * Use PositionActionData.h for reference
     *
     * Sends us ONE Position Action that took place in the specific match of a game so we
     * can process it and generate insights and analytics
     **/
    bool send_position_action(
        std::string match_guid,
        std::string player_guid,
        PositionF position,
        RotationF rotation,
        long action_time_epoch
    );
    
    /**
    * dispose: 
    * 
    * Dispose actions_buffer and game_container also stop all GameSender scripts
    **/
    void dispose();

    /**
     * TODO: delete this once everything is tested
     **/
    namespace Debug {
        /**
         * get_actions_buffer:
         * 
         * tmp function, TODO: delete later
         **/
        std::deque<BaseActionData*> get_actions_buffer();

        /**
         * get_game_container:
         * 
         * tmp function, TODO: delete later
         **/
        std::deque<GameData>& get_game_container();

        /**
         * get_matches_buffer:
         * 
         * tmp function, TODO: delete later
         **/
        std::map<std::string, MatchData>* get_matches_buffer(std::string game_guid);
    }  // namespace Debug

}  // namespace GetGudSdk