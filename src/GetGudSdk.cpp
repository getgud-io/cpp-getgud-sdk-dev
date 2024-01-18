#include "pch.h"
#include "../include/GetGudSdk.h"
#include <exception>
#include <string>
#include "config/Config.h"
#include "game_container/GameContainer.h"
#include "senders/game_sender/GameSender.h"
#include "logger/Logger.h"
#include "senders/player_updater/PlayerUpdater.h"
#include "senders/report_sender/ReportSender.h"
#include "utils/Utils.h"

namespace GetGudSdk {
extern SharedGameSenders sharedGameSenders;
extern SharedPlayerUpdaters sharedPlayerUpdaters;
extern SharedReportSenders sharedReportSenders;
extern Config sdkConfig;
extern Logger logger;
extern ActionsBuffer actionsBuffer;
extern GameContainer gameContainer;

/**
 * Init:
 *
 * Init Getgud SDK
 **/
bool Init() {
  bool init_result = false;
  try {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    if (sdkConfig.LoadSettings())
    {
      logger.Log(LogType::DEBUG,
        "Loaded config with the following parameters: \n" +
        sdkConfig.ToString());
      init_result = true;
    }
    else
    {
      sdkConfig.logToFile = true;
      logger.Log(LogType::_ERROR,
        "Config can not be loaded, exit");
      sdkConfig.logToFile = false;
    }
  } catch (std::exception& _error) {
    sdkConfig.logToFile = true;
    logger.Log(
        LogType::FATAL,
        std::string("GetGudSdk::Init->Couldn't initialize Getgud SDK: ") +
            std::string(_error.what()));
    sdkConfig.logToFile = false;
  }

  return init_result;
}

/**
 * StartGame:
 * Start new game
 **/
std::string StartGame(int titleId,
                      std::string privateKey,
                      std::string serverGuid,
                      std::string gameMode,
                      std::string serverLocation) {
  std::string gameGuid;

  try {
    if (sharedGameSenders.gameSenders.empty()) {
      GetGudSdk::GameSender* gameSender = nullptr;
      {  // lock_guard scope
        // Create first Game Sender, if hypermode is on there will be more than
        // 1
        std::lock_guard<std::mutex> locker(sharedGameSenders.gameSendersMutex);
        gameSender = new GameSender();
      }
      // Start all services
      gameSender->Start(sdkConfig.gameSenderSleepIntervalMilliseconds);
    }
    gameGuid = gameContainer.AddGame(titleId, privateKey, serverGuid, gameMode, serverLocation);
  } catch (std::exception& _error) {
    logger.Log(LogType::FATAL,
               std::string("GetGudSdk::StartGame->Couldn't start new game: ") +
                   std::string(_error.what()));
  }

  return gameGuid;
}

/**
 * StartGame:
 *
 * Start new game
 **/
std::string StartGame(std::string serverGuid, std::string gameMode, std::string serverLocation) {
  std::string gameGuid;

  try {
     if (sharedGameSenders.gameSenders.empty()) {
      GetGudSdk::GameSender* gameSender = nullptr;
      {  // lock_guard scope
        // Create first Game Sender, if hypermode is on there will be more than
        // 1
        std::lock_guard<std::mutex> locker(sharedGameSenders.gameSendersMutex);
        gameSender = new GameSender();
      }
      // Start all services
      gameSender->Start(sdkConfig.gameSenderSleepIntervalMilliseconds);
    }

    const char* titleIdHolder = std::getenv("GETGUD_TITLE_ID");
    const char* privateKeyHolder = std::getenv("GETGUD_PRIVATE_KEY");

    if (titleIdHolder == nullptr || privateKeyHolder == nullptr) {
      logger.Log(LogType::FATAL,
                 std::string(
                     "GetGudSdk::StartGame->Environment variables GETGUD_TITLE_ID and "
                     "GETGUD_PRIVATE_KEY are required"));
    }

    std::string titleId(titleIdHolder);
    std::string privateKey(privateKeyHolder);

    gameGuid = gameContainer.AddGame(std::stoi(titleId.c_str()), privateKey,
                                     serverGuid, gameMode, serverLocation);
  } catch (std::exception& _error) {
    logger.Log(LogType::FATAL,
               std::string("GetGudSdk::StartGame->Couldn't start new game") +
                   std::string(_error.what()));
  }

  return gameGuid;
}

/**
 * Start match:
 *
 * Start a new match for an existing game
 **/
std::string StartMatch(std::string gameGuid,
                       std::string matchMode,
                       std::string mapName) {
  std::string matchGuid;
  try {
    matchGuid = gameContainer.AddMatch(gameGuid, matchMode, mapName);

  } catch (std::exception& _error) {
    logger.Log(
        LogType::FATAL,
        std::string("GetGudSdk::StartMatch->Couldn't start new match: ") +
            std::string(_error.what()));
  }

  return matchGuid;
}

/**
 * MarkEndGame:
 *
 * Mark started game as finished
 **/
bool MarkEndGame(std::string gameGuid) {
  bool gameEnded = false;
  try {
    gameEnded = gameContainer.MarkEndGame(gameGuid);
  } catch (std::exception& _error) {
    logger.Log(LogType::FATAL,
               std::string("GetGudSdk::MarkEndGame->Couldn't end game: ") +
                   std::string(_error.what()));
  }

  return gameEnded;
}

/**
 * SendInMatchReport:
 *
 * Send a report which belongs to specifc match which is now live
 **/
bool SendInMatchReport(ReportInfo reportInfo) {
  bool inMatchReportAdded = false;
  try {
    inMatchReportAdded = gameContainer.AddInMatchReport(reportInfo);
  } catch (std::exception& _error) {
    logger.Log(
        LogType::FATAL,
        std::string(
            "GetGudSdk::SendInMatchReport->Couldn't add in match report: ") +
            std::string(_error.what()));
  }

  return inMatchReportAdded;
}

/**
 * SendChatMessage:
 *
 *  Send a message which belongs to specifc match which is now live
 **/
bool SendChatMessage(std::string matchGuid, ChatMessageInfo messageInfo) {
  bool chatMessageSent = false;
  try {
    chatMessageSent = gameContainer.AddChatMessage(matchGuid, messageInfo);
  } catch (std::exception& _error) {
    logger.Log(
        LogType::FATAL,
        std::string(
            "GetGudSdk::SendChatMessage->Couldn't send match chat message: ") +
            std::string(_error.what()));
  }

  return chatMessageSent;
}

/**
 * SendActions:
 *
 * Add actions to an action buffer
 **/
bool SendActions(std::deque<BaseActionData*> actions) {
  bool actionsSent = false;
  int actionsSize = 0;
  try {
    actionsSize = actions.size();
    actionsSent = actionsBuffer.AddActions(actions);
  } catch (std::exception& _error) {
    logger.Log(LogType::FATAL, std::string("GetGudSdk::SendActions->Couldn't "
                                           "send actions to Action Buffer: ") +
                                   std::string(_error.what()));
  }

  return actionsSent;
}

/**
 * SendAction:
 *
 * Send a single action to an action buffer
 **/
bool SendAction(BaseActionData* action) {
  bool sendResult = false;
  std::deque<BaseActionData*> actions;
  try {
    actions = { action };
    sendResult = SendActions(actions);
  } catch (std::exception& _error)
  {
      logger.Log(LogType::FATAL, std::string("GetGudSdk::SendAction->Couldn't "
          "send actions to Action Buffer: ") +
          std::string(_error.what()));
  }
  return sendResult;
}

/**
 * SendAffectAction:
 *
 **/
bool SendAffectAction(std::string matchGuid,
  long long actionTimeEpoch,
  std::string playerGuid,
  std::string affectGuid,
  AffectState affectState) {
  bool sendResult = false;
  try {
    AffectActionData* affectAction =
      new AffectActionData(matchGuid, actionTimeEpoch, playerGuid, affectGuid, affectState);
    std::deque<BaseActionData*> actions = { affectAction };
    sendResult = SendActions(actions);
    delete affectAction;
  } catch (std::exception& _error)
  {
      logger.Log(LogType::FATAL, std::string("GetGudSdk::SendAffectAction->Couldn't "
          "send actions to Action Buffer: ") +
          std::string(_error.what()));
  }
  return sendResult;
}

/**
 * SendAttackAction:
 *
 **/
bool SendAttackAction(std::string matchGuid,
                      long long actionTimeEpoch,
                      std::string playerGuid,
                      std::string weaponGuid) {
  bool sendResult = false;
  try {
    AttackActionData* attackAction =
        new AttackActionData(matchGuid, actionTimeEpoch, playerGuid, weaponGuid);
    std::deque<BaseActionData*> actions = { attackAction };
    sendResult = SendActions(actions);
    delete attackAction;
  } catch (std::exception& _error)
  {
    logger.Log(LogType::FATAL, std::string("GetGudSdk::SendAttackAction->Couldn't "
        "send actions to Action Buffer: ") +
        std::string(_error.what()));
  }
  return sendResult;
}

/**
 * SendDamageAction:
 *
 **/
bool SendDamageAction(std::string matchGuid,
                      long long actionTimeEpoch,
                      std::string playerGuid,
                      std::string victimPlayerGuid,
                      float damageDone,
                      std::string weaponGuid) {
  bool sendResult = false;
  try {
    DamageActionData* damageAction =
        new DamageActionData(matchGuid, actionTimeEpoch, playerGuid,
                             victimPlayerGuid, damageDone, weaponGuid);
    std::deque<BaseActionData*> actions = {damageAction};
    sendResult = SendActions(actions);
    delete damageAction;
  } catch (std::exception& _error)
  {
    logger.Log(LogType::FATAL, std::string("GetGudSdk::SendDamageAction->Couldn't "
        "send actions to Action Buffer: ") +
        std::string(_error.what()));
  }
  return sendResult;
}

/**
 * SendHealAction:
 *
 **/
bool SendHealAction(std::string matchGuid,
                    long long actionTimeEpoch,
                    std::string playerGuid,
                    float healthGained) {
  bool sendResult = false;
  try {
    HealActionData* healAction =
        new HealActionData(matchGuid, actionTimeEpoch, playerGuid, healthGained);
    std::deque<BaseActionData*> actions = {healAction};
    sendResult = SendActions(actions);
    delete healAction;
  } catch (std::exception& _error)
  {
    logger.Log(LogType::FATAL, std::string("GetGudSdk::SendHealAction->Couldn't "
        "send actions to Action Buffer: ") +
        std::string(_error.what()));
  }
  return sendResult;
}

/**
 * SendSpawnAction:
 *
 **/
bool SendSpawnAction(std::string matchGuid,
                     long long actionTimeEpoch,
                     std::string playerGuid,
                     std::string characterGuid,
                     int teamId,
                     float initialHealth,
                     PositionF position,
                     RotationF rotation) {
  bool sendResult = false;
  try {
    SpawnActionData* spawnAction =
        new SpawnActionData(matchGuid, actionTimeEpoch, playerGuid, characterGuid,
                            teamId, initialHealth, position, rotation);
    std::deque<BaseActionData*> actions = {spawnAction};
    sendResult = SendActions(actions);
    delete spawnAction;
  } catch (std::exception& _error)
  {
    logger.Log(LogType::FATAL, std::string("GetGudSdk::SendSpawnAction->Couldn't "
        "send actions to Action Buffer: ") +
        std::string(_error.what()));
  }
  return sendResult;
}

/**
 * SendDeathAction:
 *
 **/
bool SendDeathAction(std::string matchGuid,
                     long long actionTimeEpoch,
                     std::string playerGuid,
                     std::string attackerGuid) {
  bool sendResult = false;
  try {
    DeathActionData* deathAction =
        new DeathActionData(matchGuid, actionTimeEpoch, playerGuid, attackerGuid);
    std::deque<BaseActionData*> actions = {deathAction};
    sendResult = SendActions(actions);
    delete deathAction;
  } catch (std::exception& _error)
  {
      logger.Log(LogType::FATAL, std::string("GetGudSdk::SendDeathAction->Couldn't "
          "send actions to Action Buffer: ") +
          std::string(_error.what()));
  }
  return sendResult;
}

/**
 * SendPositionAction:
 *
 **/
bool SendPositionAction(std::string matchGuid,
                        long long actionTimeEpoch,
                        std::string playerGuid,
                        PositionF position,
                        RotationF rotation) {
  bool sendResult = false;
  try {
  PositionActionData* positionAction = new PositionActionData(
      matchGuid, actionTimeEpoch, playerGuid, position, rotation);
  std::deque<BaseActionData*> actions = {positionAction};
  sendResult = SendActions(actions);
  delete positionAction;
  } catch (std::exception& _error)
  {
      logger.Log(LogType::FATAL, std::string("GetGudSdk::SendPositionAction->Couldn't "
          "send actions to Action Buffer: ") +
          std::string(_error.what()));
  }
  return sendResult;
}

/**
 * SendReports:
 *
 * Send reports which are outside of the live match
 **/
bool SendReports(int titleId,
                 std::string privateKey,
                 std::deque<ReportInfo>& reports) {
  try {
    if (sharedReportSenders.reportSendersCount == 0) {
      std::lock_guard<std::mutex> locker(sharedReportSenders.reportSendersMutex);
      if (sharedReportSenders.reportSendersCount == 0)  // double check in mutex
      {
        sharedReportSenders.reportSender = new ReportSender();
        sharedReportSenders.reportSender->Start(
            sdkConfig.gameSenderSleepIntervalMilliseconds);
      }
    }

    return sharedReportSenders.reportSender->AddReports(titleId, privateKey,
                                                        reports);
  } catch (std::exception& _error) {
    logger.Log(
        LogType::FATAL,
        std::string(
            "GetGudSdk::SendReports->Failed, reports can not be sent: ") +
            std::string(_error.what()));
    return false;
  }
}

/**
 * SendReports:
 *
 * Send reports which are outside of the live match
 **/
bool SendReports(std::deque<ReportInfo>& reports) {
  std::string titleId;
  std::string privateKey;
  bool result = false;
  try {

    const char* titleIdVar = std::getenv("GETGUD_TITLE_ID");
    const char* privateKeyVar = std::getenv("GETGUD_PRIVATE_KEY");

    if (titleIdVar == nullptr || privateKeyVar == nullptr) {
      logger.Log(
          LogType::FATAL,
          std::string(
              "GetGudSdk::SendReports->Environment variables GETGUD_TITLE_ID and "
              "GETGUD_PRIVATE_KEY are required"));
      return false;
    }

    titleId = std::string(titleIdVar);
    privateKey = std::string(privateKeyVar);
    result = SendReports(std::stoi(titleId.c_str()), privateKey, reports);
  } catch (std::exception& _error) {
    logger.Log(
        LogType::FATAL,
        std::string(
            "GetGudSdk::SendReports->Failed, reports can not be sent: ") +
            std::string(_error.what()));
    return false;
  }

  return result;
  
}

/**
 * UpdatePlayers:
 *
 * Update player info outside of the live match
 **/
bool UpdatePlayers(int titleId,
                   std::string privateKey,
                   std::deque<PlayerInfo>& players) {
  bool result = false;
  try {
    if (sharedPlayerUpdaters.playerUpdatersCount == 0) {
      std::lock_guard<std::mutex> locker(
          sharedPlayerUpdaters.playerUpdatersMutex);
      if (sharedPlayerUpdaters.playerUpdatersCount == 0)  // double check in mutex
      {
        sharedPlayerUpdaters.playerUpdater = new PlayerUpdater();
        sharedPlayerUpdaters.playerUpdater->Start(
            sdkConfig.gameSenderSleepIntervalMilliseconds);
      }
    }
    result = sharedPlayerUpdaters.playerUpdater->AddPlayers(
        titleId, privateKey, players);
  } catch (std::exception& _error) {
    logger.Log(LogType::FATAL,
               std::string("GetGudSdk::UpdatePlayers->Player data "
                           "can not be updated: ") +
                   std::string(_error.what()));
  }
  return result;
}

/**
 * UpdatePlayers:
 *
 * Update player info outside of the live match
 **/
bool UpdatePlayers(std::deque<PlayerInfo>& players) {
  std::string titleId;
  std::string privateKey;
  bool result = false;
  try {
    const char* titleIdVar = std::getenv("GETGUD_TITLE_ID");
    const char* privateKeyVar = std::getenv("GETGUD_PRIVATE_KEY");

    if (titleIdVar == nullptr || privateKeyVar == nullptr) {
      logger.Log(
          LogType::FATAL,
          std::string(
              "GetGudSdk::UpdatePlayers->Environment variables GETGUD_TITLE_ID and "
              "GETGUD_PRIVATE_KEY are required"));
      return false;
    }

    titleId = std::string(titleIdVar);
    privateKey = std::string(privateKeyVar);
    result = UpdatePlayers(std::stoi(titleId), privateKey, players);
  }
  catch (std::exception& _error) {
    logger.Log(LogType::FATAL,
               std::string("GetGudSdk::UpdatePlayers->Player data "
                           "can not be updated: ") +
                   std::string(_error.what()));
  }
  return result;
}

/**
 * Dispose:
 *
 **/
void Dispose() {
  try {
    actionsBuffer.Dispose();
    gameContainer.Dispose();
    {
      std::lock_guard<std::mutex> locker(
          sharedReportSenders.reportSendersMutex);
      if (sharedReportSenders.reportSender != nullptr) {
        sharedReportSenders.reportSender->Dispose();
        sharedReportSenders.reportSender = nullptr;
      }
    }

    {
      std::lock_guard<std::mutex> locker(
          sharedPlayerUpdaters.playerUpdatersMutex);
      if (sharedPlayerUpdaters.playerUpdater != nullptr) {
        sharedPlayerUpdaters.playerUpdater->Dispose();
        sharedPlayerUpdaters.playerUpdater = nullptr;
      }
    }

    for (auto& senderThread : sharedGameSenders.gameSenders)
      senderThread->Dispose();
    sharedGameSenders.gameSenders.clear();
    sharedGameSenders.gameSendersCount = 0;
    curl_global_cleanup();

    #ifdef _DEBUG
    sdkConfig.actionsAmount = 0;
    sdkConfig.totalCreatedActions = 0;
    sdkConfig.emptyActionsAmount = 0;
    sdkConfig.totalCreatedEmptyActions = 0;
    #endif

    logger.Log(LogType::DEBUG, std::string("SDK stopped.."));
  } catch (std::exception& _error) {
    logger.Log(LogType::FATAL,
               std::string("GetGudSdk::Dispose->Couldn't dispose SDK: ") + std::string(_error.what()));
  }
}

}  // namespace GetGudSdk