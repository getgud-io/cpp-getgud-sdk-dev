#pragma once

#include <map>
#include <unordered_map>
#include "MatchData.h"

namespace GetgudSDK {
class GameData {
 private:
  //  holds pointers to matches of the game which you can get by matchGuid
  std::unordered_map<std::string, MatchData*> m_matchMap;
  // holds all guids of the matches
  std::vector<std::string> m_matchGuidVector;

  // the different meta data holders of the game
  int m_titleId = 0;
  std::string m_privateKey;
  std::string m_gameGuid;
  std::string m_serverGuid;
  std::string m_gameMode;
  std::string m_serverLocation;
  bool m_isGameMarkedAsEnded = false;
  unsigned int m_sizeInBytes = 0;
  std::chrono::system_clock::time_point m_startGameTimer;
  std::chrono::system_clock::time_point m_lastUpdateTime;

 public:
  GameData(int titleId,
           std::string privateKey,
           std::string serverGuid,
           std::string gameMode,
           std::string serverLocation);
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
  std::string GetServerLocation();
  void GetGameMatchGuids(std::vector<std::string>& matchGuidVectorOut);
  MatchData* GetGameMatch(std::string matchGuid);
  void GameToString(std::string& gameOut);
  std::string ToStringMeta();
  unsigned int GetGameSizeInBytes();
  unsigned int GetNumberOfGameReportsAndMessages();
  std::unordered_map<std::string, MatchData*>& GetMatchMap();
  void Dispose();
  bool IsValid();
};
}  // namespace GetgudSDK