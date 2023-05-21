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

    // if (reportSender == nullptr)
    //   reportSender = new ReportSender();
    //
    // if (playerUpdater == nullptr)
    //   playerUpdater = new PlayerUpdater();

    // Create first Game Sender, if hypermode is on there will be more than 1
    sharedGameSenders.gameSendersMutex.lock();
    locked = true;
    GetGudSdk::GameSender* gameSender = new GameSender();
    sharedGameSenders.gameSendersMutex.unlock();
    locked = false;

    // Start all services
    gameSender->Start(sdkConfig.gameSenderSleepIntervalMilliseconds);
    // playerUpdater->Start(sdkConfig.gameSenderSleepIntervalMilliseconds);
    // reportSender->Start(sdkConfig.gameSenderSleepIntervalMilliseconds);
  } catch (std::exception& _error) {
    if (locked)
      sharedGameSenders.gameSendersMutex.unlock();
    logger.Log(LogType::FATAL, std::string("Couldn't initialize Getgud SDK: ") +
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
                      std::string serverName,
                      std::string gameMode) {
  std::string gameGuid;

  try {
    gameGuid = gameContainer.AddGame(titleId, privateKey, serverName, gameMode);
  } catch (std::exception& _error) {
    logger.Log(LogType::FATAL, std::string("Couldn't start new game: ") +
                                   std::string(_error.what()));
  }

  if (!gameGuid.empty()) {
    logger.Log(LogType::DEBUG, std::string("Game started: " + gameGuid));
  }

  return gameGuid;
}

/**
 * StartGame:
 *
 * Start new game
 **/
std::string StartGame(std::string serverName, std::string gameMode) {
  std::string gameGuid;

  try {
    const char* titleIdHolder = std::getenv("TITLE_ID");
    const char* privateKeyHolder = std::getenv("PRIVATE_KEY");

    if (titleIdHolder == nullptr || privateKeyHolder == nullptr) {
      logger.Log(LogType::FATAL,
                 std::string("Error: environment variables CONFIG_PATH and "
                             "CONFIG_FILENAME are required"));
    }

    std::string titleId(titleIdHolder);
    std::string privateKey(privateKeyHolder);

    gameGuid = gameContainer.AddGame(std::stoi(titleId.c_str()), privateKey,
                                     serverName, gameMode);
  } catch (std::exception& _error) {
    logger.Log(LogType::FATAL, std::string("Couldn't start new game") +
                                   std::string(_error.what()));
  }

  if (!gameGuid.empty()) {
    logger.Log(LogType::DEBUG, std::string("Game started: " + gameGuid));
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
    logger.Log(LogType::FATAL, std::string("Couldn't start new match") +
                                   std::string(_error.what()));
  }
  if (!matchGuid.empty()) {
    logger.Log(LogType::DEBUG, std::string("Match started: " + matchGuid));
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
               std::string("Couldn't end game") + std::string(_error.what()));
  }

  if (gameEnded) {
    logger.Log(LogType::DEBUG,
               std::string("Stopped game with game guid: " + gameGuid));
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
    logger.Log(LogType::FATAL, std::string("Couldn't add in match report") +
                                   std::string(_error.what()));
  }

  if (inMatchReportAdded) {
    logger.Log(LogType::DEBUG, std::string("Added report to match with guid: " +
                                           reportInfo.MatchGuid));
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
    logger.Log(LogType::FATAL, std::string("Couldn't send match chat message") +
                                   std::string(_error.what()));
  }

  if (chatMessageSent) {
    logger.Log(
        LogType::DEBUG,
        std::string("Added chat message to match with guid: " + matchGuid));
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
    logger.Log(LogType::FATAL, std::string("Couldn't send actions") +
                                   std::string(_error.what()));
  }

  if (actionsSent) {
    logger.Log(LogType::DEBUG,
               std::string("Added " + std::to_string(actionsSize)) +
                   " action(s) to ActionBuffer");
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
                 std::deque<ReportInfo*>& reports) {
  try {
    return reportSender->AddReports(titleId, privateKey, reports);
  } catch (std::exception& _error) {
    logger.Log(LogType::FATAL,
               std::string(
                   "GetgudSDK::SendReports failed, reports will not be sent.") +
                   std::string(_error.what()));
    return false;
  }
}

/**
 * SendReports:
 *
 * Send reports which are outside of the live match
 **/
bool SendReports(std::deque<ReportInfo*>& reports) {
  const char* titleIdVar = std::getenv("TITLE_ID");
  const char* privateKeyVar = std::getenv("PRIVATE_KEY");

  if (titleIdVar == nullptr || privateKeyVar == nullptr) {
    logger.Log(LogType::FATAL,
               std::string("Error: environment variables CONFIG_PATH and "
                           "CONFIG_FILENAME are required"));
  }

  std::string titleId(titleIdVar);
  std::string privateKey(privateKeyVar);
  return SendReports(std::stoi(titleId.c_str()), privateKey, reports);
}

/**
 * UpdatePlayers:
 *
 * Update player info outside of the live match
 **/
bool UpdatePlayers(int titleId,
                   std::string privateKey,
                   std::deque<PlayerInfo*>& players) {
  try {
    return playerUpdater->AddPlayers(titleId, privateKey, players);
  } catch (std::exception& _error) {
    logger.Log(LogType::FATAL,
               std::string("GetgudSDK::UpdatePlayers failed, player data "
                           "will not be updated.") +
                   std::string(_error.what()));
    return false;
  }
}

/**
 * UpdatePlayers:
 *
 * Update player info outside of the live match
 **/
bool UpdatePlayers(std::deque<PlayerInfo*>& players) {
  const char* titleIdVar = std::getenv("TITLE_ID");
  const char* privateKeyVar = std::getenv("PRIVATE_KEY");

  if (titleIdVar == nullptr || privateKeyVar == nullptr) {
    logger.Log(LogType::FATAL,
               std::string("Error: environment variables CONFIG_PATH and "
                           "CONFIG_FILENAME are required"));
  }

  // TODO validate ENV first
  std::string titleId(titleIdVar);
  std::string privateKey(privateKeyVar);

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
    if (reportSender)
      reportSender->Dispose();
    if (playerUpdater)
      playerUpdater->Dispose();

    // TODO: is this correct that we dispose them because they are pointers?
    if (reportSender)
      delete reportSender;
    if (playerUpdater)
      delete playerUpdater;

    logger.Log(LogType::DEBUG, std::string("SDK disposed"));
  } catch (std::exception& _error) {
    logger.Log(LogType::FATAL,
               std::string("Dispose error") + std::string(_error.what()));
  }
}

}  // namespace GetGudSdk