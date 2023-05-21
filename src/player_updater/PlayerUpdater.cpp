#include "PlayerUpdater.h"
#include "../config/Config.h"
#include "../logger/Logger.h"
#include "PlayerData.h"

namespace {  // unnamed namespace
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

PlayerUpdater::PlayerUpdater() {
  InitCurl();
}

void PlayerUpdater::Start(int sleepIntervalMilli) {
  sleepTimeMilli = sleepIntervalMilli;
  threadWorking = true;
  logger.Log(LogType::DEBUG,
             "Player Updater is starting with the sleep interval of " +
                 std::to_string(sleepIntervalMilli));

  updaterThread = std::thread([&]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(
        sdkConfig.hyperModeThreadCreationStaggerMilliseconds));
    while (threadWorking) {
      std::this_thread::sleep_for(std::chrono::milliseconds(sleepTimeMilli));
      SendNextPlayerBatch();
    }
  });
}

bool PlayerUpdater::AddPlayers(int titleId,
                               std::string privateKey,
                               std::deque<PlayerInfo*>& arrivingPlayerVector) {
  // before doing anything, make sure the player vector has enough room for more
  // players
  if (playerVector.size() > sdkConfig.playersMaxBufferSizeInBytes) {
    logger.Log(LogType::DEBUG,
               "PlayerUpdater::AddPlayers->Reached max player buffer size, "
               "cannot add more players");
    return false;
  }

  playerUpdaterMutex.lock();

  // insert the arriving new players to the player vector, to be consumed later
  // on by the background thread
  for (int index = 0; index < arrivingPlayerVector.size(); index++) {
    // TODO change pointers
    playerVector.push_back(
        new PlayerData(titleId, privateKey, *arrivingPlayerVector[index]));
    playerBufferSize += GetPlayerDataSize();
  }

  playerUpdaterMutex.unlock();

  return true;
}

void PlayerUpdater::SendNextPlayerBatch() {
  // Check the max allowed players that we can send in a single https request
  int maxPlayerUpdatesToSendAtOnce = sdkConfig.maxPlayerUpdatesToSendAtOnce;

  std::vector<PlayerData*> playersBatch;
  std::vector<PlayerData*>::iterator it;
  PlayerData* playerData;

  playerUpdaterMutex.lock();

  // check if there are any players to send
  int playersCount = playerVector.size();
  if (playersCount == 0) {
    playerUpdaterMutex.unlock();
    return;
  }

  // the first arriving player will determine the title id for the next packet
  // -> find players with the same title to create the packet of players
  int packetTitleId = playerVector[0]->titleId;

  for (int index = 0; index < playersCount && index < maxPlayerUpdatesToSendAtOnce;
       index++) {
    playerData = playerVector[index];
    if (playerData->titleId == packetTitleId) {
      playersBatch.push_back(playerData);
      it = playerVector.begin() + index;
      playerVector.erase(it);
      playersCount--;
      playerBufferSize -= GetPlayerDataSize();
    }
  }

  playerUpdaterMutex.unlock();

  // Convert the playersBatch into players packet
  std::string playersPacket;
  playersPacket += +"{\"players\":[";
  for (PlayerData* player : playersBatch) {
    playersPacket += player->ToString();
  }
  playersPacket += +"]}";

  SendUpdatePlayerPacket(playersPacket);
}

void PlayerUpdater::SendUpdatePlayerPacket(std::string& packet) {
  if (!curl || packet.empty()) {
    return;
  }
  curlReadBuffer.clear();
  curl_easy_setopt(curl, CURLOPT_URL, sdkConfig.updatePlayersURL.c_str());
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
    std::this_thread::sleep_for(std::chrono::microseconds(50));
  }

  if (sendCode == CURLcode::CURLE_OK) {
    logger.Log(LogType::DEBUG, "Packet sent: " + curlReadBuffer);
  } else {
    logger.Log(LogType::_ERROR, "Failed to send packet: " +
                                    std::string(curl_easy_strerror(sendCode)));
  }
}

void PlayerUpdater::InitCurl() {
  logger.Log(LogType::DEBUG, "REST Api connection prepared in PlayerUpdater");

  curl = curl_easy_init();

  if (!curl) {
    logger.Log(LogType::WARN, "PlayerUpdater->initCurl Couldn't init curl");
  }
  headers = curl_slist_append(headers, "Accept: application/json");
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers = curl_slist_append(headers, "charset: utf-8");

  // TODO: describe what this does
  curl_easy_setopt(curl, CURLOPT_POST, 1);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_URL, sdkConfig.updatePlayersURL.c_str());
  curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, sdkConfig.apiTimeoutMilliseconds);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CURLWriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curlReadBuffer);
}

PlayerUpdater::~PlayerUpdater() {
  playerUpdaterMutex.lock();
  logger.Log(LogType::DEBUG, "Disposing Player Updater");
  // Iterate through all the actions in the buffer and delete them
  for (auto* playerData : playerVector) {
    delete playerData;
  }

  playerVector.clear();
  playerBufferSize = 0;
  Dispose(); // TODO: do i call inside or outside lock?

  playerUpdaterMutex.unlock();
}

void PlayerUpdater::Dispose() {
  threadWorking = false;
  updaterThread.detach();

  if (curl) {
    curl_easy_cleanup(curl);
  }
  curl_slist_free_all(headers);
  logger.Log(LogType::DEBUG, "Player Updater Disposed");
}

}  // namespace GetGudSdk