#include "../include/GetgudSDK.h"
#include "../include/GetgudSDK_C.h"
#include "logger/Logger.h"
#ifdef __linux__
#include <cstring>
#endif

namespace GetgudSDK {
    extern GetgudSDK::Logger logger;
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
      result = GetgudSDK::Init();
  }catch (std::exception& _error) {
      GetgudSDK::logger.Log(GetgudSDK::LogType::FATAL,
          std::string("GetgudSDK::init() "
              "can not be sent: ") +
          std::string(_error.what()));
  }
  return result;
}

/**
 * Init_conf:
 *
 * Init Getgud SDK with provided config
 **/
int init_conf(const char* configPath, int isConfigContent) {
    bool result = false;
    try {
        result = GetgudSDK::Init(std::string(configPath), isConfigContent);
    }
    catch (std::exception& _error) {
        GetgudSDK::logger.Log(GetgudSDK::LogType::FATAL,
            std::string("GetgudSDK::init() "
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
    std::string privateKey;
    std::string serverGuid;
    std::string gameMode;
    std::string serverLocation;

    if (gameInfo.privateKey != NULL &&
        strlen(gameInfo.privateKey) == gameInfo.privateKeySize) 
    {
        privateKey = std::string(gameInfo.privateKey, gameInfo.privateKeySize);
    }

    if (gameInfo.serverGuid != NULL &&
        strlen(gameInfo.serverGuid) == gameInfo.serverGuidSize)
    {
        serverGuid = std::string(gameInfo.serverGuid, gameInfo.serverGuidSize);
    }

    if (gameInfo.gameMode != NULL &&
        strlen(gameInfo.gameMode) == gameInfo.gameModeSize)
    {
        gameMode = std::string(gameInfo.gameMode, gameInfo.gameModeSize);
    }

    if (gameInfo.serverLocation != NULL &&
        strlen(gameInfo.serverLocation) == gameInfo.serverLocationSize)
    {
        serverLocation = std::string(gameInfo.serverLocation, gameInfo.serverLocationSize);
    }

    if (gameInfo.privateKeySize != 0) {
      gameGuid =
          GetgudSDK::StartGame(gameInfo.titleId,
                    privateKey.c_str(),
                    serverGuid.c_str(),
                    gameMode.c_str(),
                    serverLocation.c_str());
    } else {
      gameGuid = GetgudSDK::StartGame(
                    serverGuid.c_str(),
                    gameMode.c_str(),
                    serverLocation.c_str());
    }
    strcpy(gameGuidOut, gameGuid.c_str());
  } catch (std::exception& _error) {
      GetgudSDK::logger.Log(GetgudSDK::LogType::FATAL,
          std::string("GetgudSDK::StartGame-> "
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
    std::string gameGuid;
    std::string matchMode;
    std::string mapName;

    if (matchInfo.gameGuid != NULL &&
        strlen(matchInfo.gameGuid) == matchInfo.gameGuidSize)
    {
        gameGuid = std::string(matchInfo.gameGuid, matchInfo.gameGuidSize);
    }

    if (matchInfo.matchMode != NULL &&
        strlen(matchInfo.matchMode) == matchInfo.matchModeSize)
    {
        matchMode = std::string(matchInfo.matchMode, matchInfo.matchModeSize);
    }

    if (matchInfo.mapName != NULL &&
        strlen(matchInfo.mapName) == matchInfo.mapNameSize)
    {
        mapName = std::string(matchInfo.mapName, matchInfo.mapNameSize);
    }

    matchGuid = GetgudSDK::StartMatch(
                   gameGuid.c_str(),
                   matchMode.c_str(),
                   mapName.c_str());

    strcpy(matchGuidOut, matchGuid.c_str());
  } catch (std::exception& _error) {
      GetgudSDK::logger.Log(GetgudSDK::LogType::FATAL,
          std::string("GetgudSDK::StartMatch "
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
int MarkEndGame(const char* gameGuid, int guidSize) {
  bool result = false;
  try {
    std::string endGameGuid;
    if (gameGuid != NULL && 
        strlen(gameGuid) == guidSize)
    {
        endGameGuid = std::string(gameGuid, guidSize);
    }

    result = GetgudSDK::MarkEndGame(endGameGuid.c_str());
  } catch (std::exception& _error) {
      GetgudSDK::logger.Log(GetgudSDK::LogType::FATAL,
          std::string("GetgudSDK::MarkEndGame "
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
  const char* affectGuid,
  int affectGuidSize,
  AffectState affectState)
{
  bool sendResult = false;
  try {
    std::string matchGuid;
    std::string playerGuid;
    std::string inAffectGuid;

    if (baseData.matchGuid != NULL &&
        strlen(baseData.matchGuid) == baseData.matchGuidSize)
    {
        matchGuid = std::string(baseData.matchGuid, baseData.matchGuidSize);
    }

    if (baseData.playerGuid != NULL &&
        strlen(baseData.playerGuid) == baseData.playerGuidSize)
    {
        playerGuid = std::string(baseData.playerGuid, baseData.playerGuidSize);
    }

    if (affectGuid != NULL &&
        strlen(affectGuid) == affectGuidSize)
    {
        inAffectGuid = std::string(affectGuid, affectGuidSize);
    }

    GetgudSDK::AffectActionData* affectAction = new GetgudSDK::AffectActionData(
      matchGuid.c_str(),
      baseData.actionTimeEpoch,
      playerGuid.c_str(),
      inAffectGuid.c_str(),
      static_cast<GetgudSDK::AffectState>(affectState));
    std::deque<GetgudSDK::BaseActionData*> actions = { affectAction };
    sendResult = GetgudSDK::SendActions(actions);
    delete affectAction;
  } catch (std::exception& _error) {
      GetgudSDK::logger.Log(GetgudSDK::LogType::FATAL,
          std::string("GetgudSDK::SendAffectAction "
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
                     const char* weaponGuid,
                     int weaponGuidSize) {
  bool sendResult = false;
  try {
    std::string matchGuid;
    std::string playerGuid;
    std::string inWeaponGuid;

    if (baseData.matchGuid != NULL &&
        strlen(baseData.matchGuid) == baseData.matchGuidSize)
    {
        matchGuid = std::string(baseData.matchGuid, baseData.matchGuidSize);
    }

    if (baseData.playerGuid != NULL &&
        strlen(baseData.playerGuid) == baseData.playerGuidSize)
    {
        playerGuid = std::string(baseData.playerGuid, baseData.playerGuidSize);
    }

    if (weaponGuid != NULL &&
        strlen(weaponGuid) == weaponGuidSize)
    {
        inWeaponGuid = std::string(weaponGuid, weaponGuidSize);
    }

    GetgudSDK::AttackActionData* attackAction = new GetgudSDK::AttackActionData(
        matchGuid.c_str(),
        baseData.actionTimeEpoch,
        playerGuid.c_str(),
        inWeaponGuid.c_str());
    std::deque<GetgudSDK::BaseActionData*> actions = {attackAction};
    sendResult = GetgudSDK::SendActions(actions);
    delete attackAction;
  } catch (std::exception& _error) {
      GetgudSDK::logger.Log(GetgudSDK::LogType::FATAL,
          std::string("GetgudSDK::SendAttackAction "
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
                      const char* victimPlayerGuid,
                      int victimPlayerGuidSize,
                      float damageDone,
                      const char* weaponGuid,
                      int weaponGuidSize) {
  bool sendResult = false;
  try {
    std::string matchGuid;
    std::string playerGuid;
    std::string inVictimPlayerGuid;
    std::string inWeaponGuid;

    if (baseData.matchGuid != NULL &&
        strlen(baseData.matchGuid) == baseData.matchGuidSize)
    {
        matchGuid = std::string(baseData.matchGuid, baseData.matchGuidSize);
    }

    if (baseData.playerGuid != NULL &&
        strlen(baseData.playerGuid) == baseData.playerGuidSize)
    {
        playerGuid = std::string(baseData.playerGuid, baseData.playerGuidSize);
    }

    if (victimPlayerGuid != NULL &&
        strlen(victimPlayerGuid) == victimPlayerGuidSize)
    {
        inVictimPlayerGuid = std::string(victimPlayerGuid, victimPlayerGuidSize);
    }

    if (weaponGuid != NULL &&
        strlen(weaponGuid) == weaponGuidSize)
    {
        inWeaponGuid = std::string(weaponGuid, weaponGuidSize);
    }

    GetgudSDK::DamageActionData* damageAction = new GetgudSDK::DamageActionData(
        matchGuid.c_str(),
        baseData.actionTimeEpoch,
        playerGuid.c_str(),
        inVictimPlayerGuid.c_str(),
        damageDone, 
        inWeaponGuid.c_str());
    std::deque<GetgudSDK::BaseActionData*> actions = {damageAction};
    sendResult = GetgudSDK::SendActions(actions);
    delete damageAction;
  } catch (std::exception& _error) {
      GetgudSDK::logger.Log(GetgudSDK::LogType::FATAL,
          std::string("GetgudSDK::SendDamageAction "
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
    std::string matchGuid;
    std::string playerGuid;

    if (baseData.matchGuid != NULL &&
        strlen(baseData.matchGuid) == baseData.matchGuidSize)
    {
        matchGuid = std::string(baseData.matchGuid, baseData.matchGuidSize);
    }

    if (baseData.playerGuid != NULL &&
        strlen(baseData.playerGuid) == baseData.playerGuidSize)
    {
        playerGuid = std::string(baseData.playerGuid, baseData.playerGuidSize);
    }

    GetgudSDK::HealActionData* healAction = new GetgudSDK::HealActionData(
        matchGuid.c_str(),
        baseData.actionTimeEpoch,
        playerGuid.c_str(),
        healthGained);
    std::deque<GetgudSDK::BaseActionData*> actions = {healAction};
    sendResult = GetgudSDK::SendActions(actions);
    delete healAction;
  } catch (std::exception& _error) {
      GetgudSDK::logger.Log(GetgudSDK::LogType::FATAL,
          std::string("GetgudSDK::SendDamageAction "
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
                     const char* characterGuid,
                     int characterGuidSize,
                     int teamId,
                     float initialHealth,
                     PositionF position,
                     RotationF rotation) {
  bool sendResult = false;
  try {
    std::string matchGuid;
    std::string playerGuid;
    std::string inCharacterGuid;

    if (baseData.matchGuid != NULL &&
        strlen(baseData.matchGuid) == baseData.matchGuidSize)
    {
        matchGuid = std::string(baseData.matchGuid, baseData.matchGuidSize);
    }

    if (baseData.playerGuid != NULL &&
        strlen(baseData.playerGuid) == baseData.playerGuidSize)
    {
        playerGuid = std::string(baseData.playerGuid, baseData.playerGuidSize);
    }

    if (characterGuid != NULL &&
        strlen(characterGuid) == characterGuidSize)
    {
        inCharacterGuid = std::string(characterGuid, characterGuidSize);
    }

    GetgudSDK::SpawnActionData* spawnAction = new GetgudSDK::SpawnActionData(
        matchGuid.c_str(),
        baseData.actionTimeEpoch,
        playerGuid.c_str(),
        inCharacterGuid.c_str(), teamId, initialHealth,
        *(GetgudSDK::PositionF*)&position, *(GetgudSDK::RotationF*)&rotation);
    std::deque<GetgudSDK::BaseActionData*> actions = {spawnAction};
    sendResult = GetgudSDK::SendActions(actions);
    delete spawnAction;
  } catch (std::exception& _error) {
      GetgudSDK::logger.Log(GetgudSDK::LogType::FATAL,
          std::string("GetgudSDK::SendDamageAction "
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
                    const char* attackerGuid,
                    int attackerGuidSize) {
  bool sendResult = false;
  try {
    std::string matchGuid;
    std::string playerGuid;
    std::string inAttackerGuid;

    if (baseData.matchGuid != NULL &&
        strlen(baseData.matchGuid) == baseData.matchGuidSize)
    {
        matchGuid = std::string(baseData.matchGuid, baseData.matchGuidSize);
    }

    if (baseData.playerGuid != NULL &&
        strlen(baseData.playerGuid) == baseData.playerGuidSize)
    {
        playerGuid = std::string(baseData.playerGuid, baseData.playerGuidSize);
    }

    if (attackerGuid != NULL &&
        strlen(attackerGuid) == attackerGuidSize)
    {
        inAttackerGuid = std::string(attackerGuid, attackerGuidSize);
    }

    GetgudSDK::DeathActionData* deathAction = new GetgudSDK::DeathActionData(
        matchGuid.c_str(),
        baseData.actionTimeEpoch,
        playerGuid.c_str(),
        inAttackerGuid.c_str());
    std::deque<GetgudSDK::BaseActionData*> actions = {deathAction};
    sendResult = GetgudSDK::SendActions(actions);
    delete deathAction;
  } catch (std::exception& _error) {
      GetgudSDK::logger.Log(GetgudSDK::LogType::FATAL,
          std::string("GetgudSDK::SendDeathAction "
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
    std::string matchGuid;
    std::string playerGuid;

    if (baseData.matchGuid != NULL &&
        strlen(baseData.matchGuid) == baseData.matchGuidSize)
    {
        matchGuid = std::string(baseData.matchGuid, baseData.matchGuidSize);
    }

    if (baseData.playerGuid != NULL &&
        strlen(baseData.playerGuid) == baseData.playerGuidSize)
    {
        playerGuid = std::string(baseData.playerGuid, baseData.playerGuidSize);
    }

    GetgudSDK::PositionActionData* positionAction =
        new GetgudSDK::PositionActionData(
        matchGuid.c_str(),
        baseData.actionTimeEpoch,
        playerGuid.c_str(),
            *(GetgudSDK::PositionF*)&position, *(GetgudSDK::RotationF*)&rotation);
    std::deque<GetgudSDK::BaseActionData*> actions = {positionAction};
    sendResult = GetgudSDK::SendActions(actions);
    delete positionAction;
  } catch (std::exception& _error) {
      GetgudSDK::logger.Log(GetgudSDK::LogType::FATAL,
          std::string("GetgudSDK::SendDeathAction "
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
    std::string matchGuid;
    std::string reporterName;
    std::string suspectedPlayerGuid;

    if (reportInfo.matchGuid != NULL &&
        strlen(reportInfo.matchGuid) == reportInfo.matchGuidSize)
    {
        matchGuid = std::string(reportInfo.matchGuid, reportInfo.matchGuidSize);
    }

    if (reportInfo.reporterName != NULL &&
        strlen(reportInfo.reporterName) == reportInfo.reporterNameSize)
    {
        reporterName = std::string(reportInfo.reporterName, reportInfo.reporterNameSize);
    }

    if (reportInfo.suspectedPlayerGuid != NULL &&
        strlen(reportInfo.suspectedPlayerGuid) == reportInfo.suspectedPlayerGuidSize)
    {
        suspectedPlayerGuid = std::string(reportInfo.suspectedPlayerGuid, reportInfo.suspectedPlayerGuidSize);
    }

    GetgudSDK::ReportInfo reportInfoOut;

    reportInfoOut.MatchGuid = matchGuid.c_str();
    reportInfoOut.ReportedTimeEpoch = reportInfo.reportedTimeEpoch;
    reportInfoOut.ReporterName = reporterName.c_str();
    reportInfoOut.ReporterSubType = static_cast<GetgudSDK::ReporterSubtype>(reportInfo.reporterSubType);
    reportInfoOut.ReporterType = static_cast<GetgudSDK::ReporterType>(reportInfo.reporterType);
    reportInfoOut.SuggestedToxicityScore = reportInfo.suggestedToxicityScore;
    reportInfoOut.SuspectedPlayerGuid = suspectedPlayerGuid.c_str();
    reportInfoOut.TbTimeEpoch = reportInfo.tbTimeEpoch;
    reportInfoOut.TbType = static_cast<GetgudSDK::TbType>(reportInfo.tbType);

    sendResult = GetgudSDK::SendInMatchReport(reportInfoOut);
  } catch (std::exception& _error) {
      GetgudSDK::logger.Log(GetgudSDK::LogType::FATAL,
          std::string("GetgudSDK::SendDeathAction "
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
    std::string matchGuid;
    std::string playerGuid;
    std::string message;

    if (messageInfo.matchGuid != NULL &&
        strlen(messageInfo.matchGuid) == messageInfo.matchGuidSize)
    {
        matchGuid = std::string(messageInfo.matchGuid, messageInfo.matchGuidSize);
    }

    if (messageInfo.playerGuid != NULL &&
        strlen(messageInfo.playerGuid) == messageInfo.playerGuidSize)
    {
        playerGuid = std::string(messageInfo.playerGuid, messageInfo.playerGuidSize);
    }

    if (messageInfo.message != NULL &&
        strlen(messageInfo.message) == messageInfo.messageSize)
    {
        message = std::string(messageInfo.message, messageInfo.messageSize);
    }

    GetgudSDK::ChatMessageInfo messageInfoOut;

    messageInfoOut.message = message.c_str();
    messageInfoOut.messageTimeEpoch = messageInfo.messageTimeEpoch;
    messageInfoOut.playerGuid = playerGuid.c_str();
    
    sendResult = GetgudSDK::SendChatMessage(matchGuid.c_str(), messageInfoOut);
  } catch (std::exception& _error) {
      GetgudSDK::logger.Log(GetgudSDK::LogType::FATAL,
          std::string("GetgudSDK::SendDeathAction "
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
  const char* privateKey, int privateKeySize, ReportInfo reportInfo)
{
  bool sendResult = false;
  try {

    std::string inPrivateKey;
    std::string matchGuid;
    std::string reporterName;
    std::string suspectedPlayerGuid;

    if (privateKey != NULL &&
        strlen(privateKey) == privateKeySize)
    {
        inPrivateKey = std::string(privateKey, privateKeySize);
    }

    if (reportInfo.matchGuid != NULL &&
        strlen(reportInfo.matchGuid) == reportInfo.matchGuidSize)
    {
        matchGuid = std::string(reportInfo.matchGuid, reportInfo.matchGuidSize);
    }

    if (reportInfo.reporterName != NULL &&
        strlen(reportInfo.reporterName) == reportInfo.reporterNameSize)
    {
        reporterName = std::string(reportInfo.reporterName, reportInfo.reporterNameSize);
    }

    if (reportInfo.suspectedPlayerGuid != NULL &&
        strlen(reportInfo.suspectedPlayerGuid) == reportInfo.suspectedPlayerGuidSize)
    {
        suspectedPlayerGuid = std::string(reportInfo.suspectedPlayerGuid, reportInfo.suspectedPlayerGuidSize);
    }

    GetgudSDK::ReportInfo reportInfoOut;
    //required
    reportInfoOut.MatchGuid = matchGuid.c_str();
    //required
    reportInfoOut.ReportedTimeEpoch = reportInfo.reportedTimeEpoch;
    reportInfoOut.TbType = static_cast<GetgudSDK::TbType>(reportInfo.tbType);
    if (reportInfo.reporterNameSize > 0)
      reportInfoOut.ReporterName = reporterName.c_str();
    if (reportInfo.reporterSubType != -1)
      reportInfoOut.ReporterSubType = static_cast<GetgudSDK::ReporterSubtype>(reportInfo.reporterSubType);
    if (reportInfo.reporterType != -1)
      reportInfoOut.ReporterType = static_cast<GetgudSDK::ReporterType>(reportInfo.reporterType);
    if (reportInfo.suggestedToxicityScore != -1)
      reportInfoOut.SuggestedToxicityScore = reportInfo.suggestedToxicityScore;
    //required
    reportInfoOut.SuspectedPlayerGuid = suspectedPlayerGuid.c_str();
    if (reportInfo.tbTimeEpoch != -1)
      reportInfoOut.TbTimeEpoch = reportInfo.tbTimeEpoch;

    std::deque<GetgudSDK::ReportInfo> reports;
    reports.push_back(reportInfoOut);

    if (privateKeySize > 0)
    {
      sendResult = GetgudSDK::SendReports(titleId, inPrivateKey.c_str(), reports);
    }
    else
    {
      sendResult = GetgudSDK::SendReports(reports);
    }
  } catch (std::exception& _error) {
      GetgudSDK::logger.Log(GetgudSDK::LogType::FATAL,
          std::string("GetgudSDK::SendDeathAction "
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
  const char* privateKey, int privateKeySize, PlayerInfo player)
{
    GetgudSDK::logger.Log(GetgudSDK::LogType::FATAL,
        std::string("GetgudSDK::UpdatePlayer "
            "validation started"));
  bool sendResult = false;
  GetgudSDK::PlayerInfo playerOut;

  try {
    std::string inPrivateKey;
    std::string playerEmail;
    std::string playerGuid;
    std::string PlayerCampaign;
    std::string playerDevice;
    std::string playerSuspectScore;
    std::string playerGender;
    std::string playerLocation;
    std::string playerNickname;
    std::string playerNotes;
    std::string playerOS;
    std::string playerReputation;
    std::string playerStatus;

    if (privateKey != NULL && strlen(privateKey) == privateKeySize) {
        inPrivateKey = std::string(privateKey, privateKeySize);
    }

    if (player.playerEmail != NULL && strlen(player.playerEmail) == player.playerEmailSize) {
        playerEmail = std::string(player.playerEmail, player.playerEmailSize);
    }

    if (player.playerGuid != NULL && strlen(player.playerGuid) == player.playerGuidSize) {
        playerGuid = std::string(player.playerGuid, player.playerGuidSize);
    }

    if (player.PlayerCampaign != NULL && strlen(player.PlayerCampaign) == player.PlayerCampaignSize) {
        PlayerCampaign = std::string(player.PlayerCampaign, player.PlayerCampaignSize);
    }

    if (player.playerDevice != NULL && strlen(player.playerDevice) == player.playerDeviceSize) {
        playerDevice = std::string(player.playerDevice, player.playerDeviceSize);
    }

    if (player.playerSuspectScore != NULL && strlen(player.playerSuspectScore) == player.playerSuspectScoreSize) {
        playerSuspectScore = std::string(player.playerSuspectScore, player.playerSuspectScoreSize);
    }

    if (player.playerGender != NULL && strlen(player.playerGender) == player.playerGenderSize) {
        playerGender = std::string(player.playerGender, player.playerGenderSize);
    }

    if (player.playerLocation != NULL && strlen(player.playerLocation) == player.playerLocationSize) {
        playerLocation = std::string(player.playerLocation, player.playerLocationSize);
    }

    if (player.playerNickname != NULL && strlen(player.playerNickname) == player.playerNicknameSize) {
        playerNickname = std::string(player.playerNickname, player.playerNicknameSize);
    }

    if (player.playerNotes != NULL && strlen(player.playerNotes) == player.playerNotesSize) {
        playerNotes = std::string(player.playerNotes, player.playerNotesSize);
    }

    if (player.playerOS != NULL && strlen(player.playerOS) == player.playerOSSize) {
        playerOS = std::string(player.playerOS, player.playerOSSize);
    }

    if (player.playerReputation != NULL && strlen(player.playerReputation) == player.playerReputationSize) {
        playerReputation = std::string(player.playerReputation, player.playerReputationSize);
    }

    if (player.playerStatus != NULL && strlen(player.playerStatus) == player.playerStatusSize) {
        playerStatus = std::string(player.playerStatus, player.playerStatusSize);
    }

    if (player.playerEmailSize > 0)
      playerOut.PlayerEmail = playerEmail.c_str();
    //required
    playerOut.PlayerGuid = playerGuid.c_str();
    if (player.playerJoinDateEpoch != -1)
      playerOut.PlayerJoinDateEpoch = player.playerJoinDateEpoch;
    if (player.playerNicknameSize > 0)
      playerOut.PlayerNickname = player.playerNickname;
    if (player.playerRank != -1)
      playerOut.PlayerRank = player.playerRank;
    if (player.playerSuspectScoreSize > 0)
      playerOut.PlayerSuspectScore = player.playerSuspectScore;
    if (player.playerReputationSize > 0)
        playerOut.PlayerReputation = player.playerReputation;
    if (player.playerStatusSize > 0)
        playerOut.PlayerStatus = player.playerStatus;
    if (player.PlayerCampaignSize > 0)
        playerOut.PlayerCampaign = player.PlayerCampaign;
    if (player.playerNotesSize > 0)
        playerOut.PlayerNotes = player.playerNotes;
    if (player.playerDeviceSize > 0)
        playerOut.PlayerDevice = playerDevice.c_str();
    if (player.playerOSSize > 0)
        playerOut.PlayerOS = playerOS.c_str();
    if (player.playerAge != -1)
        playerOut.PlayerAge = player.playerAge;
    if (player.playerGenderSize > 0)
        playerOut.PlayerGender = playerGender.c_str();
    if (player.playerLocationSize > 0)
        playerOut.PlayerLocation = playerLocation.c_str();
    if (player.transactions != NULL && player.transactionsSize > 0)
    {
        std::vector<GetgudSDK::PlayerTransactions> transactions;
        for (int i = 0; i < player.transactionsSize; i++)
        {
            std::string transactionGuid;
            std::string transactionName;
            PlayerTransactions& transaction_ref = player.transactions[i];
            if (transaction_ref.TransactionGuid != NULL && strlen(transaction_ref.TransactionGuid) == transaction_ref.TransactionGuidSize) {
                transactionGuid = std::string(transaction_ref.TransactionGuid, transaction_ref.TransactionGuidSize);
            }
            if (transaction_ref.TransactionName != NULL && strlen(transaction_ref.TransactionName) == transaction_ref.TransactionNameSize) {
                transactionName = std::string(transaction_ref.TransactionName, transaction_ref.TransactionNameSize);
            }
            GetgudSDK::PlayerTransactions playerTransaction =
            {
                transactionGuid.c_str(),
                transactionName.c_str(),
                transaction_ref.TransactionDateEpoch,
                transaction_ref.TransactionValueUSD
            };
            transactions.push_back(playerTransaction);
        }
        playerOut.Transactions = transactions;
    }

    GetgudSDK::logger.Log(GetgudSDK::LogType::FATAL,
        std::string("GetgudSDK::UpdatePlayer "
            "player validated"));

    std::deque<GetgudSDK::PlayerInfo> players;
    players.push_back(playerOut);
    if (privateKeySize > 0)
    {
      sendResult = UpdatePlayers(titleId, inPrivateKey.c_str(), players);
    }
    else
    {
      sendResult = UpdatePlayers(players);
    }
  } catch (std::exception& _error) {
      GetgudSDK::logger.Log(GetgudSDK::LogType::FATAL,
          std::string("GetgudSDK::UpdatePlayer "
              "can not be sent: ") +
          std::string(_error.what()));
  }
  GetgudSDK::logger.Log(GetgudSDK::LogType::FATAL,
      std::string("GetgudSDK::UpdatePlayer "
          "pushed new player info"));
  return sendResult;
}

/**
 * Dispose:
 *
 **/
void dispose() {
  try {
  GetgudSDK::Dispose();
  } catch (std::exception& _error) {
      GetgudSDK::logger.Log(GetgudSDK::LogType::FATAL,
          std::string("GetgudSDK::Dispose "
              "can not be called: ") +
          std::string(_error.what()));
  }
}
} //extern "C"