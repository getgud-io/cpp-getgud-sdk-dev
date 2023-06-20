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
int init() {
  return GetGudSdk::Init();
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
  strcpy(gameGuidOut, gameGuid.c_str());
  return gameGuid.size();
}

/**
 * Start match:
 *
 * Start a new match for an existing game
 **/
int StartMatch(StartMatchInfo matchInfo, char* matchGuidOut) {

  std::string matchGuid;

  matchGuid = GetGudSdk::StartMatch(
      std::string(matchInfo.gameGuid, matchInfo.gameGuidSize),
                 std::string(matchInfo.matchMode, matchInfo.matchModeSize),
                 std::string(matchInfo.mapName, matchInfo.mapNameSize));

  strcpy(matchGuidOut, matchGuid.c_str());

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
 * SendInMatchReport:
 *
 * Send a report which belongs to specifc match which is now live
 **/
int SendInMatchReport(ReportInfo reportInfo)
{
  GetGudSdk::ReportInfo reportInfoOut;

  reportInfoOut.MatchGuid = std::string(reportInfo.matchGuid, reportInfo.matchGuidSize);
  reportInfoOut.ReportedTimeEpoch = reportInfo.reportedTimeEpoch;
  reportInfoOut.ReporterName = std::string(reportInfo.reporterName, reportInfo.reporterNameSize);
  reportInfoOut.ReporterSubType = static_cast<GetGudSdk::ReporterSubtype>(reportInfo.reporterSubType);
  reportInfoOut.ReporterType = static_cast<GetGudSdk::ReporterType>(reportInfo.reporterType);
  reportInfoOut.SuggestedToxicityScore = reportInfo.suggestedToxicityScore;
  reportInfoOut.SuspectedPlayerGuid = std::string(reportInfo.suspectedPlayerGuid, reportInfo.suspectedPlayerGuidSize);
  reportInfoOut.TbTimeEpoch = reportInfo.tbTimeEpoch;
  reportInfoOut.TbType = static_cast<GetGudSdk::TbType>(reportInfo.tbType);

  return GetGudSdk::SendInMatchReport(reportInfoOut);
}

/**
 * SendChatMessage:
 *
 *  Send a message which belongs to specifc match which is now live
 **/
int SendChatMessage(ChatMessageInfo messageInfo)
{
  GetGudSdk::ChatMessageInfo messageInfoOut;

  messageInfoOut.message = std::string(messageInfo.message, messageInfo.messageSize);
  messageInfoOut.messageTimeEpoch = messageInfo.messageTimeEpoch;
  messageInfoOut.playerGuid = std::string(messageInfo.playerGuid, messageInfo.playerGuidSize);

  return GetGudSdk::SendChatMessage(std::string(messageInfo.matchGuid, messageInfo.matchGuidSize), messageInfoOut);

}

/**
 * SendReport:
 *
 * Send report which are outside of the live match
 **/
int SendReport(int titleId,
  char* privateKey, int privateKeySize, ReportInfo reportInfo)
{
  GetGudSdk::ReportInfo reportInfoOut;
  reportInfoOut.MatchGuid = std::string(reportInfo.matchGuid, reportInfo.matchGuidSize);
  reportInfoOut.ReportedTimeEpoch = reportInfo.reportedTimeEpoch;
  reportInfoOut.ReporterName = std::string(reportInfo.reporterName, reportInfo.reporterNameSize);
  reportInfoOut.ReporterSubType = static_cast<GetGudSdk::ReporterSubtype>(reportInfo.reporterSubType);
  reportInfoOut.ReporterType = static_cast<GetGudSdk::ReporterType>(reportInfo.reporterType);
  reportInfoOut.SuggestedToxicityScore = reportInfo.suggestedToxicityScore;
  reportInfoOut.SuspectedPlayerGuid = std::string(reportInfo.suspectedPlayerGuid, reportInfo.suspectedPlayerGuidSize);
  reportInfoOut.TbTimeEpoch = reportInfo.tbTimeEpoch;
  reportInfoOut.TbType = static_cast<GetGudSdk::TbType>(reportInfo.tbType);

  std::deque<GetGudSdk::ReportInfo> reports;
  reports.push_back(reportInfoOut);

  return GetGudSdk::SendReports(reports);
}

/**
 * UpdatePlayer:
 *
 * Update player info outside of the live match
 **/
int UpdatePlayer(int titleId,
  char* privateKey, int privateKeySize, PlayerInfo player)
{
  GetGudSdk::PlayerInfo playerOut;

  playerOut.PlayerEmail = std::string(player.playerEmail, player.playerEmailSize);
  playerOut.PlayerGuid = std::string(player.playerGuid, player.playerGuidSize);
  playerOut.PlayerJoinDateEpoch = player.playerJoinDateEpoch;
  playerOut.PlayerNickname = std::string(player.playerNickname, player.playerNicknameSize);
  playerOut.PlayerRank = player.playerRank;

  std::deque<GetGudSdk::PlayerInfo> players;
  players.push_back(playerOut);

  return UpdatePlayers(titleId, std::string(privateKey,privateKeySize), players);
}

/**
 * Dispose:
 *
 **/
void dispose() {
  GetGudSdk::Dispose();
}
} //extern "C"