#include "pch.h"
#include "PlayerUpdater.h"
#include "../../config/Config.h"
#include "../../logger/Logger.h"
#include "PlayerData.h"

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
  // curl call bacl to grab the response from middleware correctly
  ((std::string*)userp)->append((char*)contents, size * nmemb);
  return size * nmemb;
}

}  // namespace

namespace GetGudSdk {
extern Logger logger;
extern Config sdkConfig;
extern SharedPlayerUpdaters sharedPlayerUpdaters;

/**
 * PlayerUpdater:
 *
 **/
PlayerUpdater::PlayerUpdater() {
  InitCurl();
  
  sharedPlayerUpdaters.playerUpdatersCount++;
}

/**
 * Start:
 *
 * Start new Player Updater thread
 **/
void PlayerUpdater::Start(int sleepIntervalMilli) {
  m_sleepTimeMilli = sleepIntervalMilli;
  m_threadWorking = true;
  logger.Log(LogType::DEBUG,
             "Player Updater is starting with the sleep interval of " +
                 std::to_string(sleepIntervalMilli));

  m_updaterThread = std::thread([&]() {
    // Stagger the creation of the threads to avoid them all waking up at the
    // same time
    std::this_thread::sleep_for(std::chrono::milliseconds(
        sdkConfig.hyperModeThreadCreationStaggerMilliseconds));
    while (m_threadWorking) {
      std::this_thread::sleep_for(std::chrono::milliseconds(m_sleepTimeMilli));
      SendNextPlayerBatch();
    }
    this->~PlayerUpdater();
  });
}

/**
 * AddPlayers:
 *
 * Add new players to the players to update vector
 **/
bool PlayerUpdater::AddPlayers(int titleId,
                               std::string privateKey,
                               std::deque<PlayerInfo>& arrivingPlayerVector) {
  // before doing anything, make sure the player vector has enough room for more
  // players
  if (m_playerVector.size() > sdkConfig.playersMaxBufferSizeInBytes) {
    logger.Log(LogType::WARN,
               "PlayerUpdater::AddPlayers->Reached max player buffer size, "
               "cannot add more players");
    return false;
  }

  m_playerUpdaterMutex.lock();

  // insert the arriving new players to the player vector, to be consumed later
  // on by the background thread
  for (int index = 0; index < arrivingPlayerVector.size(); index++) {
    PlayerData* playerData =
        new PlayerData(titleId, privateKey, arrivingPlayerVector[index]);
    if (!playerData->IsValid()) {
      delete playerData;
      m_playerUpdaterMutex.unlock();
      return false;
    }
    m_playerVector.push_back(playerData);
    m_playerBufferSize += GetPlayerDataSize();
  }

  logger.Log(
      LogType::DEBUG,
      std::string("Added " + std::to_string(arrivingPlayerVector.size()) +
                  " action(s) to Player Updater"));
  m_playerVector.shrink_to_fit();
  m_playerUpdaterMutex.unlock();

  return true;
}

/**
 * SendNextPlayerBatch:
 *
 * Send next batch of players to Getgud
 **/
void PlayerUpdater::SendNextPlayerBatch() {
  // Check the max allowed players that we can send in a single https request
  int maxPlayerUpdatesToSendAtOnce = sdkConfig.maxPlayerUpdatesToSendAtOnce;

  std::vector<PlayerData*> playersBatch;
  PlayerData* playerData;

  m_playerUpdaterMutex.lock();

  // check if there are any players to send
  if (m_playerVector.size() == 0) {
    m_playerVector.clear();
    m_playerUpdaterMutex.unlock();
    return;
  }

  // the first arriving player will determine the title id for the next packet
  // -> find players with the same title to create the packet of players
  int packetTitleId = m_playerVector[0]->m_titleId;
  std::string packetPrivateKey = m_playerVector[0]->m_privateKey;
  for (int index = 0; index < m_playerVector.size() &&
                      playersBatch.size() < maxPlayerUpdatesToSendAtOnce;
       index++) {
    playerData = m_playerVector[index];
    if (playerData->m_titleId == packetTitleId) {
      playersBatch.push_back(playerData);
      m_playerVector.erase(m_playerVector.begin() + index);
      index--;
      m_playerBufferSize -= GetPlayerDataSize();
    }
  }

  m_playerVector.shrink_to_fit();
  m_playerUpdaterMutex.unlock();
  if (playersBatch.empty())
    return;

  logger.Log(LogType::DEBUG,
             std::string("Popped " + std::to_string(playersBatch.size()) +
                         " Player Updates for sending"));
  // Convert the playersBatch into players packet
  std::string playersPacket;
  playersPacket += "{\n";
  playersPacket += "	\"privateKey\": \"" + packetPrivateKey + "\",\n";
  playersPacket += "	\"titleId\": " + std::to_string(packetTitleId) + ",\n";
  playersPacket += "	\"players\":[";
  for (PlayerData* player : playersBatch) {
    playersPacket += player->ToString() + ',';
    delete player;
  }
  playersPacket.pop_back();
  playersPacket += +"]\n}";

  // Send players update to Getgud
  // TODO: delete this log for prod
  logger.Log(LogType::DEBUG, playersPacket);
  SendUpdatePlayerPacket(playersPacket);
}

/**
 * SendUpdatePlayerPacket:
 *
 * Send packet to getgud
 **/
void PlayerUpdater::SendUpdatePlayerPacket(std::string& packet) {
  if (!m_curl || packet.empty()) {
    return;
  }
  m_curlReadBuffer.clear();
  curl_easy_setopt(m_curl, CURLOPT_URL, sdkConfig.updatePlayersURL.c_str());
  curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, packet.c_str());

  // time when we should stop trying to send packet
  auto stopWaitingTime =
      std::chrono::system_clock::now() +
      std::chrono::milliseconds(sdkConfig.apiWaitTimeMilliseconds);

  CURLcode sendCode = CURLE_UNKNOWN_OPTION;

  while (sendCode != CURLE_OK &&
         stopWaitingTime > std::chrono::system_clock::now()) {
    // send prepared packet
    sendCode = curl_easy_perform(m_curl);
    // make small delay in order to save hardware usage
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }

  if (sendCode != CURLcode::CURLE_OK) {
    logger.Log(
        LogType::_ERROR,
        "PlayerUpdater::SendUpdatePlayerPacket->Failed to send packet: " +
            std::string(curl_easy_strerror(sendCode)));
  }
}

/**
 * InitCurl:
 *
 * Prepare CURL for sending requests
 **/
void PlayerUpdater::InitCurl() {
  logger.Log(LogType::DEBUG, "REST Api connection prepared in PlayerUpdater");

  m_curl = curl_easy_init();

  if (!m_curl) {
    logger.Log(LogType::WARN, "PlayerUpdater->InitCurl Couldn't init curl");
  }
  m_headers = curl_slist_append(m_headers, "Accept: application/json");
  m_headers = curl_slist_append(m_headers, "Content-Type: application/json");
  m_headers = curl_slist_append(m_headers, "charset: utf-8");

  curl_easy_setopt(m_curl, CURLOPT_POST, 1);
  curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_headers);
  curl_easy_setopt(m_curl, CURLOPT_URL, sdkConfig.updatePlayersURL.c_str());
  curl_easy_setopt(m_curl, CURLOPT_TIMEOUT_MS,
                   sdkConfig.apiTimeoutMilliseconds);
  curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, CURLWriteCallback);
  curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &m_curlReadBuffer);
}

/**
 * ~PlayerUpdater:
 *
 **/
PlayerUpdater::~PlayerUpdater() {
  // Iterate through all the actions in the buffer and delete them

  if (m_curl) {
    curl_easy_cleanup(m_curl);
  }
  curl_slist_free_all(m_headers);
  logger.Log(LogType::DEBUG, "Player Updater stopped..");
}

/**
 * Dispose:
 *
 **/
void PlayerUpdater::Dispose() {
  m_threadWorking = false;
  m_playerUpdaterMutex.lock();
  for (auto* playerData : m_playerVector) {
    delete playerData;
  }
  m_playerVector.clear();
  m_playerBufferSize = 0;
  m_playerUpdaterMutex.unlock();

  m_updaterThread.detach();
  {
    std::lock_guard<std::mutex> locker(sharedPlayerUpdaters.playerUpdatersMutex);
    sharedPlayerUpdaters.playerUpdatersCount--;
  }
}

}  // namespace GetGudSdk