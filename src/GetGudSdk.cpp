#include "../include/GetGudSdk.h"
#include "game_sender/GameSender.h"
#include "logger/Logger.h"
#include "utils/Utils.h"

#include "../include/config/Config.h"

namespace GetGudSdk {
  std::vector<GameSender*> senders;

  extern Config sdk_config;
  extern Logger logger;
  extern ActionsBuffer actions_buffer;
  extern GameContainer game_container;


  /**
   * create_game_sender:
   *
   * Creates new game sender thread so more GameSender can be utilized in case
   * hyper mode is activated
   * TODO: Shouldn't be in GetGudSdk should be in something like ThreadManager
   **/
  void create_game_sender() {
    // if we don't have more threads then allowed
    // we can create one more thread
    if (senders.size() < sdk_config.game_senders_limit) {
      senders.push_back(new GameSender());
    }
  }

  void erase_game_sender() {
    if (senders.size() > 1)
      senders.pop_back();
  }

  /**
   * check_actions_limit:
   *
   * Creates new game sender thread so more GameSender can be utilized in case
   * hyper mode is activated
   * TODO: Shouldn't be in GetGudSdk
   * TODO: naming of method is misleading
   **/
  bool check_actions_limit() {
    if (actions_buffer.check_buffer_limit()) {
      return false; //if check is proc
    }

    if (sdk_config.hyper_speed_feature_enabled) {
      int threads_required = actions_buffer.check_buffer_load();

      if (threads_required > senders.size()) {
        threads_required = threads_required - senders.size();
        while (threads_required > 0) {
          create_game_sender();
          threads_required = threads_required - 1;
        }
      }
    }

    //TODO should we erase when actions_buffer size is small?
    return true;
  }

  // uses inside SDK
  bool check_games_limit() {
    if (game_container.check_games_limit()) {
      return true;
    } else {
      return false;
    }
  }
  
  // uses inside SDK
  bool check_matches_limit(std::string game_guid) {
    if (game_container.check_matches_limit(game_guid)) {
      return true;
    } else {
      return false;
    }
  }

  Config& get_sdk_config() {
    return sdk_config;
  }

  /**
  * init:
  * Initialize SDK variables and start the first GameSender thread so we can
  * create games and matches and send packets to the Getgud
  *
  **/
  void init() {
    try {
      sdk_config.load_settings();
      create_game_sender();
    } catch (...) {
      logger.log(LogType::_ERROR, LogLevel::Processes,
                 std::string("SDK is not init"));
    }
  }

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
  std::string start_game(int title_id,
                         std::string server_name,
                         std::string game_mode) {
    std::string game_guid;
    try {
      // make sure we don't open a new game if we already have miximum games in
      // the system
      bool result = check_games_limit();
      if (result) {
        logger.log(LogType::WARNING, LogLevel::Processes,
                   std::string("Game is not started, Games limit is over"));
        return std::string();
      }

      game_guid = generate_guid();
      GameData game_data(game_guid, title_id, server_name, game_mode);

      // add the new created game so it can be later accessed by one of
      // GameSenders
      result = game_container.add_game(game_data);

      if (result) {
        logger.log(LogType::MESSAGE, LogLevel::Actions,
                   std::string("Game started: " + game_guid));
      }

    } catch (...) {
      logger.log(LogType::_ERROR, LogLevel::Processes,
                 std::string("Game is not started"));
    }

    return game_guid;
  }

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
  std::string start_match(std::string game_guid,
                          std::string match_mode,
                          std::string map_name) {
    std::string match_guid;
    try {
      // make sure we do not create a new match if we already have too many
      // matches in the system
      bool result = check_matches_limit(game_guid);

      if (result) {
        logger.log(LogType::WARNING, LogLevel::Processes,
                   std::string("Match: " + game_guid +
                               " is not started, Matches limit is over"));
        return std::string();
      }

      match_guid = generate_guid();

      MatchData match_data(match_guid, match_mode,
                           map_name);  // fill match data

      // add the new created match so it can be later accessed by one of
      // GameSenders inside the game_container
      // TODO: rename to something more meaningful in ALL functions in
      // GetGudSdk.cpp like is_match_added
      result = game_container.add_match(game_guid, match_data);

      if (result) {
        logger.log(LogType::MESSAGE, LogLevel::Actions,
                   std::string("Match started: " + match_guid +
                               "\n\tgame:" + game_guid));
      }

    } catch (...) {
      logger.log(LogType::_ERROR, LogLevel::Processes,
                 std::string("Match is not started"));
    }

    return match_guid;
  }

  /**
  * set_end_game: TODO: call end_game
  * @game_guid: guid of the game you want to stop
  *
  * Ends game running, notifies us that the game ended and sends all actions that
  * were not sent to us yet from packet of the closed game
  * so we can save and start processing them.
  **/
  bool set_end_game(std::string game_guid) {
    bool result = false;
    try {
      // mark specific game as "finished" inside game_container so the packet
      // for this game can be sent to getgud by one of the GameSender instances
      result = game_container.set_end_game(game_guid);

      if (result) {
        logger.log(LogType::MESSAGE, LogLevel::Actions,
                   std::string("End game: " + game_guid));
      }

    } catch (...) {
      logger.log(LogType::_ERROR, LogLevel::Processes,
                 std::string("Game could not end"));
    }

    return result;
  }

  /**
  * send_in_match_report:
  * @match_guid: guid of the match you want to send report for
  * @report_info: report data you are sending. TODO: call report_info
  *
  * Sends us report for the specific match. Reports can be sent from other
  * players in match, anticheats, internal systems. We use this reports to train our algorithms
  * and become better at cheater detection
  **/
  bool send_in_match_report(std::string match_guid, ReportInfo report_info) {
    ReportData report_data(report_info);
    bool result = false;
    try {
      // add report to specific match instance in game container, it will be
      // sent along with actions, game, match metadata and chat messages in the
      // next game packet by one of the GameSenders
      result = game_container.add_report(match_guid, report_data);
    } catch (...) {
      logger.log(LogType::_ERROR, LogLevel::Processes,
                 std::string("Match report could not send"));
    }
    return result;
  }

  /**
  * send_chat_message:
  * @match_guid: guid of the match you want to send chat message for
  * @player_guid: report data you are sending. TODO: call report_info
  * @message:
  *
  * Sends us message sent in the chat of the match so we can store them along with
  * reports and match actions
  **/
  bool send_chat_message(std::string match_guid,
                         std::string player_guid,
                         std::string message) {
    bool result = false;
    try {
    ChatMessageData chat_message_data(player_guid, message);

    // add chat message to specific match instance in game container, it will be sent along with
    // actions, game, match metadata and reports in the next game packet by one of the GameSenders
    result = game_container.add_chat_message(match_guid, chat_message_data);
    } catch (...) {
    logger.log(LogType::_ERROR, LogLevel::Processes,
               std::string("Match message could not send"));
    }
    return result;
  }

  /**
  * send_actions:
  * @actions_array: array of all actions that you want to send
  * @size: number of actions you are sending
  *
  * Sends us actions that took place in the specific match of a game so we can process them
  * and generate insights and analytics
  **/
  bool send_actions(BaseActionData** actions_array, int size) {

    bool result = false;
    try {
    // make sure we do not have more actions in buffer than allowed by config. 
    // the function will also spawn more processes if there is becoming too many actions 
    // TODO: rename to something  meaningful like is_enough_space_in_buffer
    result = check_actions_limit();

    if (!result)
      return false;

    std::deque<BaseActionData*> _actions;

    std::string actions_log = "\n";

    // add all actions provided to the tmp data structure so they can be later be added to
    // actions buffer
    for (int i = 0; i < size; i++) {
      _actions.push_back(actions_array[i]->clone());
      actions_log += "\t" + actions_array[i]->match_guid + "\n";
    }

    // delete the last endline from log so the logger can display correctly
    actions_log.pop_back(); 

    // add all actions we have into the actions_buffer so they can be later accessed by
    // one of the GameSender threads
    result = actions_buffer.add_actions(_actions);
    if (result) {
      logger.log(LogType::MESSAGE, LogLevel::Actions,
                 std::string("Actions array pushed: size " +
                   std::to_string(size) + actions_log));
    }
    } catch (...) {
    logger.log(LogType::_ERROR, LogLevel::Processes,
               std::string("Actions array could not send"));
    }
    return result;
  }

  /**
  * send_actions:
  * @actions: deque of all actions that you want to send
  *
  * Sends us actions that took place in the specific match of a game so we can process them
  * and generate insights and analytics
  **/
  bool send_actions(std::deque<BaseActionData*> actions) {
    bool result = false;
    try {
    // make sure we do not have more actions in buffer than allowed by config. 
    // the function will also spawn more processes if there is becoming too many actions 
    // TODO: rename to something  meaningful like is_enough_space_in_buffer
    result = check_actions_limit();

    if (!result)
      return false;

    std::deque<BaseActionData*> _actions;

    std::string actions_log = "\n";

    // add all actions provided to the tmp data structure so they can be later be added to
    // actions buffer
    for (auto& action : actions) {
      _actions.push_back(action->clone());
      actions_log += "\t" + action->match_guid + "\n";
    }

    // delete the last endline from log so the logger can display correctly
    actions_log.pop_back(); 

    // add all actions we have into the actions_buffer so they can be later accessed by
    // one of the GameSender threads
    result = actions_buffer.add_actions(_actions);
    if (result) {
      logger.log(LogType::MESSAGE, LogLevel::Actions,
                 std::string("Actions array pushed: size " +
                   std::to_string(actions.size()) + actions_log));
    }
    } catch (...) {
    logger.log(LogType::_ERROR, LogLevel::Processes,
               std::string("Actions deque could not send"));
    }
    return result;
  }

  /**
  * send_actions:
  * @actions: vector of all actions that you want to send
  *
  * Sends us actions that took place in the specific match of a game so we can process them
  * and generate insights and analytics
  **/
  bool send_actions(std::vector<BaseActionData*> actions) {
    bool result = false;
    try {
    // make sure we do not have more actions in buffer than allowed by config. 
    // the function will also spawn more processes if there is becoming too many actions 
    // TODO: rename to something  meaningful like is_enough_space_in_buffer
    result = check_actions_limit();

    if (!result)
      return false;

    std::deque<BaseActionData*> _actions;
    std::string actions_log = "\n";

    // add all actions provided to the tmp data structure so they can be later be added to
    // actions buffer
    for (auto& action : actions) {
      _actions.push_back(action->clone());
      actions_log += "\t" + action->match_guid + "\n";
    }

    // delete the last endline from log so the logger can display correctly
    actions_log.pop_back(); 

    // add all actions we have into the actions_buffer so they can be later accessed by
    // one of the GameSender threads
    result = actions_buffer.add_actions(_actions);
    if (result) {
      logger.log(LogType::MESSAGE, LogLevel::Actions,
                 std::string("Actions array pushed: size " +
                   std::to_string(actions.size()) + actions_log));
    }
    } catch (...) {
    logger.log(LogType::_ERROR, LogLevel::Processes,
               std::string("Actions vector could not send"));
    }
    return result;
  }
  
  /**
  * send_action:
  * @action: ONE action that you want to send to us
  *
  * Sends us action that took place in the specific match of a game so we can process it
  * and generate insights and analytics
  **/
  bool send_action(BaseActionData* action) {
    bool result = false;
    try {
    // make sure we do not have more actions in buffer than allowed by config. 
    // the function will also spawn more processes if there is becoming too many actions 
    // TODO: rename to something  meaningful like is_enough_space_in_buffer
    result = check_actions_limit();

    if (!result)
      return false;

    // add action provided to the tmp data structure so it can be later be added to
    // actions buffer
    BaseActionData* clone_data = action->clone();

    // add action we have into the actions_buffer so it can be later accessed by
    // one of the GameSender threads
    result = actions_buffer.add_action(clone_data);
    if (result) {
      logger.log(LogType::MESSAGE, LogLevel::Actions,
                 std::string("Action pushed: " + action->match_guid));
    }
    } catch (...) {
    logger.log(LogType::_ERROR, LogLevel::Processes,
               std::string("Single action could not send"));
    }
    return result;
  }

  /**
  * send_attack_action:
  * Use AttackActionData.h for reference
  *
  * Sends us ONE Attack Action that took place in the specific match of a game so we
  * can process it and generate insights and analytics
  **/
  bool send_attack_action(std::string match_guid,
                          std::string player_guid,
                          std::string weapon_guid,
                          long action_time_epoch) {
    bool result = false;
    try {
    // make sure we do not have more actions in buffer than allowed by config. 
    // the function will also spawn more processes if there is becoming too many actions 
    // TODO: rename to something  meaningful like is_enough_space_in_buffer                        
    result = check_actions_limit();

    if (!result)
      return false;

    // add action provided to the tmp data structure so it can be later be added to
    // actions buffer
    AttackActionData* attack_data = new AttackActionData(
        player_guid, match_guid, action_time_epoch, weapon_guid);

    // add action we have into the actions_buffer so it can be later accessed by
    // one of the GameSender threads
    result = actions_buffer.add_action(attack_data);

    if (result) {
      logger.log(LogType::MESSAGE, LogLevel::Actions,
                 std::string("Action pushed: " + match_guid));
    }
    } catch (...) {
    logger.log(LogType::_ERROR, LogLevel::Processes,
               std::string("Attack action could not send"));
    }
    return result;
  }

  /**
  * send_damage_action:
  * Use DamageActionData.h for reference
  *
  * Sends us ONE Damage Action that took place in the specific match of a game so we
  * can process it and generate insights and analytics
  **/
  bool send_damage_action(std::string match_guid,
                          std::string player_guid,
                          std::string weapon_guid,
                          std::string victim_player_guid,
                          int damage_done,
                          long action_time_epoch) {
    bool result = false;
    try {
    // make sure we do not have more actions in buffer than allowed by config. 
    // the function will also spawn more processes if there is becoming too many actions 
    // TODO: rename to something  meaningful like is_enough_space_in_buffer                               
    result = check_actions_limit();

    if (!result)
      return false; 

    // add action provided to the tmp data structure so it can be later be added to
    // actions buffer
    DamageActionData* damage_data =
      new DamageActionData(player_guid, match_guid, action_time_epoch,
                           damage_done, victim_player_guid, weapon_guid);
    
    // add action we have into the actions_buffer so it can be later accessed by
    // one of the GameSender threads
    result = actions_buffer.add_action(damage_data);

    if (result) {
      logger.log(LogType::MESSAGE, LogLevel::Actions,
                 std::string("Action pushed: " + match_guid));
    }
    } catch (...) {
    logger.log(LogType::_ERROR, LogLevel::Processes,
               std::string("Attack action could not send"));
    }
    return result;
  }

  /**
  * send_heal_action:
  * Use HealActionData.h for reference
  *
  * Sends us ONE Heal Action that took place in the specific match of a game so we
  * can process it and generate insights and analytics
  **/
  bool send_heal_action(std::string match_guid,
                        std::string player_guid,
                        int health_gained,
                        long action_time_epoch) {
    bool result = false;
    try {
    // make sure we do not have more actions in buffer than allowed by config. 
    // the function will also spawn more processes if there is becoming too many actions 
    // TODO: rename to something  meaningful like is_enough_space_in_buffer  
    bool result = check_actions_limit();

    if (!result)
      return false; 

    // add action provided to the tmp data structure so it can be later be added to
    // actions buffer
    HealActionData* heal_data = new HealActionData(
        player_guid, match_guid, action_time_epoch, health_gained);

    // add action we have into the actions_buffer so it can be later accessed by
    // one of the GameSender threads
    result = actions_buffer.add_action(heal_data);

    if (result) {
      logger.log(LogType::MESSAGE, LogLevel::Actions,
                 std::string("Action pushed: " + match_guid));
    }
    } catch (...) {
    logger.log(LogType::_ERROR, LogLevel::Processes,
               std::string("Heal action could not send"));
    }
    return result;
  }

  /**
  * send_spawn_action:
  * Use SpawnActionData.h for reference
  *
  * Sends us ONE Spawn Action that took place in the specific match of a game so we
  * can process it and generate insights and analytics
  **/
  bool send_spawn_action(std::string match_guid,
                         std::string player_guid,
                         std::string character_guid,
                         int team_id,
                         int initial_health,
                         PositionF position,
                         RotationF rotation,
                         long action_time_epoch) {
    bool result = false;
    try {
    // Watch comments in any add_action function for reference
    result = check_actions_limit();

    if (!result)
      return false;

    SpawnActionData* spawn_data =
      new SpawnActionData(player_guid, match_guid, action_time_epoch, position,
                          rotation, initial_health, team_id, character_guid);

    result = actions_buffer.add_action(spawn_data);

    if (result) {
      logger.log(LogType::MESSAGE, LogLevel::Actions,
                 std::string("Action pushed: " + match_guid));
    }
    } catch (...) {
    logger.log(LogType::_ERROR, LogLevel::Processes,
               std::string("Spawn action could not send"));
    }
    return result;
  }
  
  /**
  * send_death_action:
  * Use DeathActionData.h for reference
  *
  * Sends us ONE Death Action that took place in the specific match of a game so we
  * can process it and generate insights and analytics
  **/
  bool send_death_action(std::string match_guid,
                         std::string player_guid,
                         long action_time_epoch) {
    bool result = false;
    try {
    // Watch comments in any add_action function for reference                      
    result = check_actions_limit();

    if (!result)
      return false;

    DeathActionData* spawn_data =
      new DeathActionData(player_guid, match_guid, action_time_epoch);

    result = actions_buffer.add_action(spawn_data);

    if (result) {
      logger.log(LogType::MESSAGE, LogLevel::Actions,
                 std::string("Action pushed: " + match_guid));
    }
    } catch (...) {
    logger.log(LogType::_ERROR, LogLevel::Processes,
               std::string("Death action could not send"));
    }
    return result;
  }
  /**
  * send_position_action:
  * Use PositionActionData.h for reference
  *
  * Sends us ONE Position Action that took place in the specific match of a game so we
  * can process it and generate insights and analytics
  **/
  bool send_position_action(std::string match_guid,
                            std::string player_guid,
                            PositionF position,
                            RotationF rotation,
                            long action_time_epoch) {
    bool result = false;
    try {
    // Watch comments in any add_action function for reference                               
    result = check_actions_limit();

    if (!result)
      return false; 

    PositionActionData* spawn_data = new PositionActionData(
        player_guid, match_guid, action_time_epoch, position, rotation);

    result = actions_buffer.add_action(spawn_data);

    if (result) {
      logger.log(LogType::MESSAGE, LogLevel::Actions,
                 std::string("Action pushed: " + match_guid));
    }
    } catch (...) {
    logger.log(LogType::_ERROR, LogLevel::Processes,
               std::string("Position action could not send"));
    }
    return result;
  }

  /**
  * dispose: 
  * 
  * Dispose actions_buffer and game_container also stop all GameSender scripts
  * TODO: shouldn't it stop all game sender threads?
  **/
  void dispose() {
    try {
    actions_buffer.dispose();
    game_container.dispose();

    logger.log(LogType::MESSAGE, LogLevel::Processes,
               std::string("SDK disposed"));
    } catch (...) {
    logger.log(LogType::_ERROR, LogLevel::Processes,
               std::string("Dispose error"));
    }
  }

  namespace Debug
  /**
  * TODO: describe how is this used, if not used delete it
  **/
  {
    std::deque<BaseActionData*> get_actions_buffer() {
      std::deque<BaseActionData*> out_buffer;
      actions_buffer.get_actions(&out_buffer);

      return out_buffer;
    }

    std::deque<GameData>& get_game_container() {
      std::deque<GameData>& data = game_container.get_buffer();

      return game_container.get_buffer();
    }

    std::map<std::string, MatchData>* get_matches_buffer(std::string game_guid) {
      std::map<std::string, MatchData>* data =
        game_container.get_matches(game_guid);

      return data;
    }
  }  // namespace Debug
}  // namespace GetGudSdk