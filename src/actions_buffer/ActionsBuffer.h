#pragma once

#include <chrono>
#include <deque>
#include <map>
#include <mutex>
#include "../../include/actions/BaseActionData.h"
#include "../utils/Utils.h"

namespace GetGudSdk {

class ActionsBuffer {
 private:
  std::mutex actionsBufferLocker;
  std::deque<BaseActionData*> actionsBuffer;
  unsigned int actionsBufferSize = 0;
  FilledAverage averageSize;

 public:
  std::deque<BaseActionData*> PopActions();
  bool AddActions(std::deque<BaseActionData*>& actions);
  unsigned int GetSizeInBytes();
  unsigned int GetAverageSizeInBytes();
  void Dispose();
};
}  // namespace GetGudSdk