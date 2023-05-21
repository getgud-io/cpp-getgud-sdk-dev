#include "GameContainer.h"
#include "../config/Config.h"
#include "../logger/Logger.h"

namespace GetGudSdk {

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
                                   std::string gameMode) {
  std::string gameGuid;
  // make sure the container has enough room for another game
  if (gameVector.size() >= sdkConfig.maxGames) {
    logger.Log(LogType::WARN,
               std::string("GameContainer::AddGame->Live game limit reached, "
                           "cannot add a new game"));
    return gameGuid;
  }

  // create a GameData and a pointer to it so we can reference the same GameData
  // object from different data structures
  GameData* gameData = new GameData(titleId, privateKey, serverGuid, gameMode);

  gameContainerMutex.lock();

  // insert the game to the game map (for quick access when searching for a game
  // via guid)
  auto gameGuidPair = std::make_pair(gameData->GetGameGuid(), gameData);
  gameMap.insert(gameGuidPair);

  // insert the game to the game vector, because this is a new game, its place
  // is at the last of the line (which is the tail of the vector)
  gameVector.push_back(gameData);
  gameGuid = gameData->GetGameGuid();
  std::string gameStringMeta = gameData->ToStringMeta();
  gameContainerMutex.unlock();

  logger.Log(LogType::DEBUG,
             std::string("A new game has been created.\n" + gameStringMeta));

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

  gameContainerMutex.lock();

  // TODO change this implementation using same logic
  //  find the game that this new match is asking to join to
  auto game_it = gameMap.find(gameGuid);
  if (game_it == gameMap.end()) {
    logger.Log(
        LogType::WARN,
        std::string(
            "GameContainer::AddMatch->Can't find game to push match into"));
    gameContainerMutex.unlock();
    return retValue;
  }
  // make sure SDK has enough room for another match
  if (matchMap.size() >= sdkConfig.maxMatchesPerGame) {
    logger.Log(LogType::WARN,
               std::string("GameContainer::AddMatch->Live matches limit "
                           "reached, cannot add a new match"));
    gameContainerMutex.unlock();
    return retValue;
  }
  // create a new match from the passed parameters and add it to the game we
  // found above
  matchData = game_it->second->AddMatch(matchMode, mapName);
  if (matchData == nullptr) {
    gameContainerMutex.unlock();
    return retValue;
  }

  // insert the match to the match map (for quick access when searching for
  // a match via guid, for example, when inserting actions)
  auto matchGuidPair = std::make_pair(matchData->GetMatchGuid(), matchData);
  matchMap.insert(matchGuidPair);

  // return the guid of the match, this acts as it's key to finding it
  retValue = matchData->GetMatchGuid();
  matchStringMeta = matchData->ToStringMeta();

  gameContainerMutex.unlock();

  if (matchData != nullptr) {
    logger.Log(LogType::DEBUG,
               std::string("A new match has been added.\n" + matchStringMeta));
  }

  return retValue;
}

/**
 * AddActions:
 *
 **/
bool GameContainer::AddActions(std::deque<BaseActionData*>& actionVector) {
  // TODO update the variables
  // TODO implement verifications of pointers and iterators
  //  first check that we are allowed to add more action to the game container
  //  and did not run out of allowed memory to consume
  gameContainerMutex.lock();
  bool result = true;

  // if game is full we add empty actions that will then trigger deletion of
  // game because they are invalid
  if (gameContainerSizeInBytes >= sdkConfig.gameContainerMaxSizeInBytes &&
      actionVector.size() > 0) {
    logger.Log(LogType::WARN,
               std::string("GameContainer::AddActions->Game container memory "
                           "limit reached reached, cannot add more actions."));
    std::deque<BaseActionData*> emptyActionVector;
    for (auto& action : actionVector) {
      auto* emptyAction = new BaseActionData(BaseData(), true);
      emptyAction->matchGuid = action->matchGuid;
      emptyActionVector.push_back(emptyAction);
      delete action;
    }
    actionVector.clear();  // TODO: Placed by Art, should it be here?
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
  int actionVectorSize = actionVector.size();
  BaseActionData* nextAction = nullptr;
  MatchData* matchData = nullptr;
  long long lastActionTime = 0;
  long long runningSubActionTimeEpoch = 0;

  // cluster all new actions according to their match guid and place them in a
  // dedicated vector per match guid
  for (int index = 0; index < actionVectorSize; index++) {
    // find the match that belongs to the action we are now iterating
    nextAction = actionVector[index];
    auto match_it = matchMap.find(nextAction->matchGuid);
    if (match_it == matchMap.end() || match_it->second == nullptr) {
      delete nextAction;
      continue;  // if a match with the passed guid was not found, just
                 // ignore the action
    }

    matchData = match_it->second;

    // validate the structure and parameters of the next action we are going to
    // assimilate
    if (nextAction->IsValid() == false) {
      // action is invalid and thus -> we are going to delete the entire game :/
      // why? Ask Art!
      DeleteGame(matchData->GetGameGuid(), false, matchPtrVector);
      logger.Log(
          LogType::WARN,
          std::string("GameContainer::AddActions->The following action is \
invalid OR is marked as empty from full Action Buffer \
or full game container. Deleting associated Game and \
matches:\n" + nextAction->ToStringMeta()));
      delete nextAction;
      continue;
    }

    // TODO changing real data of the pointer, should be a copy or something
    //  change the action time to work with dynamic programming
    nextAction->actionTimeEpoch -= lastActionTime;
    lastActionTime += nextAction->actionTimeEpoch;
    runningSubActionTimeEpoch += nextAction->actionTimeEpoch;
    // TODO matchActionsMap is empty and initialized inside the scope, update
    // this
    //  get the local vector (from the local map) that belong longs to this
    //  match and insert the action to it
    auto matchActions_it = matchActionsMap.find(matchData);
    if (matchActions_it == matchActionsMap.end()) {
      // the vector might not exists yet since this is the first time an action
      // with this match guid appeared in the method call
      // TODO: do we need new here? do we need to dispose of this below?

      // TODO: this was changed by Art, verify it works correctly
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

  // now run through all the matches that have new actions and insert the new
  // action vector to the match all at once while doing it, need to make sure
  // the game container size parameter is update accordingly
  for (int index = 0; index < matchPtrVector.size(); index++) {
    // TODO: here is the bug
    // TODO returns iterator, not a vector
    auto matchActions_it = matchActionsMap.find(matchPtrVector[index]);
    // save the size of the match before inserting the new actions
    // TODO GetMatchSizeInBytes returns wrong value
    matchSizeInBytes = matchPtrVector[index]->GetMatchSizeInBytes();
    matchPtrVector[index]->AddActions(matchActions_it->second,
                                      runningSubActionTimeEpoch);

    std::string matchGameGuid = matchPtrVector[index]->GetGameGuid();
    auto game_it = gameMap.find(matchGameGuid);
    if (game_it == gameMap.end() || game_it->second == nullptr)
      continue;
    game_it->second->UpdateLastUpdateTime();

    // update the game container size after adding the new messages to the match
    gameContainerSizeInBytes +=
        matchPtrVector[index]->GetMatchSizeInBytes() - matchSizeInBytes;
  }

  averageSize.UpdateSize(gameContainerSizeInBytes);

  // TODO: should we dispose of the passed vector and locally created maps and
  // vectors?

  gameContainerMutex.unlock();

  return result;
}

/**
 * AddInMatchReport:
 *
 **/
bool GameContainer::AddInMatchReport(ReportInfo reportData) {
  bool retValue = true;

  gameContainerMutex.lock();

  auto matchData_it = matchMap.find(reportData.MatchGuid);
  if (matchData_it == matchMap.end())
    retValue = false;

  else {
    // TODO: if we remove titleId and privateKey here we will not have
    // to do this extra search
    auto gameData_it = gameMap.find(matchData_it->second->GetGameGuid());
    if (gameData_it != gameMap.end())
      matchData_it->second->AddInMatchReport(
          gameData_it->second->GetTitleId(),
          gameData_it->second->GetPrivateKey(), reportData);
  }

  gameContainerMutex.unlock();

  return retValue;
}

/**
 * AddChatMessage:
 *
 **/
bool GameContainer::AddChatMessage(std::string matchGuid,
                                   ChatMessageInfo chatData) {
  bool retValue = true;

  gameContainerMutex.lock();

  auto matchData_it = matchMap.find(matchGuid);
  if (matchData_it == matchMap.end())
    retValue = false;

  else
    matchData_it->second->AddChatMessage(chatData);

  gameContainerMutex.unlock();

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
  std::string matchGuid;
  std::string gameGuid;
  unsigned gameDataSizeInBytes;

  gameContainerMutex.lock();

  // traverse through all the live games and find the first eligible game to be
  // processed and pop it.
  for (int index = 0; index < gameVector.size(); index++) {
    gameData = gameVector[index];
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
        gameContainerSizeInBytes -= gameData->GetGameSizeInBytes();
        break;
      } else if (gameData->CanDeleteGame() == true) {
        // the delete method will take care of reducing the game size from the
        // container
        // we are cleaning game container on the way by removing
        // unneeded games
        DeleteGame(gameGuid, false);  
        gameData = nullptr;
      } else if (gameDataSizeInBytes > 0 ||
                 gameData->GetNumberOfGameReportsAndMessages() > 0) {
        // this is a normal sized game packet that is ready to be sent whole
        // Do the old switcheroo trick: pop it and insert an empty GameData
        // instance instead of it
        std::vector<GameData*>::iterator it;
        it = gameVector.begin() + index;
        gameVector.erase(it);
        gameMap.erase(gameGuid); // TODO: do we really have to do this?

        // TODO implement verifications of pointers and iterators
        //  create the empty game data that will replace the one we are popping
        //  and insert it to all the data structures
        GameData* cloneGameData = gameData->Clone(false);
        gameVector.push_back(cloneGameData);

        std::pair<std::string, GameData*> gameGuidPair(gameGuid, cloneGameData);
        gameMap.insert(gameGuidPair);

        // TODO update this logic
        //  fill the match map (abomination) with the new match pointers of the
        //  new game (replace the old one that belong longed to the game we are
        //  popping)
        std::vector<std::string> gameMatchGuids;
        cloneGameData->GetGameMatchGuids(gameMatchGuids);
        for (int index = 0; index < gameMatchGuids.size(); index++) {
          matchGuid = gameMatchGuids[index];
          auto matchData_it = matchMap.find(matchGuid);

          // if this match that we are titrating over belong longs to the game
          // we popped, replace match pointers with the clone's pointers
          if (matchData_it != matchMap.end())
            matchMap[matchGuid] = cloneGameData->GetGameMatch(matchGuid);
        }

        // we just removed a game packet, update the size of the container
        // accordingly
        gameContainerSizeInBytes -= gameDataSizeInBytes;
        break;
      } else {
        // if gameContainerSizeInBytes = 0 we can't send it
        gameData = nullptr;
      }
    } else {
      gameData = nullptr;
    }
  }

  gameContainerMutex.unlock();

  // make sure a none empty game was found elidible and return it (even an empty
  // game has to go through the logic above) note that an empty game (with no
  // actions) will NOT be sent for processing and will not have any record it
  // ever existed
  if (gameData == nullptr || gameData->GetGameSizeInBytes() == 0) {
    gameData = nullptr;
    logger.Log(
        LogType::DEBUG,
        std::string(
            "GameContainer::PopNextGameToProcess called->No game to pop.\n"));
  } else
    logger.Log(LogType::DEBUG, std::string("Popping GameData with guid.\n" +
                                           gameData->GetGameGuid()));
  return gameData;
}

/**
 * MarkEndGame:
 *
 **/
bool GameContainer::MarkEndGame(std::string gameGuid) {
  bool retValue = true;

  gameContainerMutex.lock();

  // find the game that needs to be marked, and mark it :)
  auto gameData_it = gameMap.find(gameGuid);
  if (gameData_it == gameMap.end()) {
    logger.Log(LogType::WARN,
               std::string("GameContainer::MarkEndGame->Failed to find a \
game with the guid: " + gameGuid));
    retValue = false;
  } else {
    gameData_it->second->MarkGameAsEnded();
  }

  gameContainerMutex.unlock();

  return retValue;
}

/**
 * GetSizeInBytes:
 *
 **/
unsigned int GameContainer::GetSizeInBytes() {
  return gameContainerSizeInBytes;
}

/**
 * GetAverageSizeInBytes:
 *
 **/
unsigned int GameContainer::GetAverageSizeInBytes() {
  return averageSize.filledAverageSize;
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
    gameContainerMutex.lock();

  // find the game that needs to be deleted
  auto gameData_it = gameMap.find(gameGuid);
  if (gameData_it == gameMap.end()) {
    logger.Log(LogType::WARN,
               std::string("GameContainer::DeleteGame->Failed to find and \
deleted a game with the guid: " +
                           gameGuid));
    retValue = false;
  } else {
    GameData* gameToDelete = gameData_it->second;
    // remove the GameData pointer that resides in all the different data
    // structures
    gameMap.erase(gameGuid);
    for (int index = 0; index < gameVector.size(); index++) {
      // locate the GameData that resides in the game vector that has the
      // same guis that was passed to us
      if (gameVector[index]->GetGameGuid() == gameGuid) {
        gameVector.erase(gameVector.begin() + index);
        break;
        // TODO: do we have to call dispose here?
      }
    }

    // TODO update this
    //  remove all the game's matches from the container's match map
    std::vector<std::string> matchGuidVector;
    gameToDelete->GetGameMatchGuids(matchGuidVector);
    for (int index = 0; index < matchGuidVector.size(); index++) {
      auto match_it = matchMap.find(matchGuidVector[index]);
      int matchPtrSize = matchPtrVector.size();

      // also search for matches in matchPtrVector and clear pointers too
      for (int i = 0; i < matchPtrSize;) {
        if (match_it->second == matchPtrVector[i]) {
          matchPtrVector.erase(matchPtrVector.begin() + i);
          matchPtrSize--;
        } else
          i++;
      }
      matchMap.erase(matchGuidVector[index]);
    }

    // update the game container's size (reducing the size of the game) and
    // delete the gameData we found
    gameContainerSizeInBytes -= gameToDelete->GetGameSizeInBytes();
    gameToDelete->Dispose();
    delete gameToDelete;
    logger.Log(LogType::DEBUG,
               std::string("Deleted the game with the guid: " + gameGuid));
  }

  if (externalCall)
    gameContainerMutex.unlock();

  return retValue;
}

/**
 * GetMatchMap:
 *
 **/
std::unordered_map<std::string, MatchData*>& GameContainer::GetMatchMap() {
  return matchMap;
}

/**
 * Dispose:
 *
 **/
void GameContainer::Dispose() {
  // TODO: check logic here
  gameContainerMutex.lock();
  if (disposeRequired == false) {
    gameContainerMutex.unlock();
    return;
  }

  gameMap.clear();
  matchMap.clear();
  for (int index = 0; index < gameVector.size(); index++) {
    gameVector[index]->Dispose();
  }
  gameVector.clear();

  disposeRequired = false;
  gameContainerMutex.unlock();
}

}  // namespace GetGudSdk