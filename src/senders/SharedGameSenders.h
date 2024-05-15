#pragma once
#include <vector>
#include <mutex>

namespace GetgudSDK {

class GameSender;

/**
 * Shared data structure that stores all known game senders for easy access
 **/
struct SharedGameSenders {
  int gameSendersCount = 0;
  std::vector<GameSender*> gameSenders;
  std::mutex gameSendersMutex;
};
}  // namespace GetgudSDK