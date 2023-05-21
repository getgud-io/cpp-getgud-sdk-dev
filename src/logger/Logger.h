#pragma once

#include <mutex>
#include <string>

#include "../../include/actions/Helpers.h"

namespace GetGudSdk {
class Logger {
 private:
  std::mutex writeMutex;

 private:
  void WriteToFile(std::string outString);

 public:
  void Log(LogType logType, std::string logString);
};
}  // namespace GetGudSdk