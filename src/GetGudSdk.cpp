#include "pch.h"
#include "../include/GetGudSdk.h"
#include <exception>
#include <string>
#include "config/Config.h"
#include "game_container/GameContainer.h"
#include "game_sender/GameSender.h"
#include "logger/Logger.h"
#include "player_updater/PlayerUpdater.h"
#include "report_sender/ReportSender.h"
#include "utils/Utils.h"

namespace GetGudSdk {
extern SharedGameSenders sharedGameSenders;
ReportSender* reportSender = nullptr;
PlayerUpdater* playerUpdater = nullptr;
extern Config sdkConfig;
extern Logger logger;
extern ActionsBuffer actionsBuffer;
extern GameContainer gameContainer;

/**
 * Init:
 *
 * Init Getgud SDK
 **/
void Init() {
  bool locked = false;
  try {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    sdkConfig.LoadSettings();
    logger.Log(LogType::DEBUG,
               "Loaded config with the following parameters: \n" +
                   sdkConfig.ToString());
  } catch (std::exception& _error) {
    logger.Log(
        LogType::FATAL,
        std::string("GetGudSdk::Init->Couldn't initialize Getgud SDK: ") +
            std::string(_error.what()));
  }
}

/**
 * StartGame:
 *
 * Start new game
 **/
std::string StartGame(int titleId,
                      std::string privateKey,
                      std::string serverGuid,
                      std::string gameMode) {
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
    gameGuid = gameContainer.AddGame(titleId, privateKey, serverGuid, gameMode);
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
std::string StartGame(std::string serverGuid, std::string gameMode) {
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

    const char* titleIdHolder = std::getenv("TITLE_ID");
    const char* privateKeyHolder = std::getenv("PRIVATE_KEY");

    if (titleIdHolder == nullptr || privateKeyHolder == nullptr) {
      logger.Log(LogType::FATAL,
                 std::string(
                     "GetGudSdk::StartGame->Environment variables TITLE_ID and "
                     "PRIVATE_KEY are required"));
    }

    std::string titleId(titleIdHolder);
    std::string privateKey(privateKeyHolder);

    gameGuid = gameContainer.AddGame(std::stoi(titleId.c_str()), privateKey,
                                     serverGuid, gameMode);
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
  std::deque<BaseActionData*> actions = {action};
  return SendActions(actions);
}

/**
 * SendAttackAction:
 *
 **/
bool SendAttackAction(std::string matchGuid,
                      long long actionTimeEpoch,
                      std::string playerGuid,
                      std::string weaponGuid) {
  AttackActionData* attackAction =
      new AttackActionData(matchGuid, actionTimeEpoch, playerGuid, weaponGuid);
  std::deque<BaseActionData*> actions = {attackAction};
  bool sendResult = SendActions(actions);
  delete attackAction;
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
  DamageActionData* damageAction =
      new DamageActionData(matchGuid, actionTimeEpoch, playerGuid,
                           victimPlayerGuid, damageDone, weaponGuid);
  std::deque<BaseActionData*> actions = {damageAction};
  bool sendResult = SendActions(actions);
  delete damageAction;
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
  HealActionData* healAction =
      new HealActionData(matchGuid, actionTimeEpoch, playerGuid, healthGained);
  std::deque<BaseActionData*> actions = {healAction};
  bool sendResult = SendActions(actions);
  delete healAction;
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
  SpawnActionData* spawnAction =
      new SpawnActionData(matchGuid, actionTimeEpoch, playerGuid, characterGuid,
                          teamId, initialHealth, position, rotation);
  std::deque<BaseActionData*> actions = {spawnAction};
  bool sendResult = SendActions(actions);
  delete spawnAction;
  return sendResult;
}

/**
 * SendDeathAction:
 *
 **/
bool SendDeathAction(std::string matchGuid,
                     long long actionTimeEpoch,
                     std::string playerGuid) {
  DeathActionData* deathAction =
      new DeathActionData(matchGuid, actionTimeEpoch, playerGuid);
  std::deque<BaseActionData*> actions = {deathAction};
  bool sendResult = SendActions(actions);
  delete deathAction;
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
  PositionActionData* positionAction = new PositionActionData(
      matchGuid, actionTimeEpoch, playerGuid, position, rotation);
  std::deque<BaseActionData*> actions = {positionAction};
  bool sendResult = SendActions(actions);
  delete positionAction;
  return sendResult;
  ;
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
    if (reportSender == nullptr) {
      reportSender = new ReportSender();
      reportSender->Start(sdkConfig.gameSenderSleepIntervalMilliseconds);
    }
    return reportSender->AddReports(titleId, privateKey, reports);
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
  try {
    if (reportSender == nullptr) {
      reportSender = new ReportSender();
      reportSender->Start(sdkConfig.gameSenderSleepIntervalMilliseconds);
    }

    const char* titleIdVar = std::getenv("TITLE_ID");
    const char* privateKeyVar = std::getenv("PRIVATE_KEY");

    if (titleIdVar == nullptr || privateKeyVar == nullptr) {
      logger.Log(
          LogType::FATAL,
          std::string(
              "GetGudSdk::SendReports->Environment variables TITLE_ID and "
              "PRIVATE_KEY are required"));
      return false;
    }

    titleId = std::string(titleIdVar);
    privateKey = std::string(privateKeyVar);
  } catch (std::exception& _error) {
    logger.Log(
        LogType::FATAL,
        std::string(
            "GetGudSdk::SendReports->Failed, reports can not be sent: ") +
            std::string(_error.what()));
    return false;
  }

  return SendReports(std::stoi(titleId.c_str()), privateKey, reports);
  
}

/**
 * UpdatePlayers:
 *
 * Update player info outside of the live match
 **/
bool UpdatePlayers(int titleId,
                   std::string privateKey,
                   std::deque<PlayerInfo>& players) {
  try {
    if (playerUpdater == nullptr) {
      playerUpdater = new PlayerUpdater();

      playerUpdater->Start(sdkConfig.gameSenderSleepIntervalMilliseconds);
    }
    return playerUpdater->AddPlayers(titleId, privateKey, players);
  } catch (std::exception& _error) {
    logger.Log(LogType::FATAL,
               std::string("GetGudSdk::UpdatePlayers->Player data "
                           "can not be updated: ") +
                   std::string(_error.what()));
    return false;
  }
}

/**
 * UpdatePlayers:
 *
 * Update player info outside of the live match
 **/
bool UpdatePlayers(std::deque<PlayerInfo>& players) {
  std::string titleId;
  std::string privateKey;
  try {
    if (playerUpdater == nullptr) {
      playerUpdater = new PlayerUpdater();

      playerUpdater->Start(sdkConfig.gameSenderSleepIntervalMilliseconds);
    }

    const char* titleIdVar = std::getenv("TITLE_ID");
    const char* privateKeyVar = std::getenv("PRIVATE_KEY");

    if (titleIdVar == nullptr || privateKeyVar == nullptr) {
      logger.Log(
          LogType::FATAL,
          std::string(
              "GetGudSdk::UpdatePlayers->Environment variables TITLE_ID and "
              "PRIVATE_KEY are required"));
      return false;
    }

    titleId = std::string(titleIdVar);
    privateKey = std::string(privateKeyVar);
  }
  catch (std::exception& _error) {
    logger.Log(LogType::FATAL,
               std::string("GetGudSdk::UpdatePlayers->Player data "
                           "can not be updated: ") +
                   std::string(_error.what()));
    return false;
  }
  return UpdatePlayers(std::stoi(titleId), privateKey, players);
}

/**
 * Dispose:
 *
 **/
void Dispose() {
  try {
    actionsBuffer.Dispose();
    gameContainer.Dispose();
    if (reportSender != nullptr) {
      reportSender->Dispose();
      reportSender = nullptr;
    }
    if (playerUpdater != nullptr) {
      playerUpdater->Dispose();
      playerUpdater = nullptr;
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