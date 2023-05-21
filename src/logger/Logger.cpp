#include "Logger.h"
#include <fstream>
#include <iostream>
#include "../config/Config.h"
#include "../utils/Utils.h"

namespace GetGudSdk {
Logger logger;  // const initialization at this stage

extern Config sdkConfig;

/**
 * WriteToFile:
 *
 * Write the whole log string into the output file so it can be later accessed
 * by client for debugging purposes
 **/
void Logger::WriteToFile(std::string outString) {
  if (sdkConfig.logsFilePath.empty())
    return;
  writeMutex.lock();
  std::ofstream logsFile;
  logsFile.open(std::string(sdkConfig.logsFilePath).c_str(),
                std::ios_base::app);
  logsFile << outString;
  logsFile.close();
  writeMutex.unlock();
}

/**
 * Log:
 *
 * Central logging operation. Decided if to log string or not
 **/
void Logger::Log(LogType logType, std::string logString) {
  std::string messageTag;
  std::string current_time = GetCurrentTimeString();
  std::string messageEnd = "\n";
  if (logType == LogType::DEBUG && sdkConfig.logLevel >= LogLevel::FULL) {
    messageTag = "::Debug-> ";
    WriteToFile(current_time + messageTag + logString + messageEnd);
  } else if (logType >= LogType::WARN &&
             sdkConfig.logLevel >= LogLevel::WARN_AND_ERROR) {
    messageTag = "::Warning-> ";
    WriteToFile(current_time + messageTag + logString + messageEnd);
  } else if (logType == LogType::_ERROR &&
             sdkConfig.logLevel >= LogLevel::_ERROR) {
    messageTag = "::Error-> ";
    WriteToFile(current_time + messageTag + logString + messageEnd);
  } else if (logType == LogType::FATAL &&
             sdkConfig.logLevel >= LogLevel::FATAL) {
    messageTag = "::Fatal-> ";
    WriteToFile(current_time + messageTag + logString + messageEnd);
  }
}
}  // namespace GetGudSdk