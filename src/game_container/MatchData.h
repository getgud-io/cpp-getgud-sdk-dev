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
  std::vector<BaseActionData*> m_actionVector;

  // holds all the in match reports that are waiting to be sent
  std::vector<ReportData*> m_reportVector;

  // holds all the chat massages that are waiting to be send
  std::vector<ChatMessageData*> m_chatMessageVector;

  std::string m_matchGuid;
  std::string m_gameGuid;
  std::string m_matchMode;
  std::string m_mapName;
  bool m_throttleChecked = false;
  bool m_isInteresting = false;
  std::set<std::string> m_playerGuids; //TODO make map and add m_lastOrientation
  unsigned int m_actionsCount = 0;
  unsigned int m_sizeInBytes = 0;
  long long m_lastActionTimeEpoch = 0;

  std::map<std::string, Orientation> m_lastPositionActionVector;
  // TODO: complete
  // We need a struct here to store last x,y,z,pitch,yaw, roll
  // for every player, initially they are initialized to 0

 public:
  MatchData(std::string gameGuid, std::string matchMode, std::string mapName);
  MatchData(const MatchData& data);
  MatchData() = delete;
  ~MatchData();
  MatchData* Clone(bool isWithActions);
  void AddActions(std::vector<BaseActionData*>& actionVector);
  void SliceMatch(float ratioToSlice,
                  std::vector<BaseActionData*>& toActionVector);
  void SliceMatchReportVector(int numberToSlice,
                              std::vector<ReportData*>& toReportVector);
  void SliceMatchChatMessageVector(int numberToSlice,
                                   std::vector<ChatMessageData*>& toChatMessageVector);
  bool AddInMatchReport(ReportInfo reportInfo);
  bool AddChatMessage(ChatMessageInfo chatInfo);
  bool AddInMatchReport(ReportData* reportData);
  bool AddChatMessage(ChatMessageData* chatMessageData);
  std::map<std::string, Orientation> CorrectMatchActionsDeltas();
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
  void SetLastPlayersPosition(std::map<std::string, Orientation> lastPositionVector);
  void Dispose();
  bool IsValid();
};
}  // namespace GetGudSdk