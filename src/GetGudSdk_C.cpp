#include "pch.h"
#include "../include/GetGudSdk.h"
#include "../include/GetGudSdk_C.h"

#ifdef __linux__
#include <cstring>
#endif

//C functions header
extern "C" {

/**
 * Init:
 *
 * Init Getgud SDK
 **/
void init() {
  GetGudSdk::Init();
}

/**
 * StartGame:
 *
 * Start new game
 * Returns size
 **/
int StartGame(StartGameInfo gameInfo, char* gameGuidOut) {
  std::string gameGuid;
  if (gameInfo.privateKeySize != 0) {
    gameGuid =
        GetGudSdk::StartGame(gameInfo.titleId,
                  std::string(gameInfo.privateKey, gameInfo.privateKeySize),
                  std::string(gameInfo.serverGuid, gameInfo.serverGuidSize),
                  std::string(gameInfo.gameMode, gameInfo.gameModeSize));
  } else {
    gameGuid = GetGudSdk::StartGame(
        std::string(gameInfo.serverGuid, gameInfo.serverGuidSize),
                  std::string(gameInfo.gameMode, gameInfo.gameModeSize));
  }
  //gameGuidOut = new char[gameGuid.size()];
  strcpy(gameGuidOut, gameGuid.c_str());
  int gameGuidSize = gameGuid.size();
  return gameGuidSize;
}

/**
 * Start match:
 *
 * Start a new match for an existing game
 **/
int StartMatch(StartMatchInfo matchInfo, char** matchGuidOut) {
  /*typedef struct StartMatchInfo {
    char* gameGuid;
    int guidSize;
    char* matchMode;
    int modeSize;
    char* mapName;
    int nameSize;
  };*/

  std::string matchGuid;

  matchGuid = GetGudSdk::StartMatch(
      std::string(matchInfo.gameGuid, matchInfo.gameGuidSize),
                 std::string(matchInfo.matchMode, matchInfo.matchModeSize),
                 std::string(matchInfo.mapName, matchInfo.mapNameSize));

  *matchGuidOut = new char[matchGuid.size()];
  strcmp(*matchGuidOut, matchGuid.c_str());

  return matchGuid.size();
}

/**
 * MarkEndGame:
 *
 * Mark started game as finished
 **/
int MarkEndGame(char* gameGuid, int guidSize) {
  return GetGudSdk::MarkEndGame(std::string(gameGuid, guidSize));
}

/**
 * SendInMatchReport:
 *
 * Send a report which belongs to specifc match which is now live
 **/
int SendInMatchReport(ReportInfo reportInfo) {
  /*typedef struct ReportInfo {
  char* matchGuid;
  int matchGuidSize;
  char* reporterName;
  int nameSize;
  int reporterType;
  int reporterSubType;
  char* suspectedPlayerGuid;
  int playerGuidSize;
  int tbType;
  int tbSubType;
  long long tbTimeEpoch;
  int suggestedToxicityScore;
  long long reportedTimeEpoch;
};*/

  return GetGudSdk::SendInMatchReport(GetGudSdk::ReportInfo{
      std::string(reportInfo.matchGuid, reportInfo.matchGuidSize),
      std::string(reportInfo.reporterName, reportInfo.reporterNameSize),
      reportInfo.reporterType, reportInfo.reporterSubType,
      std::string(reportInfo.suspectedPlayerGuid,
                  reportInfo.suspectedPlayerGuidSize),
      reportInfo.tbType, reportInfo.tbSubType, reportInfo.tbTimeEpoch,
      reportInfo.suggestedToxicityScore, reportInfo.reportedTimeEpoch});
}

/**
 * SendChatMessage:
 *
 *  Send a message which belongs to specifc match which is now live
 **/
int SendChatMessage(ChatMessageInfo messageInfo) {
  /*struct ChatMessageInfo {
  long long messageTimeEpoch = 0;//>=time.min, <=time.max
  std::string playerGuid;//36 + SQL
  std::string message;//SQL, size <=10.000
};
*/
  return GetGudSdk::SendChatMessage(
      std::string(messageInfo.matchGuid, messageInfo.matchGuidSize),
      GetGudSdk::ChatMessageInfo{
          messageInfo.messageTimeEpoch,
          std::string(messageInfo.playerGuid, messageInfo.playerGuidSize),
          std::string(messageInfo.message, messageInfo.messageSize)});
}

/**
 * SendAttackAction:
 *
 **/
int SendAttackAction(BaseActionData baseData,
                     char* weaponGuid,
                     int weaponGuidSize) {
  GetGudSdk::AttackActionData* attackAction = new GetGudSdk::AttackActionData(
      std::string(baseData.matchGuid, baseData.matchGuidSize),
      baseData.actionTimeEpoch,
      std::string(baseData.playerGuid, baseData.playerGuidSize),
      std::string(weaponGuid, weaponGuidSize));
  std::deque<GetGudSdk::BaseActionData*> actions = {attackAction};
  bool sendResult = GetGudSdk::SendActions(actions);
  delete attackAction;
  return sendResult;
}

/**
 * SendDamageAction:
 *
 **/
int SendDamageAction(BaseActionData baseData,
                      char* victimPlayerGuid,
                      int victimPlayerGuidSize,
                      float damageDone,
                      char* weaponGuid,
                      int weaponGuidSize) {
  GetGudSdk::DamageActionData* damageAction = new GetGudSdk::DamageActionData(
      std::string(baseData.matchGuid, baseData.matchGuidSize),
      baseData.actionTimeEpoch,
      std::string(baseData.playerGuid, baseData.playerGuidSize),
      std::string(victimPlayerGuid, victimPlayerGuidSize), 
      damageDone, 
      std::string(weaponGuid, weaponGuidSize));
  std::deque<GetGudSdk::BaseActionData*> actions = {damageAction};
  bool sendResult = GetGudSdk::SendActions(actions);
  delete damageAction;
  return sendResult;
}

/**
 * SendHealAction:
 *
 **/
int SendHealAction(BaseActionData baseData, float healthGained) {
  GetGudSdk::HealActionData* healAction = new GetGudSdk::HealActionData(
      std::string(baseData.matchGuid, baseData.matchGuidSize),
      baseData.actionTimeEpoch,
      std::string(baseData.playerGuid, baseData.playerGuidSize), healthGained);
  std::deque<GetGudSdk::BaseActionData*> actions = {healAction};
  bool sendResult = GetGudSdk::SendActions(actions);
  delete healAction;
  return sendResult;
}

/**
 * SendSpawnAction:
 *
 **/
int SendSpawnAction(BaseActionData baseData,
                     char* characterGuid,
                     int characterGuidSize,
                     int teamId,
                     float initialHealth,
                     PositionF position,
                     RotationF rotation) {
  GetGudSdk::SpawnActionData* spawnAction = new GetGudSdk::SpawnActionData(
      std::string(baseData.matchGuid, baseData.matchGuidSize),
      baseData.actionTimeEpoch,
      std::string(baseData.playerGuid, baseData.playerGuidSize),
      std::string(characterGuid, characterGuidSize), teamId, initialHealth,
      *(GetGudSdk::PositionF*)&position, *(GetGudSdk::RotationF*)&rotation);
  std::deque<GetGudSdk::BaseActionData*> actions = {spawnAction};
  bool sendResult = GetGudSdk::SendActions(actions);
  delete spawnAction;
  return sendResult;
}

/**
 * SendDeathAction:
 *
 **/
int SendDeathAction(BaseActionData baseData) {
  GetGudSdk::DeathActionData* deathAction = new GetGudSdk::DeathActionData(
      std::string(baseData.matchGuid, baseData.matchGuidSize),
      baseData.actionTimeEpoch,
      std::string(baseData.playerGuid, baseData.playerGuidSize));
  std::deque<GetGudSdk::BaseActionData*> actions = {deathAction};
  bool sendResult = GetGudSdk::SendActions(actions);
  delete deathAction;
  return sendResult;
}

/**
 * SendPositionAction:
 *
 **/
int SendPositionAction(BaseActionData baseData,
                        PositionF position,
                        RotationF rotation) {
  GetGudSdk::PositionActionData* positionAction =
      new GetGudSdk::PositionActionData(
      std::string(baseData.matchGuid, baseData.matchGuidSize),
      baseData.actionTimeEpoch,
      std::string(baseData.playerGuid, baseData.playerGuidSize),
          *(GetGudSdk::PositionF*)&position, *(GetGudSdk::RotationF*)&rotation);
  std::deque<GetGudSdk::BaseActionData*> actions = {positionAction};
  bool sendResult = GetGudSdk::SendActions(actions);
  delete positionAction;
  return sendResult;
  ;
}

/**
 * Dispose:
 *
 **/
void dispose() {
  GetGudSdk::Dispose();
}
} //extern "C"