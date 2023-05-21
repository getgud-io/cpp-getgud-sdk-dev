#pragma once
#include <mutex>
#include <unordered_map>
#include <vector>
#include "../utils/Utils.h"
#include "GameData.h"

namespace GetGudSdk {
class GameContainer {
 private:
  // fast access map for when we need the GameData via game guid
  std::unordered_map<std::string, GameData*> gameMap;

  // fast access map for when we need the MatchData via match guid - this is
  // abusing game encapsulation but needed for speed
  std::unordered_map<std::string, MatchData*> matchMap;

  // holds the GameData and the order of the games as they need to be processed
  // (first game to be processed resides at the beginning of the vector)
  std::vector<GameData*> gameVector;

  // helpers and data holders
  unsigned int gameContainerSizeInBytes = 0;
  bool disposeRequired = true;
  std::mutex gameContainerMutex;
  FilledAverage averageSize;

 public:
  std::string AddGame(int titleId,
                      std::string privateKey,
                      std::string serverGuid,
                      std::string gameMode);
  std::string AddMatch(std::string gameGuid,
                       std::string matchMode,
                       std::string mapName);
  bool AddActions(std::deque<BaseActionData*>& actionVector);
  bool AddInMatchReport(ReportInfo reportData);
  bool AddChatMessage(std::string matchGuid, ChatMessageInfo chatData);
  unsigned int GetSizeInBytes();
  unsigned int GetAverageSizeInBytes();
  GameData* PopNextGameToProcess();
  bool MarkEndGame(std::string gameGuid);
  bool DeleteGame(std::string gameGuid, bool externalCall);
  void Dispose();
  std::unordered_map<std::string, MatchData*>& GetMatchMap();

 private:
  bool GameContainer::DeleteGame(std::string gameGuid,
                                 bool externalCall,
                                 std::vector<MatchData*>& matchPtrVector);
};
}  // namespace GetGudSdk