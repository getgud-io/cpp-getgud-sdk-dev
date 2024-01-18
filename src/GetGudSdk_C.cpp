#include "pch.h"
#include "../include/GetGudSdk.h"
#include "../include/GetGudSdk_C.h"
#include "logger/Logger.h"
#ifdef __linux__
#include <cstring>
#endif

namespace GetGudSdk {
    extern GetGudSdk::Logger logger;
}

//C functions header
extern "C" {
/**
 * Init:
 *
 * Init Getgud SDK
 **/
int init() {
  bool result = false;
  try {
      result = GetGudSdk::Init();
  }catch (std::exception& _error) {
      GetGudSdk::logger.Log(GetGudSdk::LogType::FATAL,
          std::string("GetGudSdk::init() "
              "can not be sent: ") +
          std::string(_error.what()));
  }
  return result;
}

/**
 * StartGame:
 *
 * Start new game
 * Returns size
 **/
int StartGame(StartGameInfo gameInfo, char* gameGuidOut) {
  std::string gameGuid;
  try {
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
  } catch (std::exception& _error) {
      GetGudSdk::logger.Log(GetGudSdk::LogType::FATAL,
          std::string("GetGudSdk::StartGame-> "
              "can not be sent: ") +
          std::string(_error.what()));
  }
  return gameGuid.size();
}

/**
 * Start match:
 *
 * Start a new match for an existing game
 **/
int StartMatch(StartMatchInfo matchInfo, char* matchGuidOut) {

  std::string matchGuid;
  try {
  matchGuid = GetGudSdk::StartMatch(
      std::string(matchInfo.gameGuid, matchInfo.gameGuidSize),
                 std::string(matchInfo.matchMode, matchInfo.matchModeSize),
                 std::string(matchInfo.mapName, matchInfo.mapNameSize));

  strcpy(matchGuidOut, matchGuid.c_str());
  } catch (std::exception& _error) {
      GetGudSdk::logger.Log(GetGudSdk::LogType::FATAL,
          std::string("GetGudSdk::StartMatch "
              "can not be sent: ") +
          std::string(_error.what()));
  }
  return matchGuid.size();
}

/**
 * MarkEndGame:
 *
 * Mark started game as finished
 **/
int MarkEndGame(char* gameGuid, int guidSize) {
  bool result = false;
  try {
    result = GetGudSdk::MarkEndGame(std::string(gameGuid, guidSize));
  }catch (std::exception& _error) {
      GetGudSdk::logger.Log(GetGudSdk::LogType::FATAL,
          std::string("GetGudSdk::MarkEndGame "
              "can not be sent: ") +
          std::string(_error.what()));
  }
  return result;
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
  bool sendResult = false;
  try {
    GetGudSdk::AffectActionData* affectAction = new GetGudSdk::AffectActionData(
      std::string(baseData.matchGuid, baseData.matchGuidSize),
      baseData.actionTimeEpoch,
      std::string(baseData.playerGuid, baseData.playerGuidSize),
      std::string(affectGuid, affectGuidSize),
      static_cast<GetGudSdk::AffectState>(affectState));
    std::deque<GetGudSdk::BaseActionData*> actions = { affectAction };
    sendResult = GetGudSdk::SendActions(actions);
    delete affectAction;
  } catch (std::exception& _error) {
      GetGudSdk::logger.Log(GetGudSdk::LogType::FATAL,
          std::string("GetGudSdk::SendAffectAction "
              "can not be sent: ") +
          std::string(_error.what()));
  }
  return sendResult;
}

/**
 * SendAttackAction:
 *
 **/
int SendAttackAction(BaseActionData baseData,
                     char* weaponGuid,
                     int weaponGuidSize) {
  bool sendResult = false;
  try {
    GetGudSdk::AttackActionData* attackAction = new GetGudSdk::AttackActionData(
        std::string(baseData.matchGuid, baseData.matchGuidSize),
        baseData.actionTimeEpoch,
        std::string(baseData.playerGuid, baseData.playerGuidSize),
        std::string(weaponGuid, weaponGuidSize));
    std::deque<GetGudSdk::BaseActionData*> actions = {attackAction};
    sendResult = GetGudSdk::SendActions(actions);
    delete attackAction;
  } catch (std::exception& _error) {
      GetGudSdk::logger.Log(GetGudSdk::LogType::FATAL,
          std::string("GetGudSdk::SendAttackAction "
              "can not be sent: ") +
          std::string(_error.what()));
  }
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
  bool sendResult = false;
  try {
    GetGudSdk::DamageActionData* damageAction = new GetGudSdk::DamageActionData(
        std::string(baseData.matchGuid, baseData.matchGuidSize),
        baseData.actionTimeEpoch,
        std::string(baseData.playerGuid, baseData.playerGuidSize),
        std::string(victimPlayerGuid, victimPlayerGuidSize), 
        damageDone, 
        std::string(weaponGuid, weaponGuidSize));
    std::deque<GetGudSdk::BaseActionData*> actions = {damageAction};
    sendResult = GetGudSdk::SendActions(actions);
    delete damageAction;
  } catch (std::exception& _error) {
      GetGudSdk::logger.Log(GetGudSdk::LogType::FATAL,
          std::string("GetGudSdk::SendDamageAction "
              "can not be sent: ") +
          std::string(_error.what()));
  }
  return sendResult;
}

/**
 * SendHealAction:
 *
 **/
int SendHealAction(BaseActionData baseData, float healthGained) {
  bool sendResult = false;
  try {
  GetGudSdk::HealActionData* healAction = new GetGudSdk::HealActionData(
      std::string(baseData.matchGuid, baseData.matchGuidSize),
      baseData.actionTimeEpoch,
      std::string(baseData.playerGuid, baseData.playerGuidSize), healthGained);
  std::deque<GetGudSdk::BaseActionData*> actions = {healAction};
  sendResult = GetGudSdk::SendActions(actions);
  delete healAction;
  } catch (std::exception& _error) {
      GetGudSdk::logger.Log(GetGudSdk::LogType::FATAL,
          std::string("GetGudSdk::SendDamageAction "
              "can not be sent: ") +
          std::string(_error.what()));
  }
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
  bool sendResult = false;
  try {
    GetGudSdk::SpawnActionData* spawnAction = new GetGudSdk::SpawnActionData(
        std::string(baseData.matchGuid, baseData.matchGuidSize),
        baseData.actionTimeEpoch,
        std::string(baseData.playerGuid, baseData.playerGuidSize),
        std::string(characterGuid, characterGuidSize), teamId, initialHealth,
        *(GetGudSdk::PositionF*)&position, *(GetGudSdk::RotationF*)&rotation);
    std::deque<GetGudSdk::BaseActionData*> actions = {spawnAction};
    sendResult = GetGudSdk::SendActions(actions);
    delete spawnAction;
  } catch (std::exception& _error) {
      GetGudSdk::logger.Log(GetGudSdk::LogType::FATAL,
          std::string("GetGudSdk::SendDamageAction "
              "can not be sent: ") +
          std::string(_error.what()));
  }
  return sendResult;
}

/**
 * SendDeathAction:
 *
 **/
int SendDeathAction(BaseActionData baseData,
                    char* attackerGuid,
                    int attackerGuidSize) {
  bool sendResult = false;
  try {
  GetGudSdk::DeathActionData* deathAction = new GetGudSdk::DeathActionData(
      std::string(baseData.matchGuid, baseData.matchGuidSize),
      baseData.actionTimeEpoch,
      std::string(baseData.playerGuid, baseData.playerGuidSize),
      std::string(attackerGuid, attackerGuidSize));
  std::deque<GetGudSdk::BaseActionData*> actions = {deathAction};
  sendResult = GetGudSdk::SendActions(actions);
  delete deathAction;
  } catch (std::exception& _error) {
      GetGudSdk::logger.Log(GetGudSdk::LogType::FATAL,
          std::string("GetGudSdk::SendDeathAction "
              "can not be sent: ") +
          std::string(_error.what()));
  }
  return sendResult;
}

/**
 * SendPositionAction:
 *
 **/
int SendPositionAction(BaseActionData baseData,
                        PositionF position,
                        RotationF rotation) {
  bool sendResult = false;
  try {
  GetGudSdk::PositionActionData* positionAction =
      new GetGudSdk::PositionActionData(
      std::string(baseData.matchGuid, baseData.matchGuidSize),
      baseData.actionTimeEpoch,
      std::string(baseData.playerGuid, baseData.playerGuidSize),
          *(GetGudSdk::PositionF*)&position, *(GetGudSdk::RotationF*)&rotation);
  std::deque<GetGudSdk::BaseActionData*> actions = {positionAction};
  sendResult = GetGudSdk::SendActions(actions);
  delete positionAction;
  } catch (std::exception& _error) {
      GetGudSdk::logger.Log(GetGudSdk::LogType::FATAL,
          std::string("GetGudSdk::SendDeathAction "
              "can not be sent: ") +
          std::string(_error.what()));
  }
  return sendResult;
}

/**
 * SendInMatchReport:
 *
 * Send a report which belongs to specifc match which is now live
 **/
int SendInMatchReport(ReportInfo reportInfo)
{
  bool sendResult = false;
  try {
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

  sendResult = GetGudSdk::SendInMatchReport(reportInfoOut);
  } catch (std::exception& _error) {
      GetGudSdk::logger.Log(GetGudSdk::LogType::FATAL,
          std::string("GetGudSdk::SendDeathAction "
              "can not be sent: ") +
          std::string(_error.what()));
  }
  return sendResult;
}

/**
 * SendChatMessage:
 *
 *  Send a message which belongs to specifc match which is now live
 **/
int SendChatMessage(ChatMessageInfo messageInfo)
{
  bool sendResult = false;
  try {
  GetGudSdk::ChatMessageInfo messageInfoOut;

  messageInfoOut.message = std::string(messageInfo.message, messageInfo.messageSize);
  messageInfoOut.messageTimeEpoch = messageInfo.messageTimeEpoch;
  messageInfoOut.playerGuid = std::string(messageInfo.playerGuid, messageInfo.playerGuidSize);
  
  sendResult = GetGudSdk::SendChatMessage(std::string(messageInfo.matchGuid, messageInfo.matchGuidSize), messageInfoOut);
  } catch (std::exception& _error) {
      GetGudSdk::logger.Log(GetGudSdk::LogType::FATAL,
          std::string("GetGudSdk::SendDeathAction "
              "can not be sent: ") +
          std::string(_error.what()));
  }

  return sendResult;
}

/**
 * SendReport:
 *
 * Send report which are outside of the live match
 **/
int SendReport(int titleId,
  char* privateKey, int privateKeySize, ReportInfo reportInfo)
{
  bool sendResult = false;
  try {
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
      sendResult = GetGudSdk::SendReports(titleId, std::string(privateKey, privateKeySize), reports);
    }
    else
    {
      sendResult = GetGudSdk::SendReports(reports);
    }
  } catch (std::exception& _error) {
      GetGudSdk::logger.Log(GetGudSdk::LogType::FATAL,
          std::string("GetGudSdk::SendDeathAction "
              "can not be sent: ") +
          std::string(_error.what()));
  }

  return sendResult;
}

/**
 * UpdatePlayer:
 *
 * Update player info outside of the live match
 **/
int UpdatePlayer(int titleId,
  char* privateKey, int privateKeySize, PlayerInfo player)
{
  bool sendResult = false;
  try {
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
      sendResult = UpdatePlayers(titleId, std::string(privateKey, privateKeySize), players);
    }
    else
    {
      sendResult = UpdatePlayers(players);
    }
  } catch (std::exception& _error) {
      GetGudSdk::logger.Log(GetGudSdk::LogType::FATAL,
          std::string("GetGudSdk::UpdatePlayer "
              "can not be sent: ") +
          std::string(_error.what()));
  }
  return sendResult;
}

/**
 * Dispose:
 *
 **/
void dispose() {
  try {
  GetGudSdk::Dispose();
  } catch (std::exception& _error) {
      GetGudSdk::logger.Log(GetGudSdk::LogType::FATAL,
          std::string("GetGudSdk::Dispose "
              "can not be called: ") +
          std::string(_error.what()));
  }
}
} //extern "C"