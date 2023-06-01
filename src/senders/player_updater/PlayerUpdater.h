#pragma once

#include <chrono>
#include <deque>
#include <mutex>
#include <thread>
#include <vector>
#include <unordered_map>
#include "PlayerData.h"
#include "../SharedPlayerUpdaters.h"
#define CURL_STATICLIB
#include <curl/curl.h>

namespace GetGudSdk {
class PlayerUpdater {
 public:
  PlayerUpdater();
  void Start(int sleepIntervalMilli);
  bool AddPlayers(int titleId,
                  std::string privateKey,
                  std::deque<PlayerInfo>& arrivingPlayerVector);
  ~PlayerUpdater();
  void Dispose();

 private:
  // holds all the reports being sent by the client to the SDK
  std::vector<PlayerData*> m_playerVector;

  int m_sleepTimeMilli = 0;
  std::thread m_updaterThread;
  bool m_threadWorking = false;
  curl_slist* m_headers = NULL;
  CURL* m_curl = nullptr;
  std::string m_curlReadBuffer;
  std::mutex m_playerUpdaterMutex;
  unsigned int m_playerBufferSize = 0;

 private:
  void InitCurl();
  void SendNextPlayerBatch();
  void SendUpdatePlayerPacket(std::string& packet);
};
}  // namespace GetGudSdk