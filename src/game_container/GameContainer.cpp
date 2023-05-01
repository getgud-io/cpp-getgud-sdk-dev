#include "GameContainer.h"

#include "../../include/config/Config.h"

namespace GetGudSdk {
// create a new shared packets buffer
GameContainer game_container;

extern Config sdk_config;

/**
* add_game:
* @game_data: game metadata
*  
* Add new game to the game container
**/
bool GameContainer::add_game(GameData game_data) {
  games_buffer_locker.lock();

  for (auto& game : games) 
  {
    if (game.get_game_guid() == game_data.get_game_guid())  // if game found
    {
      games_buffer_locker.unlock();
      return false; 
    }
  }

  games.push_back(game_data);
  games_buffer_locker.unlock();
  return true;
}

/**
* set_end_game:
* @game_guid: guid of the game that should be ended
*  
* End game by game_guid
**/
bool GameContainer::set_end_game(std::string game_guid) {
  games_buffer_locker.lock();
  for (auto& game : games)  
  {
    if (game.get_game_guid() == game_guid) 
    {
      auto current_point =
          std::chrono::system_clock::now(); 

      // TODO: why do we add delay?
      game.set_end_game(
          current_point +
          std::chrono::seconds(5)); 
    }
  }
  games_buffer_locker.unlock();
  return true;
}

/**
* add_match:
* @game_guid: guid of the game to which we are adding match
* @match_data: metadata of the match we are adding
*  
* Add new match to existing game packet
**/
bool GameContainer::add_match(std::string game_guid, MatchData match_data) {
  bool result = false;
  games_buffer_locker.lock();
  for (auto& game : games)  
  {
    if (game.get_game_guid() == game_guid) 
    {
      result = game.add_match(match_data);
    }
  }
  games_buffer_locker.unlock();
  return result;
}

/**
* add_report:
* @match_guid: guid of the match to which we are adding report
* @report_data: report metadata we are adding
*  
* Add report to specific match guid in the packet
**/
bool GameContainer::add_report(std::string match_guid, ReportData report_data) {
  games_buffer_locker.lock();

  auto reports_it = reports.find(match_guid); 

  if (reports_it != reports.end()) 
  {
    reports_it->second.push_back(report_data);  
  } else                                      
  {
    std::deque<ReportData> _report_buffer;  
    _report_buffer.push_back(report_data);  

    reports[match_guid] = _report_buffer; 
  }

  games_buffer_locker.unlock();

  return true;
}

/**
* add_chat_message:
* @match_guid: guid of the match to which we are adding report
* @message_data: chat metadata we are adding
*  
* Add chat message to specific match guid in the packet
**/
bool GameContainer::add_chat_message(std::string match_guid,
                                     ChatMessageData message_data) {
  games_buffer_locker.lock();

  auto messages_it =
      chat_messages.find(match_guid);

  if (messages_it != chat_messages.end())  
  {
    messages_it->second.push_back(message_data); 
  } else                                        
  {
    std::deque<ChatMessageData> _messages_buffer;  
    _messages_buffer.push_back(message_data);     

    chat_messages[match_guid] = _messages_buffer;
  }

  games_buffer_locker.unlock();

  return true;
}

/**
* check_games_limit:
*  
* Check if we have more open games in SDK than is allowed by config
**/
bool GameContainer::check_games_limit() {
  if (games.size() > sdk_config.max_games)
    return true;
  else
    return false;
}

/**
* check_matches_limit:
*  
* Check if we have more open matches in SDK than is allowed by config
* TODO: wrong logic, we should add mathes from all games togetger
**/
bool GameContainer::check_matches_limit(std::string game_guid) {

  bool result = true; 
  games_buffer_locker.lock();
  for (auto& game : games)  
  {
    if (game.get_game_guid() == game_guid) {
      if (game.get_matches_buffer().size() > sdk_config.max_matches_per_game)
        result = true; 
      else
        result = false;
      break;
    }
  }
  games_buffer_locker.unlock();

  return result;
}

/**
* dispose_oldest_game:
*  
* Pick the oldest game and dispose it
* TODO: should be replaced by queue
**/
bool GameContainer::dispose_oldest_game() {
  std::chrono::system_clock::time_point oldest_timer = std::chrono::system_clock::now();
  int oldest_game = 0; // TODO: rename to oldest_game_index or smth like this

  games_buffer_locker.lock();
  
  for (int i = 0; i < games.size(); i++) {
    if (games[i].get_start_time() < oldest_timer) {
      oldest_game = i;
      oldest_timer = games[i].get_start_time();
    }
  }

  games[oldest_game].dispose_matches();
  games.erase(games.begin() + oldest_game);

  games_buffer_locker.unlock();

  return true;
}

/**
* dispose:
*  
**/
void GameContainer::dispose() {
  games_buffer_locker.lock();
  games.clear();
  games_buffer_locker.unlock();
}

/**
* add_actions:
* @match_guid: guid of the match to which we are adding actions
* @actions_buffer: deque with all actions
*  
* Add all actions to the match data
**/
bool GameContainer::add_actions(std::string match_guid,
                                std::deque<BaseActionData*>& actions_buffer) {
  bool result = false;
  bool match_found = false;
  games_buffer_locker.lock();
  // TODO: update add_actions logic, make it thought game_container
  // TODO: what happens if when adding match we see that the buffer is full?
  for (auto& game : games) 
  {
    for (auto& match : game.get_matches_buffer())
    {
      if (match.first == match_guid) {
        unsigned int size_increase = 0;

        if (game.get_matches_buffer().empty())
          size_increase = game.get_game_size();

        size_increase = size_increase + match.second.push_actions(actions_buffer);
        game_container_size = game_container_size + size_increase;
        match_found = true;
        result = true;
        game.update_game();

        // we already added all actions to the match, we do not need to search further
        break; 
      }
    }
    
    // we already added all actions to the match, we do not need to search further
    if (match_found) 
      break;
  }
  games_buffer_locker.unlock();
  return result;
}

/**
* get_game_stream:
* @stream_out: guid of the match to which we are adding actions
* @erase: deque with all actions
*  
* Creates game packet for the first game in games (buffer). Can delete game packets if needed
* TODO: why first game in games? how are they stored
**/
void GameContainer::get_game_stream(std::string& stream_out, bool erase) {
  games_buffer_locker.lock();
  if (games.empty()) {
    games_buffer_locker.unlock();
    return;
  }

  stream_out.clear();

  stream_out += "{ \n";
  stream_out += "	\"privateKey\": \"" + sdk_config.private_key + "\",\n";

  if (erase) {
    GameData first_game = games[0];
    games.pop_front();
    games_buffer_locker.unlock();

    // reduce game container size because we delete the game
    game_container_size = game_container_size - first_game.get_game_size();

    // write json data of the game packet to stream_out
    first_game.game_to_string(stream_out);
  } else {
    // write json data of game packet to stream_out
    games[0].game_to_string(stream_out);
    games_buffer_locker.unlock();
  }
  stream_out += "}"; 
}

/**
* get_container_size:
*
**/
unsigned int GameContainer::get_container_size(){
  return game_container_size;
}

/**
* get_game_to_send:
*
* Determines the next game packet to be sent to Getgud
**/
GameData GameContainer::get_game_to_send() {
  GameData game_out;

  unsigned int packets_limit_in_bytes =
      sdk_config.packets_size_mb * 1024 * 1024; // TODO: should be in data holder


  // TODO: the name is wrong it is not percentage
  unsigned int packet_send_percentage =
      (packets_limit_in_bytes * sdk_config.send_packet_at_percentage) / 100; // TODO: should be in data holder

  games_buffer_locker.lock();
  auto game_it = games.begin();

  while (game_it != games.end()) {
    if (game_it->get_game_ended() ||  // if game is ended
        game_it->get_game_size() > packet_send_percentage  ||  // or if game size if overlimit
        game_it->get_end_by_time(sdk_config.live_game_timeout_seconds))  // or game inactive
    {
      game_out = *game_it; 
      games.erase(game_it);
      game_container_size = game_container_size - game_out.get_game_size();

      // we found the game to send and do not need to search anymore
      break;
    }
    game_it++;
  }

  games_buffer_locker.unlock();

  return game_out;
}


/**
* get_buffer:
*
**/
std::deque<GameData>& GameContainer::get_buffer() {
  return games;
};

/**
* get_matches:
* @game_guid: guid of the game for which we need to get matches
* 
* Get matches of specific game guid
**/
std::map<std::string, MatchData>* GameContainer::get_matches(
    std::string game_guid) {
  for (auto& game : games)  
  {
    if (game.get_game_guid() == game_guid)
    {
      return &game.get_matches_buffer();
    }
  }

  return nullptr;
};

}  // namespace GetGudSdk