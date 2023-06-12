#include "pch.h"
#include "MatchData.h"
#include <algorithm>
#include "../../include/actions/DamageActionData.h"
#include "../../include/actions/PositionActionData.h"
#include "../logger/Logger.h"
#include "../utils/Utils.h"
#include "./zip/Zip.h"
#include "../src/utils/Validator.h"
#include "../src/config/Config.h"

namespace GetGudSdk {
extern Config sdkConfig;
extern Zipper zipper;
extern Logger logger;

namespace {
int matchesAmount = 0;        // matches counter
int totalCreatedMatches = 0;  // total created matches;
}  // namespace

/**
 * MatchData:
 *
 **/
MatchData::MatchData(std::string gameGuid,
                     std::string matchMode,
                     std::string mapName)
    : m_gameGuid(gameGuid), m_matchMode(matchMode), m_mapName(mapName) {
  m_matchGuid = GenerateGuid();
  matchesAmount++;
  totalCreatedMatches++;
}

/**
 * MatchData:
 *
 **/
MatchData::MatchData(const MatchData& data)
    : m_matchGuid(data.m_matchGuid),
      m_gameGuid(data.m_gameGuid),
      m_matchMode(data.m_matchMode),
      m_mapName(data.m_mapName),
      m_actionVector(data.m_actionVector),
      m_reportVector(data.m_reportVector),
      m_chatMessageVector(data.m_chatMessageVector),
      m_throttleChecked(data.m_throttleChecked),
      m_isInteresting(data.m_isInteresting),
      m_playerGuids(data.m_playerGuids),
      m_actionsCount(data.m_actionsCount),
      m_sizeInBytes(data.m_sizeInBytes)
 {
  matchesAmount++;
  totalCreatedMatches++;
}

/**
 * ~MatchData:
 *
 **/
MatchData::~MatchData() {
  matchesAmount--;
}

/**
 * Clone:
 *
 * We need clone when we do Game and Match slicing so we can split match actions
 * to the original match and match copy that will be sent to Getgud
 **/
MatchData* MatchData::Clone(bool isWithActions) {
  MatchData* cloneMatchData = new MatchData(m_gameGuid, m_matchMode, m_mapName);
  // Clone some metadata variables
  cloneMatchData->m_matchGuid = m_matchGuid;
  cloneMatchData->m_gameGuid = m_gameGuid;
  cloneMatchData->m_matchMode = m_matchMode;
  cloneMatchData->m_mapName = m_mapName;
  cloneMatchData->m_isInteresting = m_isInteresting;
  cloneMatchData->m_throttleChecked = m_throttleChecked;

  // Clone actions, reports and chat if needed
  if (isWithActions == true) {
    cloneMatchData->AddActions(m_actionVector);

    cloneMatchData->m_reportVector.insert(cloneMatchData->m_reportVector.end(),
                                        m_reportVector.begin(),
                                        m_reportVector.end());
    cloneMatchData->m_chatMessageVector.insert(
        cloneMatchData->m_chatMessageVector.end(), m_chatMessageVector.begin(),
        m_chatMessageVector.end());

    // only if we are going to clone the data (AKA - actions) should we also
    // update the size of the object
    cloneMatchData->m_actionsCount = m_actionsCount;
    cloneMatchData->m_sizeInBytes = m_sizeInBytes;
  }

  if (isWithActions == true && cloneMatchData->m_reportVector.empty() &&
      cloneMatchData->m_chatMessageVector.empty() &&
      cloneMatchData->m_actionVector.empty()) {
    delete cloneMatchData;
    cloneMatchData = nullptr;
  }

  return cloneMatchData;
}

/**
 * AddActions:
 *
 * Add actions to the match, make sure dynamic time stamps of the actions are
 *linked to previous actions if we copy match
 **/
void MatchData::AddActions(std::vector<BaseActionData*>& incomingActionVector) {
  if (incomingActionVector.size() == 0)
    return;

  int newActionVectorSize = incomingActionVector.size();

  // add the newly arrived action vector to the current action vector, where all
  // the actions are waiting to be processed
  for (auto& actionPtr : incomingActionVector) {
    actionPtr->m_actionTimeEpoch -= m_lastActionTimeEpoch;
    m_lastActionTimeEpoch += actionPtr->m_actionTimeEpoch;

    // TODO: complete
    // get action player guid, get last position action for player guid
    // get delta between x,y,z,yaw,pitch, roll
    // multiply each number by 10^K, where K is max number of leading zeros after .
    // save into action
    // rewrite original action as last
    // only first position action for every player should be full!!
    // for each player, all other actions will be deltas
    // we do not have to change anything in Slice like with 
    // timestamps for example

    m_actionVector.push_back(actionPtr);
    // store unique player guids of the match
    m_playerGuids.insert(actionPtr->m_playerGuid);
  }
  m_actionVector.shrink_to_fit();
  // calculate the new size of this object (using deltas only)
  m_actionsCount += newActionVectorSize;
  m_sizeInBytes += GetPositionActionSize() * newActionVectorSize;

  logger.Log(LogType::DEBUG,
             std::string("Added " + std::to_string(newActionVectorSize) +
                         " action(s) to Match guid " + m_matchGuid));
}

/**
 * SliceMatch:
 *
 * Devide match into 2 matches, original match and it's copy taking ratio of
 * actions to the copy. This is used when match is too large for the packet
 **/
void MatchData::SliceMatch(float ratioToSlice,
                           std::vector<BaseActionData*>& toActionVector) {
  // calculate the number of actions we need to slice from this match
  // we take upper bound here so we will take at least 1 action always
  int actionsToTakeFromMatch = ceil(ratioToSlice * (float)m_actionVector.size());
  if (actionsToTakeFromMatch <= 0)
    return;


  // the the actions to the passed vector and erase them from this match's
  // vector
  toActionVector.insert(toActionVector.end(), m_actionVector.begin(),
                        m_actionVector.begin() + actionsToTakeFromMatch);
  m_actionVector.erase(m_actionVector.begin(),
                     m_actionVector.begin() + actionsToTakeFromMatch);
  m_actionVector.shrink_to_fit();

  // we need to convert time stamps of returned actions to the old format
  // because later we call add and the format will  be calculated
  // again
  long long lastActionTimeEpochToSlice = 0;
  for (auto& actionPtr : toActionVector) {
    actionPtr->m_actionTimeEpoch += lastActionTimeEpochToSlice;
    lastActionTimeEpochToSlice = actionPtr->m_actionTimeEpoch;
  }
  // adjust first remaining action for dynamic programming
  if (!m_actionVector.empty())
    m_actionVector.front()->m_actionTimeEpoch += lastActionTimeEpochToSlice;


  // calculate the new size of this match after the slice
  m_actionsCount -= actionsToTakeFromMatch;
  m_sizeInBytes -= GetPositionActionSize() * actionsToTakeFromMatch;
}


/**
 * SliceMatchReportVector:
 *
 * Devide match reports vector into 2 parts so we will not send too
 * many reports at once
 **/
void MatchData::SliceMatchReportVector(int numberToSlice,
                            std::vector<ReportData*>& toReportVector){
  if (numberToSlice <= 0)
    return;

  if (m_reportVector.size() < numberToSlice)
    return;

  // insert reports to the output vector and erase them from this match's
  // vector
  toReportVector.insert(toReportVector.end(), m_reportVector.begin(),
                        m_reportVector.begin() + numberToSlice);
  m_reportVector.erase(m_reportVector.begin(),
                       m_reportVector.begin() + numberToSlice);
  m_reportVector.shrink_to_fit();
}

/**
 * SliceMatchChatMessageVector:
 *
 * Devide chat message vector into 2 parts so we will not send too
 * many chat messages at once
 **/
void MatchData::SliceMatchChatMessageVector(int numberToSlice,
    std::vector<ChatMessageData*>& toChatMessageVector) {
  if (numberToSlice <= 0)
    return;

  if (m_chatMessageVector.size() < numberToSlice)
    return;

  // insert chat messages to the output vector and erase them from this match's
  // vector
  toChatMessageVector.insert(toChatMessageVector.end(), m_chatMessageVector.begin(),
                             m_chatMessageVector.begin() + numberToSlice);
  m_chatMessageVector.erase(m_chatMessageVector.begin(),
                            m_chatMessageVector.begin() + numberToSlice);
  m_chatMessageVector.shrink_to_fit();
}


/**
 * AddInMatchReport:
 *
 * Attach report to the match
 **/
bool MatchData::AddInMatchReport(ReportInfo reportInfo) {
  ReportData* reportData = new ReportData(reportInfo);
  if (!reportData->IsValid()) {
    delete reportData;
    return false;
  }
  // we do not have to pass titleId and privateKey here
  m_reportVector.push_back(reportData);
  logger.Log(LogType::DEBUG,
             std::string("Report added to Match with guid: " + m_matchGuid));
  return true;
}

/**
 * AddInMatchReport:
 *
 * Attach report to the match
 **/
bool MatchData::AddInMatchReport(ReportData* reportData) {
  if (!reportData->IsValid()) {
    delete reportData;
    return false;
  }
  // we do not have to pass titleId and privateKey here
  m_reportVector.push_back(reportData);
  logger.Log(LogType::DEBUG,
             std::string("Report added to Match with guid: " + m_matchGuid));
  return true;
}

/**
 * AddChatMessage:
 *
 * Add chat message to the match
 **/
bool MatchData::AddChatMessage(ChatMessageInfo chatInfo) {
  ChatMessageData* chatMessageData = new ChatMessageData(
      chatInfo.playerGuid, chatInfo.message, chatInfo.messageTimeEpoch);
  if (!chatMessageData->IsValid()) {
    delete chatMessageData;
    return false;
  }
  m_chatMessageVector.push_back(chatMessageData);
  logger.Log(
      LogType::DEBUG,
      std::string("Chat message added to Match with guid: " + m_matchGuid));
  return true;
}

/**
 * AddChatMessage:
 *
 * Add chat message to the match
 **/
bool MatchData::AddChatMessage(ChatMessageData* chatMessageData) {
  if (!chatMessageData->IsValid()) {
    delete chatMessageData;
    return false;
  }
  m_chatMessageVector.push_back(chatMessageData);
  logger.Log(
      LogType::DEBUG,
      std::string("Chat message added to Match with guid: " + m_matchGuid));
  return true;
}



/**
 * MatchToString:
 *
 * Convert this match to string for sending to Getgud. Will output empty string
 * if the match is not interesting
 **/
void MatchData::MatchToString(std::string& matchOut) {
  if (m_actionVector.size() + m_reportVector.size() + m_chatMessageVector.size() == 0)
    return;

  // if match is not interesting from throttle check we do not need to send it
  if (!m_isInteresting)
    return;

  std::string actionStream;
  std::string compressedActionStream;

  matchOut += "{";
  matchOut += "\"matchGuid\":\"" + m_matchGuid + "\",";
  matchOut += "\"mapName\":\"" + m_mapName + "\",";
  matchOut += "\"matchMode\":\"" + m_matchMode + "\",";
  matchOut += "\"matchActionStream\":\"";

  // TODO: complete
  // IF we do not know match K yet 
  // iterate over all actions of the match that are available
  // find max amount of leading zeros in deltas for x,y,z,pitch,yaw,roll
  // separately.
  // save it for this match
  // pass it to action.ToStringScaled() to multiply action values accordingly
  // To let us know the scale you will need to pass very first position action
  // twice!!! Unscaled first, and then scaled
  // m_actionVector[0]->ToString() + "," // scale_x, y,..yaw
  // m_actionVector[0]->ToString(scale_x,scale_y,scale_z, ...) + ","
  // by doing this we will  be able to reconstruct scale for each item on getgud
  // if you will not do that we will understand and assume the scale is real

  // run through all the actions and append them as strings - creating the
  // action stream
  for (int index = 0; index < m_actionVector.size(); index++) {
    actionStream += m_actionVector[index]->ToString() + ",";
  }
  if (actionStream.size() > 1) {
    actionStream.pop_back();  // pop the last delimiter
    zipper.CompressString(actionStream, compressedActionStream);
  }
  actionStream.clear();
  matchOut += compressedActionStream;
  matchOut += "\"";
  // convert all reports to json string
  if (m_reportVector.size() > 0)
  {
    matchOut += ",\"reports\":[";
    for (int index = 0; index < m_reportVector.size(); index++) {
      matchOut += m_reportVector[index]->ToString() + ",";
#ifdef _DEBUG
      sdkConfig.totalReportsSent++;
#endif
    }
    matchOut.pop_back();       // pop the last delimiter
    matchOut += "]";  // close the report array
  }

  if (m_chatMessageVector.size() > 0)
  {
    // convert all chat messages to json string
    matchOut += ",\"chat\":[";
    for (int index = 0; index < m_chatMessageVector.size(); index++) {
      matchOut += "" + m_chatMessageVector[index]->ToString() + ",";
#ifdef _DEBUG
      sdkConfig.totalChatSent++;
#endif
    }
    matchOut.pop_back();         // pop the last delimiter
    matchOut += "]";  // close the chat array and match
  }
  matchOut += "}";
}

/**
 * SetThrottleCheckResults:
 *
 * Update throttle check results for this match
 **/
void MatchData::SetThrottleCheckResults(bool inThrottleChecked,
                                        bool inIsInteresting) {
  m_throttleChecked = inThrottleChecked;
  m_isInteresting = inIsInteresting;

}

/**
 * GetThrottleChecked:
 *
 * Check if match was throttle checked before
 **/
bool MatchData::GetThrottleChecked() {
  return m_throttleChecked;
}

/**
 * GetMatchSizeInBytes:
 *
 **/
unsigned int MatchData::GetMatchSizeInBytes() {
  return m_sizeInBytes;
}

/**
 * GetPlayerGuids:
 *
 **/
std::set<std::string> MatchData::GetPlayerGuids() {
  return m_playerGuids;
}

/**
 * GetMatchMode:
 *
 **/
std::string MatchData::GetMatchMode() {
  return m_matchMode;
}

/**
 * GetMapName:
 *
 **/
std::string MatchData::GetMapName() {
  return m_mapName;
}

/**
 * GetNumberOfMatchReportsAndMessages:
 *
 **/
unsigned int MatchData::GetNumberOfMatchReportsAndMessages() {
  return m_reportVector.size() + m_chatMessageVector.size();
}

/**
 * GetMatchGuid:
 *
 **/
std::string MatchData::GetMatchGuid() {
  return m_matchGuid;
}

/**
 * GetGameGuid:
 *
 **/
std::string MatchData::GetGameGuid() {
  return m_gameGuid;
}

/**
 * ToStringMeta:
 *
 * ToString but for logging purposes
 **/
std::string MatchData::ToStringMeta() {
  return std::string("Match Guid: " + m_matchGuid +
                     " | Parent Game Guid: " + m_gameGuid +
                     " | Match Mode: " + m_matchMode + " | Map Name: " + m_mapName);
}

/**
 * Dispose:
 *
 **/
void MatchData::Dispose() {
  for (auto& action : m_actionVector)
    delete action;
  m_actionVector.clear();
  m_actionVector.shrink_to_fit();
  for (auto& report : m_reportVector)
    delete report;
  m_reportVector.clear();
  m_reportVector.shrink_to_fit();
  for (auto& chatMessage : m_chatMessageVector)
    delete chatMessage;
  m_chatMessageVector.clear();
  m_chatMessageVector.shrink_to_fit();
  m_playerGuids.clear();
}

bool MatchData::IsValid() {
  bool isActionValid = Validator::ValidateStringLength(m_gameGuid, 1, 36);
  isActionValid &= Validator::ValidateStringChars(m_gameGuid);
  isActionValid = Validator::ValidateStringLength(m_matchMode, 1, 100);
  isActionValid &= Validator::ValidateStringChars(m_matchMode);
  isActionValid = Validator::ValidateStringLength(m_matchMode, 1, 100);
  isActionValid &= Validator::ValidateStringChars(m_matchMode);
  return isActionValid;
}
}  // namespace GetGudSdk