#pragma once

#include <map>
#include <unordered_map>
#include "MatchData.h"

namespace GetGudSdk {
class GameData {
 private:
  //  holds pointers to matches of the game which you can get by matchGuid
  std::unordered_map<std::string, MatchData*> matchMap;
  // holds all guids of the matches
  std::vector<std::string> matchGuidVector;

  // the different meta data holders of the game
  int titleId = 0;
  std::string privateKey;
  std::string gameGuid;
  std::string serverGuid;
  std::string gameMode;
  bool isGameMarkedAsEnded = false;
  unsigned int sizeInBytes = 0;
  std::chrono::system_clock::time_point startGameTimer;
  std::chrono::system_clock::time_point lastUpdateTime;

 public:
  GameData(int titleId,
           std::string privateKey,
           std::string serverGuid,
           std::string gameMode);
  GameData() = delete;
  GameData(const GameData& data);
  ~GameData();
  GameData* Clone(bool isWithActions);
  void MarkGameAsEnded();
  MatchData* AddMatch(std::string matchMode, std::string mapName);
  bool IsGameEligibleForProcessing();
  GameData* SliceGame(int sizeToSliceInBytes);
  bool CanDeleteGame();
  std::string GetGameGuid();
  void UpdateLastUpdateTime();
  int GetTitleId();
  std::string GetPrivateKey();
  std::string GetServerGuid();
  std::string GetGameMode();
  void GetGameMatchGuids(std::vector<std::string>& matchGuidVectorOut);
  MatchData* GetGameMatch(std::string match_guid);
  void GameToString(std::string& gameOut);
  std::string ToStringMeta();
  unsigned int GetGameSizeInBytes();
  unsigned int GetNumberOfGameReportsAndMessages();
  std::unordered_map<std::string, MatchData*>& GetMatchMap();
  void Dispose();
};
}  // namespace GetGudSdk