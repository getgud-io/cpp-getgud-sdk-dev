#include "GameData.h"
#include "../config/Config.h"
#include "../logger/Logger.h"
#include "../utils/Utils.h"
namespace GetGudSdk {
extern Config sdkConfig;
extern Logger logger;

namespace {
int gamesAmount = 0;
int totalCreatedGames = 0;
}  // namespace

/**
 * GameData:
 *
 **/
GameData::GameData(int titleId,
                   std::string privateKey,
                   std::string serverGuid,
                   std::string gameMode)
    : titleId(titleId),
      privateKey(privateKey),
      serverGuid(serverGuid),
      gameMode(gameMode) {
  gameGuid = GenerateGuid();
  startGameTimer = std::chrono::system_clock::now();
  lastUpdateTime = std::chrono::system_clock::now();
  gamesAmount++;
  totalCreatedGames++;
}

/**
 * GameData:
 *
 **/
GameData::GameData(const GameData& data)
    : matchMap(data.matchMap),
      matchGuidVector(data.matchGuidVector),
      titleId(data.titleId),
      privateKey(data.privateKey),
      gameGuid(data.gameGuid),
      serverGuid(data.serverGuid),
      gameMode(data.gameMode),
      isGameMarkedAsEnded(data.isGameMarkedAsEnded),
      sizeInBytes(data.sizeInBytes),
      startGameTimer(data.startGameTimer),
      lastUpdateTime(data.lastUpdateTime) {
  gamesAmount++;
  totalCreatedGames++;
}

/**
 * ~GameData:
 *
 **/
GameData::~GameData() {
  gamesAmount--;
}

/**
 * Clone:
 *
 * We need clone when we do Game and Match slicing so we can split match actions
 * to the original match and match copy that will be sent to Getgud
 **/
GameData* GameData::Clone(bool isWithActions) {
  // Clone some metadata variables
  GameData* cloneGameData =
      new GameData(titleId, privateKey, serverGuid, gameMode);
  cloneGameData->gameGuid = gameGuid;
  cloneGameData->isGameMarkedAsEnded = isGameMarkedAsEnded;
  cloneGameData->sizeInBytes = sizeInBytes;
  cloneGameData->startGameTimer = startGameTimer;
  cloneGameData->lastUpdateTime = lastUpdateTime;

  std::string matchGuid;

  // run through all the game's matches and clone them as well
  for (int index = 0; index < matchGuidVector.size(); index++) {
    matchGuid = matchGuidVector[index];
    auto matchData_it = matchMap.find(matchGuid);
    if (matchData_it == matchMap.end())
      continue;
    std::pair<std::string, MatchData*> matchGuidPair(
        matchGuid, matchData_it->second->Clone(isWithActions));
    cloneGameData->matchMap.insert(matchGuidPair);
    cloneGameData->matchGuidVector.push_back(matchGuid);
  }

  return cloneGameData;
}

/**
 * MarkGameAsEnded:
 *
 **/
void GameData::MarkGameAsEnded() {
  isGameMarkedAsEnded = true;
  logger.Log(LogType::DEBUG,
             std::string("Mark end game for game guid.\n" + gameGuid));
}

/**
 * AddMatch:
 *
 **/
MatchData* GameData::AddMatch(std::string matchMode, std::string mapName) {
  // make sure the game has enough room for another match
  if (matchGuidVector.size() > sdkConfig.maxMatchesPerGame) {
    logger.Log(
        LogType::WARN,
        std::string("GameData::AddMatch->Live matches per game limit reached, "
                    "cannot add a new match to game with guid: " +
                    gameGuid));
    return nullptr;
  }

  // create the new match with the passed parameters and insert it to the game's
  // match map
  MatchData* matchData = new MatchData(gameGuid, matchMode, mapName);
  std::pair<std::string, MatchData*> matchGuidPair(matchData->GetMatchGuid(),
                                                   matchData);
  matchMap.insert(matchGuidPair);
  matchGuidVector.push_back(matchData->GetMatchGuid());

  lastUpdateTime = std::chrono::system_clock::now();

  return matchData;
}

/**
 * IsGameEligibleForProcessing:
 *
 * Checks if game is eligible for processing for PopNextGameToProcess of
 * GameContainer purposes, it means that the game packet COULD be sent
 * to Getgud
 **/
bool GameData::IsGameEligibleForProcessing() {
  unsigned int gameSizeInBytes = GetGameSizeInBytes();

  // check if this game's size so far is big enough to be sent
  if (gameSizeInBytes > sdkConfig.minPacketSizeForSendingInBytes)
    return true;

  // check if the game is marked as ended AND enough time has passed since the
  // marking (to give time for actions to assimilate before closing)
  else if (isGameMarkedAsEnded == true &&
           lastUpdateTime +
                   std::chrono::milliseconds(
                       sdkConfig.gameCloseGraceAfterMarkEndInMilliseconds) <
               std::chrono::system_clock::now())
    return true;

  // check if this game's packet has been waiting for long long enough without
  // any data coming in to it
  else if (lastUpdateTime + std::chrono::milliseconds(
                                sdkConfig.packetTimeoutInMilliseconds) <
           std::chrono::system_clock::now())
    return true;

  // none of the conditions of eligibility are met, this game is not elidible
  // for processing (it's not big enough, it's not closed be the client and it's
  // not old enough)
  return false;
}

/**
 * SliceGame:
 *
 * Devide game into 2 games, original game and it's copy taking ratio of
 * actions inside the matches to the copy. This is used when game is too large
 * for sending the packet
 **/
GameData* GameData::SliceGame(int sizeToSliceInBytes) {
  // only clone the metadata of the game, without any actions
  GameData* cloneGameData = this->Clone(false);
  // if we are cloning we might have more packets and this is not the last one
  cloneGameData->isGameMarkedAsEnded = false;
  std::string matchGuid;

  // calculate the action percentage that we need to take from each match in
  // order to create a packet of the passed parameter size
  unsigned int gameSizeInBytes = GetGameSizeInBytes();
  float ratio = ((float)sizeToSliceInBytes / (float)gameSizeInBytes);

  // run through all the game's matches and fetch the calculated ratio of their
  // actions
  for (int index = 0; index < matchGuidVector.size(); index++) {
    matchGuid = matchGuidVector[index];
    auto matchData_it = matchMap.find(matchGuid);
    if (matchData_it == matchMap.end())
      continue;

    auto cloneMatchData_it = cloneGameData->matchMap.find(matchGuid);
    std::vector<BaseActionData*> slicedActions;

    // grabs the ratio of full actions of the match that we will write
    // into the match copy
    matchData_it->second->SliceMatch(ratio, slicedActions);

    long long slicedActionsTimeStamp = 0;
    // calculate running sum of slicedActions
    for (auto& action : slicedActions) {
      slicedActionsTimeStamp += action->actionTimeEpoch;
    }
    cloneMatchData_it->second->AddActions(slicedActions,
                                          slicedActionsTimeStamp);
  }

  return cloneGameData;
}

/**
 * CanDeleteGame:
 *
 * Check if we can delete the game because the client ended it, conditions:
 * 1. Game was marked as ended by the client
 * 2. Enough time has passed since the marking (to give time for actions to
 * assimilate before closing)
 * 3. Game is empty and has no action to process
 * OR there has been enough time when have passed since game received last
 * actions
 **/
bool GameData::CanDeleteGame() {
  unsigned int gameSizeInBytes = GetGameSizeInBytes();
  // Check if game is ended, has no actions and grace time to add remaining
  // actions has passed
  if (isGameMarkedAsEnded == true && gameSizeInBytes == 0 &&
      lastUpdateTime + std::chrono::milliseconds(
                           sdkConfig.gameCloseGraceAfterMarkEndInMilliseconds) <
          std::chrono::system_clock::now())
    return true;

  // Check if this game did not receive any actions for a very long long time,
  // indicating it's probably closed
  else if (lastUpdateTime + std::chrono::milliseconds(
                                sdkConfig.liveGameTimeoutInMilliseconds) <
           std::chrono::system_clock::now())
    return true;

  // game is not elidible for deletion - it did not meet the above conditions
  return false;
}

/**
 * GetGameGuid:
 *
 **/
std::string GameData::GetGameGuid() {
  return gameGuid;
}

/**
 * GetTitleId:
 *
 **/
int GameData::GetTitleId() {
  return titleId;
}

/**
 * GetPrivateKey:
 *
 **/
std::string GameData::GetPrivateKey() {
  return privateKey;
}

/**
 * UpdateLastUpdateTime:
 *
 **/
void GameData::UpdateLastUpdateTime() {
  lastUpdateTime = std::chrono::system_clock::now();
}

/**
 * GetGameMatchGuids:
 *
 **/
void GameData::GetGameMatchGuids(std::vector<std::string>& matchGuidVectorOut) {
  MatchData* matchData;
  std::string matchGuid;

  for (int index = 0; index < matchGuidVector.size(); index++) {
    matchGuid = matchGuidVector[index];
    matchGuidVectorOut.push_back(matchGuid);
  }
}

/**
 * GetGameMatch:
 *
 **/
MatchData* GameData::GetGameMatch(std::string matchGuid) {
  auto foundMatch_it = matchMap.find(matchGuid);
  MatchData* MatchOut = nullptr;
  if (foundMatch_it != matchMap.end())
    MatchOut = foundMatch_it->second;

  return MatchOut;
}

/**
 * GameToString:
 *
 **/
void GameData::GameToString(std::string& gameOut) {
  std::string lastPacket = "false";
  if (isGameMarkedAsEnded == true)
    lastPacket = "true";

  bool containsMatch = false;

  gameOut += "{ \n";
  gameOut += "	\"privateKey\": \"" + privateKey + "\",\n";
  gameOut += "	\"titleId\": " + std::to_string(titleId) + ",\n";
  gameOut += "	\"gameGuid\": \"" + gameGuid + "\",\n";
  gameOut += "	\"gameMode\": \"" + gameMode + "\",\n";
  gameOut += "	\"serverGuid\": \"" + serverGuid + "\",\n";
  gameOut += "	\"gameLastPacket\": " + lastPacket + ",\n";
  gameOut += "	\"matches\":\n	[\n";

  // run through all the game's matches, get their match_out string and append
  // them to the game
  for (int index = 0; index < matchGuidVector.size(); index++) {
    auto matchGuid = matchGuidVector[index];
    auto matchData_it = matchMap.find(matchGuid);
    if (matchData_it == matchMap.end() || matchData_it->second == nullptr)
      continue;

    std::string matchString;
    matchData_it->second->MatchToString(matchString);
    // in case MatchToString returned anything
    // if it is not isInteresting it will not return anything
    if (matchString.size() > 0) {
      gameOut += matchString;
      gameOut += ",";
      containsMatch = true;
    }
  }
  if (containsMatch) {
    gameOut.pop_back();  // pop the last delimiter

    gameOut += "\n	]\n";
    gameOut += "}";
  } else {
    gameOut.clear();
  }
}

/**
 * ToStringMeta:
 *
 **/
std::string GameData::ToStringMeta() {
  return "Game Guid: " + gameGuid + " | Server: " + serverGuid +
         " | Title id: " + std::to_string(titleId);
}

/**
 * GetGameSizeInBytes:
 *
 **/
unsigned int GameData::GetGameSizeInBytes() {
  unsigned int totalGameSize = 0;

  // traverse through all the game's matches, get their size and add it together
  // to get the total size of the current GameData object
  for (int index = 0; index < matchGuidVector.size(); index++) {
    auto matchGuid_it = matchGuidVector[index];
    auto matchData_it = matchMap.find(matchGuid_it);
    if (matchData_it != matchMap.end())
      totalGameSize += matchData_it->second->GetMatchSizeInBytes();
  }

  return totalGameSize;
}

/**
 * GetNumberOfGameReportsAndMessages:
 *
 **/
unsigned int GameData::GetNumberOfGameReportsAndMessages() {
  unsigned int numberOfReportAndMessages = 0;

  // traverse through all the game's matches, get their size and add it together
  // to get the total size of the current GameData object
  for (int index = 0; index < matchGuidVector.size(); index++) {
    auto matchGuid_it = matchGuidVector[index];
    auto matchData_it = matchMap.find(matchGuid_it);
    if (matchData_it == matchMap.end())
      continue;
    numberOfReportAndMessages +=
        matchData_it->second->GetNumberOfMatchReportsAndMessages();
  }

  return numberOfReportAndMessages;
}

/**
 * GetMatchMap:
 *
 **/
std::unordered_map<std::string, MatchData*>& GameData::GetMatchMap() {
  return matchMap;
}

/**
 * GetServerGuid:
 *
 **/
std::string GameData::GetServerGuid() {
  return serverGuid;
}

/**
 * GetGameMode:
 *
 **/
std::string GameData::GetGameMode() {
  return gameMode;
}

/**
 * Dispose:
 *
 **/
void GameData::Dispose() {
  for (int index = 0; index < matchGuidVector.size(); index++) {
    auto matchGuid = matchGuidVector[index];
    // TODO matchMap iteration should be better
    auto matchData_it = matchMap.find(matchGuid);
    if (matchData_it == matchMap.end() || matchData_it->second == nullptr)
      continue;
    matchData_it->second->Dispose();
    delete matchData_it->second;
  }

  matchMap.clear();
  matchGuidVector.clear();
}
}  // namespace GetGudSdk
