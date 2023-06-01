#include "pch.h"
#include "GameSender.h"
#include "../../../include/actions/Helpers.h"
#include "../../actions_buffer/ActionsBuffer.h"
#include "../../config/Config.h"
#include "../../game_container/GameContainer.h"
#include "../../game_container/GameData.h"
#include "../../logger/Logger.h"

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
extern SharedGameSenders sharedGameSenders;

/**
 * GameSender:
 *
 **/
GameSender::GameSender() {
  InitCurl();
  sharedGameSenders.gameSendersCount++;
  sharedGameSenders.gameSenders.push_back(this);
}

/**
 * Start:
 *
 * Start new Game Sender thread
 **/
void GameSender::Start(int sleepIntervalMilli) {
  m_sleepTimeMilli = sleepIntervalMilli;
  m_threadWorking = true;
  logger.Log(LogType::DEBUG,
             "Game Sender is starting with the sleep interval of " +
                 std::to_string(sleepIntervalMilli));

  m_updaterThread = std::thread([&]() {
    // Stagger the creation of the threads to avoid them all waking up at the
    // same time
    std::this_thread::sleep_for(std::chrono::milliseconds(
        sdkConfig.hyperModeThreadCreationStaggerMilliseconds));
    while (m_threadWorking) {
      std::this_thread::sleep_for(std::chrono::milliseconds(m_sleepTimeMilli));
      ManageHyperMode();
      SendNextGame();
    }
    // TODO: For future: we shouldn't be self destructing
    // this is a long term task
    this->~GameSender();
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
               "GameSender::SendNextGame->Failed to add actions to "
               "GameContainer");

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
  if (!gameOut.empty()) {
    logger.Log(LogType::DEBUG, "Sending Game packet for Game guid: " +
                                   gameDataToSend->GetGameGuid());
    SendGamePacket(gameOut);
  }

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

  // hyperModeUpperPercentage should be > 0 for hypermode activation
  if (sdkConfig.hyperModeUpperPercentageBound <= 0) {
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
    logger.Log(LogType::DEBUG, "Spawned extra threads, total thread count " +
                                   std::to_string(numThreadsNeeded));
  } else if (numThreadsNeeded <
             activeThreads) {  // in case we need to delete extra threads
    int threadsToStop = activeThreads - numThreadsNeeded;

    // stop as many extra game senders as we need
    for (int i = 0; i < threadsToStop; i++) {
      sharedGameSenders.gameSenders.back()->Dispose();
      sharedGameSenders.gameSenders.pop_back();
    }
    logger.Log(LogType::DEBUG, "Removed extra threads, total thread count " +
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

      if (result == true) {
        logger.Log(LogType::DEBUG,
                   std::string("Getgud found interesting Match with guid: " +
                               match.second->GetMatchGuid()));
      } else {
        logger.Log(
            LogType::DEBUG,
            std::string("Getgud found NOT interesting Match with guid: " +
                        match.second->GetMatchGuid()));
      }

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
  if (!m_throttleCurl || packet.empty()) {
    return false;
  }

  m_throttleCurlReadBuffer.clear();
  curl_easy_setopt(m_throttleCurl, CURLOPT_POSTFIELDS, packet.c_str());

  // time when we should stop trying to send packet
  auto stopWaitingTime =
      std::chrono::system_clock::now() +
      std::chrono::milliseconds(sdkConfig.apiWaitTimeMilliseconds);

  CURLcode sendCode = CURLE_UNKNOWN_OPTION;
  bool result = true;  // we consider match not interesting by default
  while (!result && sendCode != CURLE_OK &&
         stopWaitingTime > std::chrono::system_clock::now()) {
    // send prepared packet
    sendCode = curl_easy_perform(m_throttleCurl);
    // make small delay in order to save hardware usage
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  if (sendCode == CURLcode::CURLE_OK) {
    // check what response we got from Getgud
    if (m_throttleCurlReadBuffer == "true" ||
        m_throttleCurlReadBuffer == "TRUE")
      result = true;
  } else {
    logger.Log(LogType::_ERROR,
               "GameSender::SendThrottleCheckForMatch->Failed to send throttle "
               "request: " +
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
  if (!m_streamCurl || packet.empty()) {
    return;
  }
  m_streamCurlReadBuffer.clear();
  curl_easy_setopt(m_streamCurl, CURLOPT_POSTFIELDS, packet.c_str());

  // time when we should stop trying to send packet
  auto stopWaitingTime =
      std::chrono::system_clock::now() +
      std::chrono::milliseconds(sdkConfig.apiWaitTimeMilliseconds);

  CURLcode sendCode = CURLE_UNKNOWN_OPTION;

  while (sendCode != CURLE_OK &&
         stopWaitingTime > std::chrono::system_clock::now()) {
    // send prepared packet
    sendCode = curl_easy_perform(m_streamCurl);
    // make small delay in order to save hardware usage
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  if (sendCode != CURLcode::CURLE_OK) {
    logger.Log(LogType::_ERROR,
               "GameSender::SendGamePacket->Failed to send packet: " +
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

  m_throttleCurl = curl_easy_init();
  m_streamCurl = curl_easy_init();

  if (!m_streamCurl || !m_throttleCurl) {
    logger.Log(LogType::WARN, "GameSender->InitCurl Couldn't init curl");
    return;
  }
  //Stream Curl
  m_streamHeaders =
      curl_slist_append(m_streamHeaders, "Accept: application/json");
  m_streamHeaders =
      curl_slist_append(m_streamHeaders, "Content-Type: application/json");
  m_streamHeaders = curl_slist_append(m_streamHeaders, "charset: utf-8");

  curl_easy_setopt(m_streamCurl, CURLOPT_POST, 1);
  curl_easy_setopt(m_streamCurl, CURLOPT_HTTPHEADER, m_streamHeaders);
  curl_easy_setopt(m_streamCurl, CURLOPT_URL, sdkConfig.streamGameURL.c_str());
  curl_easy_setopt(m_streamCurl, CURLOPT_TIMEOUT_MS,
                   sdkConfig.apiTimeoutMilliseconds);
  curl_easy_setopt(m_streamCurl, CURLOPT_WRITEFUNCTION, CURLWriteCallback);
  curl_easy_setopt(m_streamCurl, CURLOPT_WRITEDATA, &m_streamCurlReadBuffer);

  curl_easy_setopt(m_streamCurl, CURLOPT_URL, sdkConfig.streamGameURL.c_str());

  //Throttle Curl
  m_throttleHeaders =
      curl_slist_append(m_throttleHeaders, "Accept: application/json");
  m_throttleHeaders =
      curl_slist_append(m_throttleHeaders, "Content-Type: application/json");
  m_throttleHeaders = curl_slist_append(m_throttleHeaders, "charset: utf-8");

  curl_easy_setopt(m_throttleCurl, CURLOPT_POST, 1);
  curl_easy_setopt(m_throttleCurl, CURLOPT_HTTPHEADER, m_throttleHeaders);
  curl_easy_setopt(m_throttleCurl, CURLOPT_URL,
                   sdkConfig.streamGameURL.c_str());
  curl_easy_setopt(m_throttleCurl, CURLOPT_TIMEOUT_MS,
                   sdkConfig.apiTimeoutMilliseconds);
  curl_easy_setopt(m_throttleCurl, CURLOPT_WRITEFUNCTION, CURLWriteCallback);
  curl_easy_setopt(m_throttleCurl, CURLOPT_WRITEDATA, &m_throttleCurlReadBuffer);

  curl_easy_setopt(m_throttleCurl, CURLOPT_URL,
                   sdkConfig.throttleCheckUrl.c_str());
}

/**
 * Dispose:
 *
 **/
void GameSender::Dispose() {
  m_updaterThread.detach();
  m_threadWorking = false;
  sharedGameSenders.gameSendersCount--;
}

/**
 * ~GameSender:
 *
 **/
GameSender::~GameSender() {
  if (m_throttleCurl) {
    curl_easy_cleanup(m_throttleCurl);
  }
  if (m_streamCurl) {
    curl_easy_cleanup(m_streamCurl);
  }
  if (m_streamHeaders)
    curl_slist_free_all(m_streamHeaders);
  if (m_throttleHeaders)
    curl_slist_free_all(m_throttleHeaders);
}

}  // namespace GetGudSdk