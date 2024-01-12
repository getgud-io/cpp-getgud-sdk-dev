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
                  std::string(gameInfo.gameMode, gameInfo.gameModeSize),
                  std::string(gameInfo.serverLocation, gameInfo.serverLocationSize));
  } else {
    gameGuid = GetGudSdk::StartGame(
                  std::string(gameInfo.serverGuid, gameInfo.serverGuidSize),
                  std::string(gameInfo.gameMode, gameInfo.gameModeSize),
                  std::string(gameInfo.serverLocation, gameInfo.serverLocationSize));
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
 * SendAffectAction:
 *
 **/
int SendAffectAction(struct BaseActionData baseData,
  char* affectGuid,
  int affectGuidSize,
  AffectState affectState)
{
  GetGudSdk::AffectActionData* affectAction = new GetGudSdk::AffectActionData(
    std::string(baseData.matchGuid, baseData.matchGuidSize),
    baseData.actionTimeEpoch,
    std::string(baseData.playerGuid, baseData.playerGuidSize),
    std::string(affectGuid, affectGuidSize),
    static_cast<GetGudSdk::AffectState>(affectState));
  std::deque<GetGudSdk::BaseActionData*> actions = { affectAction };
  bool sendResult = GetGudSdk::SendActions(actions);
  delete affectAction;
  return sendResult;
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
int SendDeathAction(BaseActionData baseData,
                    char* attackerGuid,
                    int attackerGuidSize) {
  GetGudSdk::DeathActionData* deathAction = new GetGudSdk::DeathActionData(
      std::string(baseData.matchGuid, baseData.matchGuidSize),
      baseData.actionTimeEpoch,
      std::string(baseData.playerGuid, baseData.playerGuidSize),
      std::string(attackerGuid, attackerGuidSize));
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
  //required
  reportInfoOut.MatchGuid = std::string(reportInfo.matchGuid, reportInfo.matchGuidSize);
  //required
  reportInfoOut.ReportedTimeEpoch = reportInfo.reportedTimeEpoch;
  if (reportInfo.reporterNameSize > 0)
    reportInfoOut.ReporterName = std::string(reportInfo.reporterName, reportInfo.reporterNameSize);
  if (reportInfo.reporterSubType != -1)
    reportInfoOut.ReporterSubType = static_cast<GetGudSdk::ReporterSubtype>(reportInfo.reporterSubType);
  if (reportInfo.reporterType != -1)
    reportInfoOut.ReporterType = static_cast<GetGudSdk::ReporterType>(reportInfo.reporterType);
  if (reportInfo.suggestedToxicityScore != -1)
    reportInfoOut.SuggestedToxicityScore = reportInfo.suggestedToxicityScore;
  //required
  reportInfoOut.SuspectedPlayerGuid = std::string(reportInfo.suspectedPlayerGuid, reportInfo.suspectedPlayerGuidSize);
  if (reportInfo.tbTimeEpoch != -1)
    reportInfoOut.TbTimeEpoch = reportInfo.tbTimeEpoch;
  if (reportInfo.tbType != -1)
    reportInfoOut.TbType = static_cast<GetGudSdk::TbType>(reportInfo.tbType);

  std::deque<GetGudSdk::ReportInfo> reports;
  reports.push_back(reportInfoOut);

  if (privateKeySize > 0)
  {
    return GetGudSdk::SendReports(titleId, std::string(privateKey, privateKeySize), reports);
  }
  else
  {
    return GetGudSdk::SendReports(reports);
  }
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
  if (player.playerEmailSize > 0)
    playerOut.PlayerEmail = std::string(player.playerEmail, player.playerEmailSize);
  //required
  playerOut.PlayerGuid = std::string(player.playerGuid, player.playerGuidSize);
  if (player.playerJoinDateEpoch != -1)
    playerOut.PlayerJoinDateEpoch = player.playerJoinDateEpoch;
  if (player.playerNicknameSize > 0)
    playerOut.PlayerNickname = std::string(player.playerNickname, player.playerNicknameSize);
  if (player.playerRank != -1)
    playerOut.PlayerRank = player.playerRank;
  if (player.playerSuspectScoreSize > 0)
    playerOut.PlayerSuspectScore = std::string(player.playerSuspectScore, player.playerSuspectScoreSize);
  if (player.playerReputationSize > 0)
      playerOut.PlayerReputation = std::string(player.playerReputation, player.playerReputationSize);
  if (player.playerStatusSize > 0)
      playerOut.PlayerStatus = std::string(player.playerStatus, player.playerStatusSize);
  if (player.PlayerCampaignSize > 0)
      playerOut.PlayerCampaign = std::string(player.PlayerCampaign, player.PlayerCampaignSize);
  if (player.playerNotesSize > 0)
      playerOut.PlayerNotes = std::string(player.playerNotes, player.playerNotesSize);
  if (player.playerDeviceSize > 0)
      playerOut.PlayerDevice = std::string(player.playerDevice, player.playerDeviceSize);
  if (player.playerOSSize > 0)
      playerOut.PlayerOS = std::string(player.playerOS, player.playerOSSize);
  if (player.playerAge != -1)
      playerOut.PlayerAge = player.playerAge;
  if (player.playerGenderSize > 0)
      playerOut.PlayerGender = std::string(player.playerGender, player.playerGenderSize);
  if (player.playerLocationSize > 0)
      playerOut.PlayerLocation = std::string(player.playerLocation, player.playerLocationSize);
  if (player.transactionsSize > 0)
  {
      std::vector<GetGudSdk::PlayerTransactions> transactions;
      for (int i = 0; i < player.transactionsSize; i++)
      {
          PlayerTransactions& transaction_ref = player.transactions[i];
          GetGudSdk::PlayerTransactions playerTransaction =
          {
              std::string(transaction_ref.TransactionGuid, transaction_ref.TransactionGuidSize),
              std::string(transaction_ref.TransactionName, transaction_ref.TransactionNameSize),
              transaction_ref.TransactionDateEpoch,
              transaction_ref.TransactionValueUSD
          };
          transactions.push_back(playerTransaction);
      }
      playerOut.Transactions = transactions;
  }

  std::deque<GetGudSdk::PlayerInfo> players;
  players.push_back(playerOut);
  if (privateKeySize > 0)
  {
    return UpdatePlayers(titleId, std::string(privateKey, privateKeySize), players);
  }
  else
  {
    return UpdatePlayers(players);
  }
}

/**
 * Dispose:
 *
 **/
void dispose() {
  GetGudSdk::Dispose();
}
} //extern "C"