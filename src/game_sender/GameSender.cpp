#include "GameSender.h"
#include "../logger/Logger.h"
#include "../../include/config/Config.h"

namespace GetGudSdk {
extern Logger logger;
extern ActionsBuffer actions_buffer;
extern GameContainer game_container;
extern Config sdk_config;

/**
* GameSender:
* 
* Starts new game sender thread
**/
GameSender::GameSender() {
  if (!curl)
    init_curl();
  if (!thread_working)
    start_thread();
}

/**
* GameSender:
* 
* Stop game sender thread
**/
GameSender::~GameSender() {
  logger.log(LogType::MESSAGE, LogLevel::Processes, "Game sender stoped");
  stop_required = true;
  updater_thread.detach();
}

/**
* init_curl:
* 
* Prepare curl for sending game packets to Getgud
**/
void GameSender::init_curl() {
  logger.log(LogType::MESSAGE, LogLevel::Responses,
             "REST Api connection prepared");

  curl = curl_easy_init();
  headers = curl_slist_append(headers, "Accept: application/json");
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "charset: utf-8");

  // TODO: describe what this does
  curl_easy_setopt(curl, CURLOPT_POST, 1); 
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_URL, sdk_config.rest_api_link);
}

/**
* start_thread:
* 
* Prepare curl for sending game packets to Getgud
**/
void GameSender::start_thread() {
  thread_working = true;
  logger.log(LogType::MESSAGE, LogLevel::Processes,
             "Game Sender Thread started");

  // TODO: describe why thread sleeps for one sec
  // TODO: ALL numbers should be outside code
  updater_thread = std::thread([&]() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    update_loop(); 
        }); 
}

/**
* check_actions:
* 
* Checks if there are any more actions in the action buffer that this game sender
* can grab
**/
void GameSender::check_actions() {
  std::deque<BaseActionData*> load_buffer;

  unsigned int buffer_to_append_size = 0;

  // Check if there are any new actions for this game sender in the actions buffer
  buffer_to_append_size = actions_buffer.get_actions(&load_buffer);

  if (buffer_to_append_size == 0) 
  {
    return;  
  }

  // TODO: ALL numbers should be outside code
  // If game container doesn't have enough memory we delete oldest game
  if (buffer_to_append_size + game_container.get_container_size() >
      sdk_config.packets_size_mb * 1024 * 1024) {
    game_container.dispose_oldest_game();
  }

  std::unordered_map<std::string, std::deque<BaseActionData*>> splitted_actions;

  // put actions from actions buffer to their respective matches
  split_actions_by_match(load_buffer, splitted_actions);  
  append_splitted_actions(splitted_actions); 
}

/**
* update_games:
* 
* Check if there are any ended games that we can send.
* TODO: the name is bad, should be send_ready_game_packet 
**/
void GameSender::update_games() {

  // Grab the game which is ready to be sent
  GameData check_data = game_container.get_game_to_send(); 
  
  // check the game is valid and not empty
  if (!check_data.get_game_guid()
           .empty())
  {
    logger.log(LogType::MESSAGE, LogLevel::Processes,
               "Game ended: " + check_data.get_game_guid() + "\n\t matches: " +
                   std::to_string(check_data.get_matches_buffer().size()));
    std::string out_packet;

    // prepare game packet and send
    check_data.game_to_string(out_packet); 
    CURLcode send_code = send_packet(out_packet);

    if (send_code == CURLcode::CURLE_OK) {
      logger.log(LogType::MESSAGE, LogLevel::Responses,
                 "Packet sent: " + check_data.get_game_guid());
    } else {
      logger.log(LogType::_ERROR, LogLevel::Responses,
                 "Packet didn't send: " + check_data.get_game_guid());
    }
  }
  // TODO: explain why we dispose matches but not games
  check_data.dispose_matches();
}

/**
* update_loop:
* 
* One iteration of game sender. Check for new actions, check if there are any games ready
* for sending
**/
void GameSender::update_loop() {
  try {
  while (!stop_required) {
    // TODO: dispose the thread if it does nothing some times
    check_actions();
    update_games();

    // Go sleep before you wake up next time
    std::this_thread::sleep_for(
        std::chrono::seconds(sdk_config.send_packet_interval_seconds)); 
  }
  } catch (...) {
  logger.log(LogType::_ERROR, LogLevel::Processes,
             std::string("Game Sender update loop error"));
  }
}


/**
* append_splitted_actions:
* @actions_in: actions splitted by match
* 
* Takes actions already splitted by match and puts them into respective packet part
**/
void GameSender::append_splitted_actions(
    std::unordered_map<std::string, std::deque<BaseActionData*>>& actions_in) {
 

  for (auto& sorted_actions : actions_in) {
    logger.log(LogType::MESSAGE, LogLevel::Processes,
               "Match: " + sorted_actions.first +
                   "\n\t actions pushed by Game Sender, size: " +
                   std::to_string(sorted_actions.second.size()));

    game_container.add_actions(
        sorted_actions.first,
        sorted_actions.second);  // add by match_guid and data buffer pair
  }
}

/**
* split_actions_by_match:
* @actions_in: actions to be splitted by match, each action has match_guid
* @actions_out: match_id and all match actions buffer
* 
* Split all actions from actions_in into actions_out
* TODO: rename variables
**/
void GameSender::split_actions_by_match(
    std::deque<BaseActionData*>& actions_in,
    std::unordered_map<std::string, std::deque<BaseActionData*>>&
        actions_out) 
{
  for (auto& action : actions_in) {
    if (action == nullptr) 
      continue;             

    logger.log(LogType::MESSAGE, LogLevel::Responses,
               "REST Api connection prepared");

    auto find_it = actions_out.find(action->match_guid);
    if (find_it == actions_out.end())                     
    {
      // make new sorted buffer element with match_guid and deque buffer pair
      actions_out.emplace(std::make_pair(
          action->match_guid, std::deque<GetGudSdk::BaseActionData*>()));
    }

    auto& match_actions_buffer =
        actions_out[action->match_guid]; 

    actions_out[action->match_guid].push_back(
        action); 
  }
}

/**
* send_packet:
* @packet: packet ready to be sent
* 
* Take packet ready to be sent and send it via curl
**/
CURLcode GameSender::send_packet(std::string& packet) {
  if (!curl) 
  {
    return CURLcode::CURLE_SEND_ERROR;
  }

  // set packet string to send
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS,
                   packet.c_str());             
  
  // send data
  CURLcode send_code = curl_easy_perform(curl); 

  return send_code;
}
}  // namespace GetGudSdk