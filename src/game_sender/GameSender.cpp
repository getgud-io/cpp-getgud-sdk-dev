#include "GameSender.h"
#include "../../include/actions/Helpers.h"
#include "../actions_buffer/ActionsBuffer.h"
#include "../config/Config.h"
#include "../game_container/GameContainer.h"
#include "../game_container/GameData.h"
#include "../logger/logger.h"

#include <memory>

namespace {

/**
 * CURLWriteCallback:
 *
 * Curl custom callback to grab response from Getgud
 **/
size_t CURLWriteCallback(char* contents,
                         size_t size,
                         size_t nmemb,
                         void* userp) {
  if (userp == nullptr)
    return 0;
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

}  // namespace

namespace GetGudSdk {
extern Logger logger;
extern ActionsBuffer actionsBuffer;
extern GameContainer gameContainer;
extern Config sdkConfig;
SharedGameSenders sharedGameSenders;

/**
 * GameSender:
 *
 **/
GameSender::GameSender() {
  InitCurl();
  // TODO: here is push_back bug, might be not pushed
  sharedGameSenders.gameSendersCount++;
  sharedGameSenders.gameSenders.push_back(this);
}

/**
 * Start:
 *
 * Start new Game Sender thread
 **/
void GameSender::Start(int sleepIntervalMilli) {
  sleepTimeMilli = sleepIntervalMilli;
  threadWorking = true;
  logger.Log(LogType::DEBUG,
             "Game Sender is starting with the sleep interval of " +
                 std::to_string(sleepIntervalMilli));

  updaterThread = std::thread([&]() {
    // Stagger the creation of the threads to avoid them all waking up at the
    // same time
    // TODO: we can replace this by random waiting time from min to max
    std::this_thread::sleep_for(std::chrono::milliseconds(
        sdkConfig.hyperModeThreadCreationStaggerMilliseconds));
    while (threadWorking) {
      std::this_thread::sleep_for(std::chrono::milliseconds(sleepTimeMilli));
      ManageHyperMode();
      SendNextGame();
    }
    this->~GameSender();  // TODO: self destruct shouldn't be
  });
}

/**
 * SendNextGame:
 *
 * Grab new actions, search for next game packet to send, send it if exists
 **/
void GameSender::SendNextGame() {
  // first thing is first, grab all the actions from from the buffer and inject
  // them to the game container
  std::deque<BaseActionData*> nextActions = actionsBuffer.PopActions();
  bool result = gameContainer.AddActions(nextActions);
  if (result == false)
    logger.Log(LogType::WARN,
               "GameSender::sendNextGame()->Failed to add actions to "
               "GameContainer (it's probably full)");

  // get the next game we need to send. might be that there are no games to send
  GameData* gameDataToSend = gameContainer.PopNextGameToProcess();
  if (gameDataToSend == nullptr)
    return;

  // If there is a game packet to send go through each match of the packet
  // check if it is interesting for Getgud or no with a thtrottle check
  // api request.
  ThrottleCheckGameMatches(gameDataToSend);

  // convert the game to a sendable string and send it to Getgud.io's cloud
  // using curl
  std::string gameOut;
  gameDataToSend->GameToString(gameOut);
  // TODO: clean this logs for prod
  logger.Log(LogType::WARN, gameOut);
  SendGamePacket(gameOut);

  // Dispose actions
  gameDataToSend->Dispose();
  delete gameDataToSend;
}

/**
 * ManageHyperMode:
 *
 * Check if action buffer OR game container are more than X% full, if yes
 * spawn extra Game Senders
 **/
void GameSender::ManageHyperMode() {
  if (!sdkConfig.hyperModeFeatureEnabled)
    return;

  if (sdkConfig.hyperModeMaxThreads <= 0)
    return;

  if (sdkConfig.hyperModeUpperPercentageBound == 0) {
    logger.Log(LogType::WARN,
               "GameSender->manageHyperMode hyperModeUpperPercentage should be "
               "> 0 for hypermode activation");
    return;
  }

  sharedGameSenders.gameSendersMutex.lock();

  // then check if hyper mode needs to be activated - buffer and container
  // might be small and we do not need extra threads yet
  float actionBufferFilledPercentage =
      static_cast<float>(actionsBuffer.GetAverageSizeInBytes()) /
      static_cast<float>(sdkConfig.actionsBufferMaxSizeInBytes) * 100;
  float gameContainerFilledPercentage =
      static_cast<float>(gameContainer.GetAverageSizeInBytes()) /
      static_cast<float>(sdkConfig.gameContainerMaxSizeInBytes) * 100;

  // understand how many threads we need to spawn in TOTAL
  int numThreadsNeeded = 0;
  if (actionBufferFilledPercentage <= gameContainerFilledPercentage) {
    numThreadsNeeded =
        static_cast<int>(std::round((gameContainerFilledPercentage /
                                     sdkConfig.hyperModeUpperPercentageBound) *
                                    sdkConfig.hyperModeMaxThreads));
  } else {
    numThreadsNeeded =
        static_cast<int>(std::round((actionBufferFilledPercentage /
                                     sdkConfig.hyperModeUpperPercentageBound) *
                                    sdkConfig.hyperModeMaxThreads));
  }

  // Clip the numThreadsNeeded to higher thread number bound from Config
  if (numThreadsNeeded > sdkConfig.hyperModeMaxThreads) {
    numThreadsNeeded = sdkConfig.hyperModeMaxThreads;
  }
  // We should always have at least 1 thread
  if (numThreadsNeeded < 1)
    numThreadsNeeded = 1;

  // Spawn or mark threads for deletion
  int activeThreads = sharedGameSenders.gameSendersCount;
  if (numThreadsNeeded > activeThreads) {
    // Spawn extra threads
    for (int i = activeThreads; i < numThreadsNeeded; i++) {
      auto* newGameSender = new GameSender();

      newGameSender->Start(sdkConfig.gameSenderSleepIntervalMilliseconds);
    }
    logger.Log(LogType::WARN, "Spawned extra threads, total thread count " +
                                  std::to_string(numThreadsNeeded));
  } else if (numThreadsNeeded <
             activeThreads) {  // in case we need to delete extra threads
    int threadsToStop = activeThreads - numThreadsNeeded;

    // stop as many extra game senders as we need
    for (int i = 0; i < threadsToStop; i++) {
      sharedGameSenders.gameSenders.back()->Dispose();
      sharedGameSenders.gameSenders.pop_back();
    }
    logger.Log(LogType::WARN, "Removed extra threads, total thread count " +
                                  std::to_string(numThreadsNeeded));
  }

  sharedGameSenders.gameSendersMutex.unlock();
}

/**
 * ThrottleCheckGameMatches:
 *
 * Do a throttle check on the prepared game packet, so we understand if each
 *match of the game is interesting for Getgud or no
 **/
void GameSender::ThrottleCheckGameMatches(GameData* gameDataToSend) {
  // for every match of the prepared game packet
  for (auto& match : gameDataToSend->GetMatchMap()) {
    // if match wasn't throttle checked yet (we do throttle check once per
    // match)
    if (!match.second->GetThrottleChecked()) {
      // check if match has players at all, in other words if the match has
      // actions
      auto playerGuids = match.second->GetPlayerGuids();
      if (playerGuids.empty()) {
        // in this case we will consider match not interesting yet
        continue;
      }
      // start preparing throttle check request JSON body
      std::string packet;
      packet += "{ \n";
      packet +=
          "	\"privateKey\": \"" + gameDataToSend->GetPrivateKey() + "\",\n";
      packet +=
          "	\"titleId\": " + std::to_string(gameDataToSend->GetTitleId()) +
          ",\n";
      packet +=
          "	\"serverGuid\": \"" + gameDataToSend->GetServerGuid() + "\",\n";
      packet +=
          "	\"gameMode\": \"" + gameDataToSend->GetGameMode() + "\",\n";
      packet +=
          "	\"matchMode\": \"" + match.second->GetMatchMode() + "\",\n";
      packet += "	\"mapName\": \"" + match.second->GetMapName() + "\",\n";
      packet += "	\"playerGuids\": [";
      for (auto& playerGuid : playerGuids) {
        packet += "\"" + playerGuid + "\",";
      }
      packet.pop_back();  // delete last comma
      packet += "]\n}";

      // send throttle check request
      bool result = SendThrottleCheckForMatch(packet);
      // save throttle check result for the match
      match.second->SetThrottleCheckResults(true, result);

      // Because we Slice too large packets we also need to make sure we
      // set throttle check results for the same match but in original
      // match map, we may be working in the copy of match map after Slice
      auto match_it = gameContainer.GetMatchMap().find(match.first);
      if (match_it != gameContainer.GetMatchMap().end()) {
        // set the same throttle check as for the potential copy
        match_it->second->SetThrottleCheckResults(true, result);
      }
    }
  }
}

/**
 * SendThrottleCheckForMatch:
 *
 * Send throttle check request for specific match
 **/
bool GameSender::SendThrottleCheckForMatch(std::string& packet) {
  // in case somthing goes wrong we will consider match not interesting
  if (!curl) {
    return false;
  }

  curlReadBuffer.clear();
  curl_easy_setopt(curl, CURLOPT_URL, sdkConfig.throttleCheckUrl.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, packet.c_str());

  // time when we should stop trying to send packet
  auto stopWaitingTime =
      std::chrono::system_clock::now() +
      std::chrono::milliseconds(sdkConfig.apiWaitTimeMilliseconds);

  CURLcode sendCode = CURLE_UNKNOWN_OPTION;
  bool result = false;  // we consider match not interesting by default
  while (!result && sendCode != CURLE_OK &&
         stopWaitingTime > std::chrono::system_clock::now()) {
    // send prepared packet
    sendCode = curl_easy_perform(curl);
    // make small delay in order to save hardware usage
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  if (sendCode == CURLcode::CURLE_OK) {
    logger.Log(LogType::DEBUG, "Packet sent: " + curlReadBuffer);
    // check what response we got from Getgud
    if (curlReadBuffer == "true" || curlReadBuffer == "TRUE")
      result = true;
  } else {
    logger.Log(LogType::_ERROR, "Failed to send throttle request: " +
                                    std::string(curl_easy_strerror(sendCode)));
  }

  return result;
}

/**
 * SendGamePacket:
 *
 * Send game packet with action streams of game matches to Getgud
 **/
void GameSender::SendGamePacket(std::string& packet) {
  if (!curl || packet.empty()) {
    return;
  }
  curlReadBuffer.clear();
  curl_easy_setopt(curl, CURLOPT_URL, sdkConfig.streamGameURL.c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, packet.c_str());

  // time when we should stop trying to send packet
  auto stopWaitingTime =
      std::chrono::system_clock::now() +
      std::chrono::milliseconds(sdkConfig.apiWaitTimeMilliseconds);

  CURLcode sendCode = CURLE_UNKNOWN_OPTION;

  while (sendCode != CURLE_OK &&
         stopWaitingTime > std::chrono::system_clock::now()) {
    // send prepared packet
    sendCode = curl_easy_perform(curl);
    // make small delay in order to save hardware usage
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  if (sendCode == CURLcode::CURLE_OK) {
    logger.Log(LogType::DEBUG, "Packet sent: " + curlReadBuffer);
  } else {
    logger.Log(LogType::_ERROR, "Failed to send packet: " +
                                    std::string(curl_easy_strerror(sendCode)));
  }
}

/**
 * InitCurl:
 *
 * Prepare CURL for sending requests
 **/
void GameSender::InitCurl() {
  logger.Log(LogType::DEBUG, "REST Api connection prepared in GameSender");

  curl = curl_easy_init();

  if (!curl) {
    logger.Log(LogType::WARN, "GameSender->InitCurl Couldn't init curl");
  }
  headers = curl_slist_append(headers, "Accept: application/json");
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "charset: utf-8");

  // TODO: describe what this does
  curl_easy_setopt(curl, CURLOPT_POST, 1);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_URL, sdkConfig.streamGameURL.c_str());
  curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, sdkConfig.apiTimeoutMilliseconds);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CURLWriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curlReadBuffer);
}

/**
 * Dispose:
 *
 **/
void GameSender::Dispose() {
  updaterThread.detach();
  threadWorking = false;
  sharedGameSenders.gameSendersCount--;
  logger.Log(LogType::WARN, "Game sender disposed");
}

/**
 * ~GameSender:
 *
 **/
GameSender::~GameSender() {
  if (curl) {
    curl_easy_cleanup(curl);
  }
  curl_slist_free_all(headers);
}

}  // namespace GetGudSdk