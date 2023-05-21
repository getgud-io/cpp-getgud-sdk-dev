#pragma once

#include <chrono>
#include <thread>
#include <unordered_map>
#include <deque>
#include <mutex>
#include "PlayerData.h"
#define CURL_STATICLIB
#include <curl/curl.h>

namespace GetGudSdk {
class PlayerUpdater {
 public:
  PlayerUpdater();
  void Start(int sleepIntervalMilli);
  bool AddPlayers(int titleId,
                  std::string privateKey,
                  std::deque<PlayerInfo*>& arrivingPlayerVector);
  ~PlayerUpdater();
  void Dispose();

 private:
  // holds all the reports being sent by the client to the SDK
  std::vector<PlayerData*> playerVector;

  int sleepTimeMilli = 0;
  std::thread updaterThread;
  bool threadWorking = false;
  curl_slist* headers = NULL;
  CURL* curl = nullptr;
  std::string curlReadBuffer;
  std::mutex playerUpdaterMutex;
  unsigned int playerBufferSize = 0;

 private:
  void InitCurl();
  void SendNextPlayerBatch();
  void SendUpdatePlayerPacket(std::string& packet);
};
}  // namespace GetGudSdk