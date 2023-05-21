#include "MatchData.h"
#include <algorithm>
#include "../../include/actions/DamageActionData.h"
#include "../../include/actions/PositionActionData.h"
#include "../logger/Logger.h"
#include "../utils/Utils.h"
#include "./zip/Zip.h"
// TODO: tmps
#include <fstream>
#include <iostream>

namespace GetGudSdk {
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
    : gameGuid(gameGuid), matchMode(matchMode), mapName(mapName) {
  matchGuid = GenerateGuid();
  matchesAmount++;
  totalCreatedMatches++;
}

/**
 * MatchData:
 *
 **/
MatchData::MatchData(const MatchData& data)
    : matchGuid(data.matchGuid),
      gameGuid(data.gameGuid),
      matchMode(data.matchMode),
      mapName(data.mapName),
      actionVector(data.actionVector),
      reportVector(data.reportVector),
      chatMessageVector(data.chatMessageVector),
      throttleChecked(data.throttleChecked),
      isInteresting(data.isInteresting),
      playerGuids(data.playerGuids),
      actionsCount(data.actionsCount),
      sizeInBytes(data.sizeInBytes),
      runningSubActionTimeEpoch(data.runningSubActionTimeEpoch) {
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
  MatchData* cloneMatchData = new MatchData(gameGuid, matchMode, mapName);
  // Clone some metadata variables
  cloneMatchData->matchGuid = matchGuid;
  cloneMatchData->gameGuid = gameGuid;
  cloneMatchData->matchMode = matchMode;
  cloneMatchData->mapName = mapName;
  cloneMatchData->isInteresting = isInteresting;
  cloneMatchData->throttleChecked = throttleChecked;

  // Clone actions, reports and chat if needed
  if (isWithActions == true) {
    cloneMatchData->AddActions(actionVector, runningSubActionTimeEpoch);

    cloneMatchData->reportVector.insert(cloneMatchData->reportVector.end(),
                                        reportVector.begin(),
                                        reportVector.end());
    cloneMatchData->chatMessageVector.insert(
        cloneMatchData->chatMessageVector.end(), chatMessageVector.begin(),
        chatMessageVector.end());

    // only if we are going to clone the data (AKA - actions) should we also
    // update the size of the object
    cloneMatchData->actionsCount = actionsCount;
    cloneMatchData->sizeInBytes = sizeInBytes;
  }

  return cloneMatchData;
}

/**
 * AddActions:
 *
 * Add actions to the match, make sure dynamic time stamps of the actions are
 *linked to previous actions if we copy match
 **/
void MatchData::AddActions(std::vector<BaseActionData*>& incomingActionVector,
                           long long newRunningSubActionTimeEpoch) {
  if (incomingActionVector.size() == 0)
    return;

  int newActionVectorSize = incomingActionVector.size();

  // change the first arriving action's time to work with dynamic programming
  // with the last action's time that already resides in the vector
  // TODO: validate that it actually works as expected
  BaseActionData* firstNewAction = incomingActionVector.back();
  firstNewAction->actionTimeEpoch -= runningSubActionTimeEpoch;
  runningSubActionTimeEpoch += newRunningSubActionTimeEpoch;

  // add the newly arrived action vector to the current action vector, where all
  // the actions are waiting to be processed
  for (auto& actionPtr : incomingActionVector) {
    actionVector.push_back(actionPtr);
    // store unique player guids of the match
    playerGuids.insert(actionPtr->playerGuid);
  }

  // calculate the new size of this object (using deltas only)
  actionsCount += newActionVectorSize;
  sizeInBytes += GetPositionActionSize() * newActionVectorSize;

  logger.Log(LogType::DEBUG,
             std::string("Actions added to match guid.\n" + matchGuid +
                         " # added: " + std::to_string(newActionVectorSize)));
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
  int actionsToTakeFromMatch = ceil(ratioToSlice * (float)actionVector.size());
  if (actionsToTakeFromMatch <= 0)
    return;
  long long newRunningSumActionTimeEpoch = 0;

  // the the actions to the passed vector and erase them from this match's
  // vector
  toActionVector.insert(toActionVector.end(), actionVector.begin(),
                        actionVector.begin() + actionsToTakeFromMatch);
  actionVector.erase(actionVector.begin(),
                     actionVector.begin() + actionsToTakeFromMatch);

  // calculate the new size of this match after the slice
  actionsCount -= actionsToTakeFromMatch;
  sizeInBytes -= GetPositionActionSize() * actionsToTakeFromMatch;

  // adjust the running sum after removing a bulk of accordingly
  for (int index = 0; index < actionsToTakeFromMatch; index++) {
    // calculate the running sum of the new vector we are going to send
    newRunningSumActionTimeEpoch += toActionVector[index]->actionTimeEpoch;
  }
  // adjust the match's running sum after removing actions
  runningSubActionTimeEpoch -= newRunningSumActionTimeEpoch;

  // TODO: make sure dynamic algo works here
  // give a real timestamp to the first action in the array we are going to send
  if (toActionVector.size() > 0)
    toActionVector[0]->actionTimeEpoch += runningSubActionTimeEpoch;
}

/**
 * AddInMatchReport:
 *
 * Attach report to the match
 **/
void MatchData::AddInMatchReport(int titleId,
                                 std::string privateKey,
                                 ReportInfo reportInfo) {
  // TODO: we should use ReportInfo not report data in the vector
  // we do not have to pass titleId and privateKey here
  reportVector.push_back(new ReportData(titleId, privateKey, reportInfo));
  logger.Log(LogType::DEBUG,
             std::string("Report added to match guid.\n" + matchGuid));
}

/**
 * AddChatMessage:
 *
 * Add chat message to the match
 **/
void MatchData::AddChatMessage(ChatMessageInfo chatInfo) {
  chatMessageVector.push_back(new ChatMessageData(
      chatInfo.playerGuid, chatInfo.message, chatInfo.messageTimeEpoch));
  logger.Log(LogType::DEBUG,
             std::string("Chat message added to match guid.\n" + matchGuid));
}

/**
 * MatchToString:
 *
 * Convert this match to string for sending to Getgud. Will output empty string
 * if the match is not interesting
 **/
void MatchData::MatchToString(std::string& matchOut) {
  if (actionVector.size() + reportVector.size() + chatMessageVector.size() == 0)
    return;

  // if match is not interesting from throttle check we do not need to send it
  if (!isInteresting)
    return;

  std::string actionStream;
  std::string compressedActionStream;

  matchOut += "\n	{";
  matchOut += "	\"matchGuid\": \"" + matchGuid + "\",\n";
  matchOut += "	\"mapName\": \"" + mapName + "\",\n";
  matchOut += "	\"matchMode\": \"" + matchMode + "\",\n";
  matchOut += "	\"matchActionStream\": \"";

  // run through all the actions and append them as strings - creating the
  // action stream
  for (int index = 0; index < actionVector.size(); index++) {
    actionStream += actionVector[index]->ToString() + ",";
  }
  if (actionStream.size() > 1) {
    actionStream.pop_back();  // pop the last delimiter
    zipper.CompressString(actionStream, compressedActionStream);
  }
  actionStream.clear();
  matchOut += compressedActionStream;
  // convert all reports to json string
  matchOut += "\",\n	\"reports\":\n	[\n";
  for (int index = 0; index < reportVector.size(); index++) {
    matchOut += "\n" + reportVector[index]->ToString() + ",";
  }
  matchOut.pop_back();       // pop the last delimiter
  matchOut += "\n	]";  // close the report array

  // convert all chat messages to json string
  matchOut += ",\n	\"chat\":\n	[\n";
  for (int index = 0; index < chatMessageVector.size(); index++) {
    matchOut += "\n" + chatMessageVector[index]->ToString() + ",";
  }
  matchOut.pop_back();         // pop the last delimiter
  matchOut += "\n	]\n";  // close the chat array
  matchOut += "\n	}";    // close the match
}

/**
 * SetThrottleCheckResults:
 *
 * Update throttle check results for this match
 **/
void MatchData::SetThrottleCheckResults(bool inThrottleChecked,
                                        bool inIsInteresting) {
  throttleChecked = inThrottleChecked;
  isInteresting = inIsInteresting;
}

/**
 * GetThrottleChecked:
 *
 * Check if match was throttle checked before
 **/
bool MatchData::GetThrottleChecked() {
  return throttleChecked;
}

/**
 * GetMatchSizeInBytes:
 *
 **/
unsigned int MatchData::GetMatchSizeInBytes() {
  return sizeInBytes;
}

/**
 * GetPlayerGuids:
 *
 **/
std::set<std::string> MatchData::GetPlayerGuids() {
  return playerGuids;
}

/**
 * GetMatchMode:
 *
 **/
std::string MatchData::GetMatchMode() {
  return matchMode;
}

/**
 * GetMapName:
 *
 **/
std::string MatchData::GetMapName() {
  return mapName;
}

/**
 * GetNumberOfMatchReportsAndMessages:
 *
 **/
unsigned int MatchData::GetNumberOfMatchReportsAndMessages() {
  return reportVector.size() + chatMessageVector.size();
}

/**
 * GetMatchGuid:
 *
 **/
std::string MatchData::GetMatchGuid() {
  return matchGuid;
}

/**
 * GetGameGuid:
 *
 **/
std::string MatchData::GetGameGuid() {
  return gameGuid;
}

/**
 * ToStringMeta:
 *
 * ToString but for logging purposes
 **/
std::string MatchData::ToStringMeta() {
  return std::string("Match Guid: " + matchGuid +
                     " | Parent Game Guid: " + gameGuid +
                     " | Match Mode: " + matchMode + " | Map Name: " + mapName);
}

/**
 * Dispose:
 *
 **/
void MatchData::Dispose() {
  for (auto& action : actionVector)
    delete action;
  actionVector.clear();
  for (auto& report : reportVector)
    delete report;
  reportVector.clear();
  for (auto& chatMessage : chatMessageVector)
    delete chatMessage;
  chatMessageVector.clear();
  playerGuids.clear();
}
}  // namespace GetGudSdk