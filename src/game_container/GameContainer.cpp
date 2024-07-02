#include "GameContainer.h"
#include "../config/Config.h"
#include "../logger/Logger.h"

namespace GetgudSDK {

GameContainer gameContainer;

extern Config sdkConfig;
extern Logger logger;

/**
 * AddGame:
 *
 **/
std::string GameContainer::AddGame(int titleId,
                                   std::string privateKey,
                                   std::string serverGuid,
                                   std::string gameMode,
                                   std::string serverLocation) {
	std::string gameGuid;

	// make sure the container has enough room for another game
	if (m_gameVector.size() >= sdkConfig.maxGames) {
		logger.Log(LogType::WARN,std::string("GameContainer::AddGame->Live game limit reached - cannot add a new game"));
		return gameGuid;
	}

	// create a GameData and a pointer to it so we can reference the same GameData
	// object from different data structures
	GameData* gameData = new GameData(titleId, privateKey, serverGuid, gameMode, serverLocation);

	if (!gameData->IsValid()) {

        logger.Log(LogType::WARN, std::string("GameContainer::AddGame->One or more of the Game's paraemters are not valid - Game will not start. Game paraemters: Title ID: " + std::to_string(titleId) + " | serverGuid: " + serverGuid + " | gameMode: " + gameMode + " | serverLocation: " + serverLocation));
		delete gameData;
		return gameGuid;
	}

	m_gameContainerMutex.lock();

	// insert the game to the game map (for quick access when searching for a game
	// via guid)
	auto gameGuidPair = std::make_pair(gameData->GetGameGuid(), gameData);
	m_gameMap.insert(gameGuidPair);

	// insert the game to the game vector, because this is a new game, its place
	// is at the last of the line (which is the tail of the vector)
	m_gameVector.push_back(gameData);
	gameGuid = gameData->GetGameGuid();
	std::string gameStringMeta = gameData->ToStringMeta();
	m_gameContainerMutex.unlock();

	logger.Log(LogType::DEBUG, std::string("Started a new Game: " + gameGuid));

	// return the guid of the match, this acts as it's key to finding it
	return gameGuid;
}

/**
 * AddMatch:
 *
 **/
std::string GameContainer::AddMatch(std::string gameGuid,
                                    std::string matchMode,
                                    std::string mapName) {
	std::string retValue;
	MatchData* matchData = nullptr;
	std::string matchStringMeta;

	m_gameContainerMutex.lock();

	//  find the game that this new match is asking to join to
	auto game_it = m_gameMap.find(gameGuid);
	if (game_it == m_gameMap.end()) {
		logger.Log(LogType::WARN, std::string("GameContainer::AddMatch->Can't find game with guid \"" + gameGuid + "\" to push match into - Match will not start"));
		m_gameContainerMutex.unlock();
		return retValue;
	}
	// create a new match from the passed parameters and add it to the game we
	// found above
	matchData = game_it->second->AddMatch(matchMode, mapName);
	if (matchData == nullptr) {
		m_gameContainerMutex.unlock();
		return retValue;
  }

  // insert the match to the match map (for quick access when searching for
  // a match via guid, for example, when inserting actions)
  auto matchGuidPair = std::make_pair(matchData->GetMatchGuid(), matchData);
  m_matchMap.insert(matchGuidPair);

  // return the guid of the match, this acts as it's key to finding it
  retValue = matchData->GetMatchGuid();
  matchStringMeta = matchData->ToStringMeta();

  m_gameContainerMutex.unlock();

  if (matchData != nullptr) {
    logger.Log(LogType::DEBUG,
               std::string("Started a new Match: " + retValue +
                           " for the Game: " + matchData->GetGameGuid()));
  }

  return retValue;
}

/**
 * AddActions:
 *
 **/
bool GameContainer::AddActions(std::deque<BaseActionData*>& actionVector) {
  //  first check that we are allowed to add more action to the game container
  //  and did not run out of allowed memory to consume
  m_gameContainerMutex.lock();
  bool result = true;

  // if game is full we add empty actions that will then trigger deletion of
  // game because they are invalid
  if (m_gameContainerSizeInBytes >= sdkConfig.gameContainerMaxSizeInBytes &&
      actionVector.size() > 0) {
    std::deque<BaseActionData*> emptyActionVector;
    for (auto& action : actionVector) {
      auto* emptyAction = new BaseActionData(BaseData(), true);
      emptyAction->m_matchGuid = action->m_matchGuid;
      emptyActionVector.push_back(emptyAction);
      delete action;
    }
    actionVector.clear();
    actionVector = emptyActionVector;

    result = false;
  }

  // this local (temp) map will hold all the actions per match. we do this to
  // inject all the actions to a match at the same time (and for that we need to
  // sort them by match guid first)
  std::unordered_map<MatchData*, std::vector<BaseActionData*>> matchActionsMap;

  // this vector will hold all the match guids which have new actions coming in
  std::vector<MatchData*> matchPtrVector;

  int matchSizeInBytes = 0;

  // calculate the size of the new actions vector here to avoid calling this
  // method in the 'for loop' which happens many times
  MatchData* matchData = nullptr;

  // cluster all new actions according to their match guid and place them in a
  // dedicated vector per match guid
  bool failedToPushSomeActions = false;
  for (auto& nextAction : actionVector) {
    // find the match that belongs to the action we are now iterating
    auto match_it = m_matchMap.find(nextAction->m_matchGuid);
    if (match_it == m_matchMap.end() || match_it->second == nullptr) {
      delete nextAction;
      failedToPushSomeActions = true;
      continue;  // if a match with the passed guid was not found, just
                 // ignore the action
    }

    matchData = match_it->second;

    // validate the structure and parameters of the next action we are going to
    // assimilate
    if (nextAction->IsValid() == false) {
      // action is invalid and thus -> we are going to delete the entire game :/
      // why? Ask Art!
      logger.Log(
          LogType::DEBUG,
          std::string("Action is invalid OR empty because Game Container or Action Buffer is full"));
      logger.Log(LogType::DEBUG, nextAction->ToStringMeta());
      DeleteGame(matchData->GetGameGuid(), false, matchPtrVector);
      delete nextAction;
      continue;
    }

    //  get the local vector (from the local map) that belong longs to this
    //  match and insert the action to it
    auto matchActions_it = matchActionsMap.find(matchData);
    if (matchActions_it == matchActionsMap.end()) {
      // the vector might not exists yet since this is the first time an action
      // with this match guid appeared in the method call

      std::vector<BaseActionData*> newMatchActions;
      newMatchActions.push_back(nextAction);
      auto matchActionsPair = std::make_pair(matchData, newMatchActions);
      matchActionsMap.insert(matchActionsPair);
      // save the match pointer so we can iterate over all of them later on
      matchPtrVector.push_back(matchData);
    } else {
      // insert the new action to the temp vector which is associated to the
      // same match guid that the action is
      matchActions_it->second.push_back(nextAction);
    }
  }

  if (failedToPushSomeActions)
    logger.Log(LogType::WARN,
               "GameContainer::AddActions->Failed to push some actions to "
               "GameContainer because no matching guid was found!");

  // now run through all the matches that have new actions and insert the new
  // action vector to the match all at once while doing it, need to make sure
  // the game container size parameter is update accordingly
  for (auto* matchPtr : matchPtrVector) {
    auto matchActions_it = matchActionsMap.find(matchPtr);
    if (matchActions_it == matchActionsMap.end() ||
        matchActions_it->first == nullptr)
      continue;
    // save the size of the match before inserting the new actions
    matchSizeInBytes = matchPtr->GetMatchSizeInBytes();
    matchPtr->AddActions(matchActions_it->second);

    std::string matchGameGuid = matchPtr->GetGameGuid();
    auto game_it = m_gameMap.find(matchGameGuid);
    if (game_it == m_gameMap.end() || game_it->second == nullptr)
      continue;
    game_it->second->UpdateLastUpdateTime();

    // update the game container size after adding the new messages to the match
    m_gameContainerSizeInBytes +=
        matchPtr->GetMatchSizeInBytes() - matchSizeInBytes;
  }

  m_averageSize.UpdateSize(m_gameContainerSizeInBytes);

  m_gameContainerMutex.unlock();

  return result;
}

/**
 * AddInMatchReport:
 *
 **/
bool GameContainer::AddInMatchReport(ReportInfo reportData) {
  bool retValue = false;

  m_gameContainerMutex.lock();

  auto matchData_it = m_matchMap.find(reportData.MatchGuid);
  if (matchData_it != m_matchMap.end()) {
    retValue = matchData_it->second->AddInMatchReport(reportData);
  }

  m_gameContainerMutex.unlock();

  return retValue;
}

/**
 * AddChatMessage:
 *
 **/
bool GameContainer::AddChatMessage(std::string matchGuid,
                                   ChatMessageInfo chatData) {
  bool retValue = false;

  m_gameContainerMutex.lock();

  auto matchData_it = m_matchMap.find(matchGuid);
  if (matchData_it != m_matchMap.end()) {
    retValue = matchData_it->second->AddChatMessage(chatData);
  }

  m_gameContainerMutex.unlock();

  return retValue;
}

/**
 * PopNextGameToProcess:
 *
 * Returns the next game packet to be sent to Getgud, may return nullptr
 * meaning no packet is ready to be sent at the moment
 **/
GameData* GameContainer::PopNextGameToProcess() {
	GameData* gameData = nullptr;
	std::string gameGuid;
	unsigned gameDataSizeInBytes;
    bool sendingEmptyGameMarkedAsEnded = false;

	//m_gameContainerMutex.lock();

	// traverse through all the live games and find the first eligible game to be
	// processed and pop it.
	for (int index = 0; index < m_gameVector.size(); index++) {
		gameData = m_gameVector[index];
		gameGuid = gameData->GetGameGuid();

		gameDataSizeInBytes = gameData->GetGameSizeInBytes();
		if (gameData->IsGameEligibleForProcessing() == true) {
			// we found our earliest injected, eligible game
			// check if this game is bigger than max allowed size to process
			if (gameDataSizeInBytes > sdkConfig.packetMaxSizeInBytes) {
				//  create a copy of game data with max allowed size, the rest will
				//  remain for next time someone will try to pop a game
				gameData = gameData->SliceGame(sdkConfig.packetMaxSizeInBytes);

				// update the game container size after this slice
				m_gameContainerSizeInBytes -= gameData->GetGameSizeInBytes();
				break;
			}
            // check if this is a game with no current actions (they were probably sent already) but just now it was marked as ended
            // thus it contains an empty packet but still needs to be sent in order to signal to the server that it needs to be finalized
			else if (gameDataSizeInBytes == 0 && gameData->IsGameMarkedAsEnded() == true && gameData->DidSendGameMarkedAsEnded() == false) {
                sendingEmptyGameMarkedAsEnded = true;
			}
			else if (gameData->CanDeleteGame() == true) {
				// the delete method will take care of reducing the game size from the
				// container
				// we are cleaning game container on the way by removing
				// unneeded games
				DeleteGame(gameGuid, false);
				gameData = nullptr;
			}
			else if (gameDataSizeInBytes > 0 ||
				gameData->GetNumberOfGameReportsAndMessages() > 0) {
				// this is a normal sized game packet that is ready to be sent whole
				// Do the old switcheroo trick: pop it and insert an empty GameData
				// instance instead of it
				m_gameVector.erase(m_gameVector.begin() + index);
				index--;
				m_gameMap.erase(gameGuid);

				//  create the empty game data that will replace the one we are popping
				//  and insert it to all the data structures
				GameData* cloneGameData = gameData->Clone(false);
				m_gameVector.push_back(cloneGameData);

				std::pair<std::string, GameData*> gameGuidPair(gameGuid, cloneGameData);
				m_gameMap.insert(gameGuidPair);

				//  fill the match map (abomination) with the new match pointers of the
				//  new game (replace the old one that belong longed to the game we are
				//  popping)
				std::vector<std::string> gameMatchGuids;
				cloneGameData->GetGameMatchGuids(gameMatchGuids);
				for (auto& gameMatchGuid : gameMatchGuids) {
					auto matchData_it = m_matchMap.find(gameMatchGuid);

					// if this match that we are titrating over belong longs to the game
					// we popped, replace match pointers with the clone's pointers
					if (matchData_it != m_matchMap.end())
						m_matchMap[gameMatchGuid] =
						cloneGameData->GetGameMatch(gameMatchGuid);
				}

				// we just removed a game packet, update the size of the container
				// accordingly
				m_gameContainerSizeInBytes -= gameDataSizeInBytes;
				m_gameVector.shrink_to_fit();
				break;
			}
			else {
				// if gameContainerSizeInBytes = 0 we can't send it
				gameData = nullptr;
			}
		}
		else {
			gameData = nullptr;
		}
	}

	//m_gameContainerMutex.unlock();

	// make sure a none empty game was found elidible and return it (even an empty
	// game has to go through the logic above) note that an empty game (with no
	// actions) will NOT be sent for processing and will not have any record it
	// ever existed
	if ( (gameData == nullptr || gameData->GetGameSizeInBytes() == 0) && sendingEmptyGameMarkedAsEnded == false) {
		gameData = nullptr;
	}
	else logger.Log(LogType::DEBUG, std::string("Popping Game with guid: " + gameData->GetGameGuid()));
	return gameData;
}

/**
 * MarkEndGame:
 *
 **/
bool GameContainer::MarkEndGame(std::string gameGuid) {
  bool retValue = true;

  m_gameContainerMutex.lock();

  // find the game that needs to be marked, and mark it :)
  auto gameData_it = m_gameMap.find(gameGuid);
  if (gameData_it == m_gameMap.end()) {
    logger.Log(LogType::WARN,
               std::string("GameContainer::MarkEndGame->Failed to find a \
game with the guid: " + gameGuid));
    retValue = false;
  } else {
    gameData_it->second->MarkGameAsEnded();
  }

  m_gameContainerMutex.unlock();

  return retValue;
}

/**
 * GetSizeInBytes:
 *
 **/
unsigned int GameContainer::GetSizeInBytes() {
  return m_gameContainerSizeInBytes;
}

/**
 * GetAverageSizeInBytes:
 *
 **/
unsigned int GameContainer::GetAverageSizeInBytes() {
  return m_averageSize.filledAverageSize;
}

/**
 * DeleteGame:
 *
 **/
bool GameContainer::DeleteGame(std::string gameGuid, bool externalCall = true) {
  std::vector<MatchData*> emptyMatchPtrVector;
  return DeleteGame(gameGuid, externalCall, emptyMatchPtrVector);
}

/**
 * DeleteGame:
 *
 **/
bool GameContainer::DeleteGame(std::string gameGuid,
                               bool externalCall,
                               std::vector<MatchData*>& matchPtrVector) {
  bool retValue = true;

  // this method might be called from another method that is already locking
  // gameContainerMutex
  if (externalCall)
    m_gameContainerMutex.lock();

  // find the game that needs to be deleted
  auto gameData_it = m_gameMap.find(gameGuid);
  if (gameData_it == m_gameMap.end()) {
    logger.Log(LogType::WARN,
               std::string("GameContainer::DeleteGame->Failed to find and \
deleted a game with the guid: " +
                           gameGuid));
    retValue = false;
  } else {
    GameData* gameToDelete = gameData_it->second;
    // remove the GameData pointer that resides in all the different data
    // structures
    m_gameMap.erase(gameGuid);
    for (int index = 0; index < m_gameVector.size(); index++) {
      // locate the GameData that resides in the game vector that has the
      // same guis that was passed to us
      if (m_gameVector[index]->GetGameGuid() == gameGuid) {
        m_gameVector.erase(m_gameVector.begin() + index);
        index--;  // because we are reducing size of gameVector
        break;
      }
    }

    //  remove all the game's matches from the container's match map
    std::vector<std::string> matchGuidVector;
    gameToDelete->GetGameMatchGuids(matchGuidVector);
    for (int index = 0; index < matchGuidVector.size(); index++) {
      auto match_it = m_matchMap.find(matchGuidVector[index]);

      // also search for matches in matchPtrVector and clear pointers too
      for (int i = 0; i < matchPtrVector.size(); i++) {
        if (match_it->second == matchPtrVector[i]) {
          matchPtrVector.erase(matchPtrVector.begin() + i);
          i--;
        }
      }
      m_matchMap.erase(matchGuidVector[index]);
    }

    // update the game container's size (reducing the size of the game) and
    // delete the gameData we found
    m_gameContainerSizeInBytes -= gameToDelete->GetGameSizeInBytes();
    gameToDelete->Dispose();
    delete gameToDelete;
    logger.Log(LogType::DEBUG,
               std::string("Deleted the game with guid: " + gameGuid));
  }
  m_gameVector.shrink_to_fit();

  if (externalCall)
    m_gameContainerMutex.unlock();

  return retValue;
}

/**
 * GetMatchMap:
 *
 **/
std::unordered_map<std::string, MatchData*>& GameContainer::GetMatchMap() {
  return m_matchMap;
}

/**
 * Dispose:
 *
 **/
void GameContainer::Dispose() {
  m_gameContainerMutex.lock();

  m_gameMap.clear();
  m_matchMap.clear();
  for (int index = 0; index < m_gameVector.size(); index++) {
    m_gameVector[index]->Dispose();
    delete m_gameVector[index];
  }
  m_gameVector.clear();
  m_gameContainerMutex.unlock();
}

}  // namespace GetgudSDK