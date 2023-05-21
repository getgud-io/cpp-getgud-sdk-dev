#pragma once

#include <chrono>
#include <deque>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>
#include "../../include/actions/Helpers.h"
#include "../actions_buffer/ActionsBuffer.h"
#include "../game_container/GameContainer.h"

#define CURL_STATICLIB
#include <curl/curl.h>

namespace GetGudSdk {
class GameSender;

/**
 * Shared data structure that stores all known game senders for easy access
 **/
struct SharedGameSenders {
  int gameSendersCount = 0;
  std::vector<GameSender*> gameSenders;
  std::mutex gameSendersMutex;
};

class GameSender {
 public:
  GameSender();
  void Start(int sleepIntervalMilli);
  void Dispose();
  ~GameSender();

 private:
  int sleepTimeMilli = 0;
  std::thread updaterThread;
  bool threadWorking = false;
  curl_slist* headers = NULL;
  CURL* curl = nullptr;
  std::string curlReadBuffer;

 private:
  void InitCurl();
  void SendNextGame();
  void ManageHyperMode();
  void ThrottleCheckGameMatches(GameData* gameDataToSend);
  bool SendThrottleCheckForMatch(std::string& packet);
  void SendGamePacket(std::string& packet);
};
}  // namespace GetGudSdk