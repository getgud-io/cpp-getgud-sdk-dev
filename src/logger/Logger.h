#pragma once

#include <mutex>
#include <string>

#include "../../include/actions/Helpers.h"

namespace GetGudSdk {
class Logger {
 private:
  std::mutex m_writeMutex;

 private:
  void WriteToFile(std::string outString);
  long GetFileSize();
  void ManageConfigFileSize();

 public:
  void Log(LogType logType, std::string logString);
  void WriteActionToFile(std::string fileName, std::string outString);
};
}  // namespace GetGudSdk