#pragma once

#include <deque>
#include <mutex>
#include <set>
#include <vector>
#include "../../include/actions/BaseActionData.h"
#include "ChatMessageData.h"
#include "ReportData.h"

namespace GetGudSdk {
class MatchData {
 private:
  // holds all the actions (in order) that arrived from the client and are
  // waiting to be processed
  std::vector<BaseActionData*> actionVector;

  // holds all the in match reports that are waiting to be sent
  std::vector<ReportData*> reportVector;

  // holds all the chat massages that are waiting to be send
  std::vector<ChatMessageData*> chatMessageVector;

  std::string matchGuid;
  std::string gameGuid;
  std::string matchMode;
  std::string mapName;
  bool throttleChecked = false;
  bool isInteresting = false;
  std::set<std::string> playerGuids;
  unsigned int actionsCount = 0;
  unsigned int sizeInBytes = 0;
  long long runningSubActionTimeEpoch = 0;

 public:
  MatchData(std::string gameGuid, std::string matchMode, std::string mapName);
  MatchData(const MatchData& data);
  MatchData() = delete;
  ~MatchData();
  MatchData* Clone(bool isWithActions);
  void AddActions(std::vector<BaseActionData*>& actionVector,
                  long long newRunningSubActionTimeEpoch);
  void SliceMatch(float ratioToSlice,
                  std::vector<BaseActionData*>& toActionVector);
  void AddInMatchReport(int titleId,
                        std::string privateKey,
                        ReportInfo reportInfo);
  void AddChatMessage(ChatMessageInfo chatInfo);
  void MatchToString(std::string& match_out);
  unsigned int GetMatchSizeInBytes();
  unsigned int GetNumberOfMatchReportsAndMessages();
  std::string GetMatchGuid();
  std::string GetGameGuid();
  std::string ToStringMeta();
  void SetThrottleCheckResults(bool inThrottleChecked, bool inIsInteresting);
  bool GetThrottleChecked();
  std::set<std::string> GetPlayerGuids();
  std::string GetMatchMode();
  std::string GetMapName();
  void Dispose();
};
}  // namespace GetGudSdk