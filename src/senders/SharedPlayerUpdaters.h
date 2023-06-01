#pragma once
#include <mutex>
#include <vector>

namespace GetGudSdk {

class PlayerUpdater;

/**
 * Shared data structure that stores all known player updaters for easy access
 **/
//TODO make it using vector
struct SharedPlayerUpdaters {
  int playerUpdatersCount = 0;
  PlayerUpdater* playerUpdater;
  std::mutex playerUpdatersMutex;
};
}  // namespace GetGudSdk