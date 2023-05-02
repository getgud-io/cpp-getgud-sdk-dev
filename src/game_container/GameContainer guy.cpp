#include "GameContainer.h"
#include "logger/Logger.h"
#include "../../include/config/Config.h"

namespace GetGudSdk {

// create a new game container 
GameContainer game_container;

extern Config sdk_config;

bool GameContainer::AddGame(int titleId, std::string privateKey, std::string serverGuid, std::string gameMode)
{
  // before doing anything, make sure the container has enough room for another game
  if (aGame_vector.size() > sdk_config.max_games)
  {
      logger.log(LogType::WARNING, LogLevel::Processes, std::string("GameContainer::AddGame->Live game limit reached, cannot add a new game"));
      return false;
  }

  // create a GameData pointer so we can reference the same GameData object from different data structures
  GameData* game_data = new GameData(titleId, privateKey, serverGuid, gameMode);
  
  aGame_container_mutex.lock();

  // first, insert the game to the game map (for quick access when searching for a game via guid)
  std::pair<std::string, GameData*> game_guid_pair(game_data.get_game_guid(),game_data);
  aGame_map.insert(game_guid_pair);

  // second, insert the game to the game vector, and because this is a new game, its place is at the last of the line (which is the head of the vector)
  aGame_vector.insert(aGame_vector.begin(), game_data);

  aGame_container_mutex.unlock();

  logger.log(LogType::INFO, LogLevel::Processes, std::string("A new game has been added.\n" + game_data.toStringMeta()));
  
  return true;
}

}  // namespace GetGudSdk