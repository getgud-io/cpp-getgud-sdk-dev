#include "pch.h"
#include "../config/Config.h"
#include "../logger/Logger.h"
#include "../utils/Validator.h"

// file modification verification
#include <sys/stat.h>
#include <sys/types.h>
#ifndef _WIN32
#include <unistd.h>
#endif

#ifdef _WIN32
#define stat _stat
#endif

#include <fstream>
#include <iostream>
#include <memory>

namespace GetgudSDK {
Config sdkConfig;
extern Logger logger;

/**
 * LoadSettings:
 *
 * Get values of variables from the config file
 **/
bool Config::LoadSettings() {
  char* logsFilePathHolder = std::getenv("GETGUD_LOG_FILE_PATH");
  if (logsFilePathHolder == nullptr) {
    // Environment variable LOG_FILE_PATH is required to work
    logger.Log(LogType::WARN, std::string("Config::LoadSettings->Environment "
      "variable GETGUD_LOG_FILE_PATH is empty"));
  }
  else {
    logsFilePath = std::string(logsFilePathHolder);
  }

  std::map<std::string, std::string> configData = ReadUserConfigFile();

  if (configData.empty())
    return false;

  bool valueReadResult = false;

  // Read logger parameters
  GetConfigValue(configData, sdkConfigFieldNames.logToFile,
                 sdkConfig.logToFile);

  unsigned int logFileSizeInBytes = 0;
  valueReadResult = GetConfigValue(
      configData, sdkConfigFieldNames.logFileSizeInBytes, logFileSizeInBytes);
  valueReadResult &=
      Validator::ValidateItemValue(logFileSizeInBytes, 0U, 100000000U);
  if (valueReadResult) {
    sdkConfig.logFileSizeInBytes = logFileSizeInBytes;
  }

  GetConfigValue(configData, sdkConfigFieldNames.circularLogFile,
                 sdkConfig.circularLogFile);

  // Read all Getgud API URLs
  GetConfigValue(configData, sdkConfigFieldNames.streamGameURL,
                 sdkConfig.streamGameURL);
  GetConfigValue(configData, sdkConfigFieldNames.updatePlayersURL,
                 sdkConfig.updatePlayersURL);
  GetConfigValue(configData, sdkConfigFieldNames.sendReportsURL,
                 sdkConfig.sendReportsURL);
  GetConfigValue(configData, sdkConfigFieldNames.throttleCheckUrl,
                 sdkConfig.throttleCheckUrl);

  // Read report sender variables
  unsigned int reportsMaxBufferSizeInBytes = 0;

  valueReadResult = GetConfigValue(
      configData, sdkConfigFieldNames.reportsMaxBufferSizeInBytes,
      reportsMaxBufferSizeInBytes);
  valueReadResult &=
      Validator::ValidateItemValue(reportsMaxBufferSizeInBytes, 0U, 10000000U);
  if (valueReadResult) {
    sdkConfig.reportsMaxBufferSizeInBytes = reportsMaxBufferSizeInBytes;
  }

  unsigned int maxReportsToSendAtOnce = 0;
  valueReadResult =
      GetConfigValue(configData, sdkConfigFieldNames.maxReportsToSendAtOnce,
                     maxReportsToSendAtOnce);
  valueReadResult &=
      Validator::ValidateItemValue(maxReportsToSendAtOnce, 0U, 100U);
  if (valueReadResult) {
    sdkConfig.maxReportsToSendAtOnce = maxReportsToSendAtOnce;
  }

  // Read player updeter variables
  unsigned int playersMaxBufferSizeInBytes = 0;
  valueReadResult = GetConfigValue(
      configData, sdkConfigFieldNames.playersMaxBufferSizeInBytes,
      playersMaxBufferSizeInBytes);
  valueReadResult &=
      Validator::ValidateItemValue(playersMaxBufferSizeInBytes, 0U, 10000000U);
  if (valueReadResult) {
    sdkConfig.playersMaxBufferSizeInBytes = playersMaxBufferSizeInBytes;
  }

  unsigned int maxPlayerUpdatesToSendAtOnce = 0;
  valueReadResult = GetConfigValue(
      configData, sdkConfigFieldNames.maxPlayerUpdatesToSendAtOnce,
      maxPlayerUpdatesToSendAtOnce);
  valueReadResult &=
      Validator::ValidateItemValue(maxPlayerUpdatesToSendAtOnce, 0U, 100U);
  if (valueReadResult) {
    sdkConfig.maxPlayerUpdatesToSendAtOnce = maxPlayerUpdatesToSendAtOnce;
  }

  // Read chat message variables
  unsigned int maxChatMessagesToSendAtOnce = 0;
  valueReadResult = GetConfigValue(
      configData, sdkConfigFieldNames.maxChatMessagesToSendAtOnce,
      maxChatMessagesToSendAtOnce);
  valueReadResult &=
      Validator::ValidateItemValue(maxChatMessagesToSendAtOnce, 0U, 100U);
  if (valueReadResult) {
    sdkConfig.maxChatMessagesToSendAtOnce = maxChatMessagesToSendAtOnce;
  }

  // Read game sender variables
  unsigned int gameSenderSleepIntervalMilliseconds = 0;
  valueReadResult = GetConfigValue(
      configData, sdkConfigFieldNames.gameSenderSleepIntervalMilliseconds,
      gameSenderSleepIntervalMilliseconds);
  valueReadResult &= Validator::ValidateItemValue(
      gameSenderSleepIntervalMilliseconds, 0U, 5000U);
  if (valueReadResult) {
    sdkConfig.gameSenderSleepIntervalMilliseconds =
        gameSenderSleepIntervalMilliseconds;
  }
  unsigned int apiTimeoutMilliseconds = 0;
  valueReadResult =
      GetConfigValue(configData, sdkConfigFieldNames.apiTimeoutMilliseconds,
                     apiTimeoutMilliseconds);
  valueReadResult &=
      Validator::ValidateItemValue(apiTimeoutMilliseconds, 0U, 20000U);
  if (valueReadResult) {
    sdkConfig.apiTimeoutMilliseconds = apiTimeoutMilliseconds;
  }
  unsigned int apiWaitTimeMilliseconds = 0;
  valueReadResult =
      GetConfigValue(configData, sdkConfigFieldNames.apiWaitTimeMilliseconds,
                     apiWaitTimeMilliseconds);
  valueReadResult &=
      Validator::ValidateItemValue(apiWaitTimeMilliseconds, 0U, 20000U);
  if (valueReadResult) {
    sdkConfig.apiWaitTimeMilliseconds = apiWaitTimeMilliseconds;
  }
  unsigned int packetMaxSizeInBytes = 0;
  valueReadResult =
      GetConfigValue(configData, sdkConfigFieldNames.packetMaxSizeInBytes,
                     packetMaxSizeInBytes);
  valueReadResult &=
      Validator::ValidateItemValue(packetMaxSizeInBytes, 0U, 2000000U);
  if (valueReadResult) {
    sdkConfig.packetMaxSizeInBytes = packetMaxSizeInBytes;
  }

  // Read action buffer variables
  unsigned int actionsBufferMaxSizeInBytes = 0;
  valueReadResult = GetConfigValue(
      configData, sdkConfigFieldNames.actionsBufferMaxSizeInBytes,
      actionsBufferMaxSizeInBytes);
  valueReadResult &= Validator::ValidateItemValue(actionsBufferMaxSizeInBytes,
                                                  500U, 100000000U);
  if (valueReadResult) {
    sdkConfig.actionsBufferMaxSizeInBytes = actionsBufferMaxSizeInBytes;
  }

  // Read game container variables
  unsigned int gameContainerMaxSizeInBytes = 0;
  valueReadResult = GetConfigValue(
      configData, sdkConfigFieldNames.gameContainerMaxSizeInBytes,
      gameContainerMaxSizeInBytes);
  valueReadResult &= Validator::ValidateItemValue(gameContainerMaxSizeInBytes,
                                                  500U, 500000000U);
  if (valueReadResult) {
    sdkConfig.gameContainerMaxSizeInBytes = gameContainerMaxSizeInBytes;
  }
  unsigned int maxGames = 0;
  valueReadResult =
      GetConfigValue(configData, sdkConfigFieldNames.maxGames, maxGames);
  valueReadResult &= Validator::ValidateItemValue(maxGames, 1U, 100U);
  if (valueReadResult) {
    sdkConfig.maxGames = maxGames;
  }
  unsigned int maxMatchesPerGame = 0;
  valueReadResult = GetConfigValue(
      configData, sdkConfigFieldNames.maxMatchesPerGame, maxMatchesPerGame);
  valueReadResult &= Validator::ValidateItemValue(maxMatchesPerGame, 1U, 100U);
  if (valueReadResult) {
    sdkConfig.maxMatchesPerGame = maxMatchesPerGame;
  }
  unsigned int minPacketSizeForSendingInBytes = 0;
  valueReadResult = GetConfigValue(
      configData, sdkConfigFieldNames.minPacketSizeForSendingInBytes,
      minPacketSizeForSendingInBytes);
  valueReadResult &= Validator::ValidateItemValue(
      minPacketSizeForSendingInBytes, 500U, 1500000U);
  if (valueReadResult) {
    sdkConfig.minPacketSizeForSendingInBytes = minPacketSizeForSendingInBytes;
  }
  unsigned int packetTimeoutInMilliseconds = 0;
  valueReadResult = GetConfigValue(
      configData, sdkConfigFieldNames.packetTimeoutInMilliseconds,
      packetTimeoutInMilliseconds);
  valueReadResult &=
      Validator::ValidateItemValue(packetTimeoutInMilliseconds, 500U, 100000U);
  if (valueReadResult) {
    sdkConfig.packetTimeoutInMilliseconds = packetTimeoutInMilliseconds;
  }
  unsigned int gameCloseGraceAfterMarkEndInMilliseconds = 0;
  valueReadResult = GetConfigValue(
      configData, sdkConfigFieldNames.gameCloseGraceAfterMarkEndInMilliseconds,
      gameCloseGraceAfterMarkEndInMilliseconds);
  valueReadResult &= Validator::ValidateItemValue(
      gameCloseGraceAfterMarkEndInMilliseconds, 0U, 200000U);
  if (valueReadResult) {
    sdkConfig.gameCloseGraceAfterMarkEndInMilliseconds =
        gameCloseGraceAfterMarkEndInMilliseconds;
  }
  unsigned int liveGameTimeoutInMilliseconds = 0;
  valueReadResult = GetConfigValue(
      configData, sdkConfigFieldNames.liveGameTimeoutInMilliseconds,
      liveGameTimeoutInMilliseconds);
  valueReadResult &=
      Validator::ValidateItemValue(liveGameTimeoutInMilliseconds, 0U, 300000U);
  if (valueReadResult) {
    sdkConfig.liveGameTimeoutInMilliseconds = liveGameTimeoutInMilliseconds;
  }

  // Read hyper mode variables
  GetConfigValue(configData, sdkConfigFieldNames.hyperModeFeatureEnabled,
                 sdkConfig.hyperModeFeatureEnabled);

  unsigned int hyperModeMaxThreads = 0;
  valueReadResult = GetConfigValue(
      configData, sdkConfigFieldNames.hyperModeMaxThreads, hyperModeMaxThreads);
  valueReadResult &= Validator::ValidateItemValue(hyperModeMaxThreads, 1U, 20U);
  if (valueReadResult) {
    sdkConfig.hyperModeMaxThreads = hyperModeMaxThreads;
  }
  unsigned int hyperModeAtBufferPercentage = 0;
  valueReadResult = GetConfigValue(
      configData, sdkConfigFieldNames.hyperModeAtBufferPercentage,
      hyperModeAtBufferPercentage);
  valueReadResult &=
      Validator::ValidateItemValue(hyperModeAtBufferPercentage, 10U, 90U);
  if (valueReadResult) {
    sdkConfig.hyperModeAtBufferPercentage = hyperModeAtBufferPercentage;
  }
  unsigned int hyperModeUpperPercentageBound = 0;
  valueReadResult = GetConfigValue(
      configData, sdkConfigFieldNames.hyperModeUpperPercentageBound,
      hyperModeUpperPercentageBound);
  valueReadResult &=
      Validator::ValidateItemValue(hyperModeUpperPercentageBound, 30U, 90U);
  valueReadResult &=
      sdkConfig.hyperModeAtBufferPercentage < hyperModeUpperPercentageBound;
  if (valueReadResult) {
    sdkConfig.hyperModeUpperPercentageBound = hyperModeUpperPercentageBound;
  }
  unsigned int hyperModeThreadCreationStaggerMilliseconds = 0;
  valueReadResult = GetConfigValue(
      configData,
      sdkConfigFieldNames.hyperModeThreadCreationStaggerMilliseconds,
      hyperModeThreadCreationStaggerMilliseconds);
  valueReadResult &= Validator::ValidateItemValue(
      hyperModeThreadCreationStaggerMilliseconds, 0U, 10000U);
  if (valueReadResult) {
    sdkConfig.hyperModeThreadCreationStaggerMilliseconds =
        hyperModeThreadCreationStaggerMilliseconds;
  }

  GetConfigValue(configData, sdkConfigFieldNames.logLevel, sdkConfig.logLevel);

  return true;
}

/**
 * ReadUserConfigFile:
 *
 * Read all config values from the file
 **/
std::map<std::string, std::string> Config::ReadUserConfigFile() {
  std::map<std::string, std::string> output_map;

  char* configPathHolder = std::getenv("GETGUD_CONFIG_PATH");
  if (configPathHolder == nullptr) {
    logger.Log(LogType::DEBUG, std::string("Config::LoadSettings->Environment "
                                           "variable GETGUD_CONFIG_PATH is empty"));
  }
  else
  {
    configFilePath = std::string(configPathHolder);
  }

  logger.Log(LogType::DEBUG,
             std::string("Loading config file from " + configFilePath + " path"));

  std::string next_key;
  std::string next_value;

  bool key_started = false;
  bool key_ended = false;
  bool value_started = false;

  std::ifstream file_stream(configFilePath);
  char next_character = 0;

  if (file_stream.is_open()) {
    while (file_stream.good()) {
      file_stream.get(next_character);

      if (next_character == '\t')
        continue;

      if (key_started && next_character != '\"' && next_character != ',' &&
          next_character != '\n') {
        next_key += next_character;
      } else if (value_started && next_character != ' ' &&
                 next_character != '\"' && next_character != ',' &&
                 next_character != '\n' && next_character != '}') {
        next_value += next_character;
      }

      if (next_character == '\"' || next_character == ',' ||
          (next_character == '\n' && key_ended) ||
          (next_character == ':' && key_ended && !value_started)) {
        if (value_started && next_character == '\"')
          continue;

        if (key_ended && !value_started) {
          value_started = true;
        } else if (key_ended && value_started) {
          value_started = false;
        } else if (!key_ended && !key_started) {
          key_started = true;
        } else if (!key_ended && key_started) {
          key_started = false;
          key_ended = true;
        }
      }
      if ((next_character == ',' || next_character == '}') && key_ended) {
        output_map.insert(std::make_pair(next_key, next_value));
        next_key.clear();
        next_value.clear();
        key_started = false;
        key_ended = false;
        value_started = false;
      }
    }
  }

  file_stream.close();

  return output_map;
}

/**
 * GetConfigValue:
 *
 **/

bool Config::GetConfigValue(std::map<std::string, std::string>& configData,
                            std::string configKey,
                            unsigned int& outValue) {
  auto configIter = configData.find(configKey);
  if (configIter != configData.end() && !configIter->second.empty()) {
    outValue = std::stoi(configIter->second);
    return true;
  }

  return false;
}

/**
 * GetConfigValue:
 *
 **/
bool Config::GetConfigValue(std::map<std::string, std::string>& configData,
                            std::string configKey,
                            std::string& outValue) {
  auto configIter = configData.find(configKey);
  if (configIter != configData.end()) {
    outValue = configIter->second;
    return true;
  }
  return false;
}

/**
 * GetConfigValue:
 *
 **/
bool Config::GetConfigValue(std::map<std::string, std::string>& configData,
                            std::string configKey,
                            bool& outValue) {
  auto configIter = configData.find(configKey);
  if (configIter != configData.end() && !configIter->second.empty()) {
    if (configIter->second == "true") {
      outValue = true;
      return true;
    } else if (configIter->second == "false") {
      outValue = false;
      return true;
    }
  }
  return false;
}

/**
 * GetConfigValue:
 *
 **/
bool Config::GetConfigValue(std::map<std::string, std::string>& configData,
                            std::string configKey,
                            LogLevel& outValue) {
  std::string configValue;

  auto configIter = configData.find(configKey);
  if (configIter != configData.end())
    configValue = configIter->second;

  if (!configValue.empty()) {
    if (configValue == "FATAL") {
      outValue = LogLevel::FATAL;
    } else if (configValue == "_ERROR") {
      outValue = LogLevel::_ERROR;
    } else if (configValue == "WARN_AND_ERROR") {
      outValue = LogLevel::WARN_AND_ERROR;
    } else if (configValue == "FULL") {
      outValue = LogLevel::FULL;
    }
    return true;
  }
  return false;
}

std::string Config::ToString() {
  std::string configString;

  SdkConfigFieldNames fieldNames = sdkConfig.sdkConfigFieldNames;

  // Adding all parameters to the configString
  configString +=
      "\t" + fieldNames.streamGameURL + ": " + sdkConfig.streamGameURL + ",\n";
  configString += "\t" + fieldNames.updatePlayersURL + ": " +
                  sdkConfig.updatePlayersURL + ",\n";
  configString += "\t" + fieldNames.sendReportsURL + ": " +
                  sdkConfig.sendReportsURL + ",\n";
  configString += "\t" + fieldNames.throttleCheckUrl + ": " +
                  sdkConfig.throttleCheckUrl + ",\n";

  configString += "\t" + fieldNames.logToFile + ": " +
                  std::to_string(sdkConfig.logToFile) + ",\n";
  configString += "\t" + fieldNames.logFileSizeInBytes + ": " +
                  std::to_string(sdkConfig.logFileSizeInBytes) + ",\n";

  configString += "\t" + fieldNames.circularLogFile + ": " +
                  std::to_string(sdkConfig.circularLogFile) + ",\n";
  
  configString += "\t" + fieldNames.reportsMaxBufferSizeInBytes + ": " +
                  std::to_string(sdkConfig.reportsMaxBufferSizeInBytes) + ",\n";
  configString += "\t" + fieldNames.maxReportsToSendAtOnce + ": " +
                  std::to_string(sdkConfig.maxReportsToSendAtOnce) + ",\n";

  configString += "\t" + fieldNames.playersMaxBufferSizeInBytes + ": " +
                  std::to_string(sdkConfig.playersMaxBufferSizeInBytes) + ",\n";
  configString += "\t" + fieldNames.maxPlayerUpdatesToSendAtOnce + ": " +
                  std::to_string(sdkConfig.maxPlayerUpdatesToSendAtOnce) +
                  ",\n";

  configString += "\t" + fieldNames.maxChatMessagesToSendAtOnce + ": " +
                  std::to_string(sdkConfig.maxChatMessagesToSendAtOnce) + ",\n";

  configString +=
      "\t" + fieldNames.gameSenderSleepIntervalMilliseconds + ": " +
      std::to_string(sdkConfig.gameSenderSleepIntervalMilliseconds) + ",\n";
  configString += "\t" + fieldNames.apiTimeoutMilliseconds + ": " +
                  std::to_string(sdkConfig.apiTimeoutMilliseconds) + ",\n";
  configString += "\t" + fieldNames.apiWaitTimeMilliseconds + ": " +
                  std::to_string(sdkConfig.apiWaitTimeMilliseconds) + ",\n";
  configString += "\t" + fieldNames.packetMaxSizeInBytes + ": " +
                  std::to_string(sdkConfig.packetMaxSizeInBytes) + ",\n";

  configString += "\t" + fieldNames.actionsBufferMaxSizeInBytes + ": " +
                  std::to_string(sdkConfig.actionsBufferMaxSizeInBytes) + ",\n";

  configString += "\t" + fieldNames.gameContainerMaxSizeInBytes + ": " +
                  std::to_string(sdkConfig.gameContainerMaxSizeInBytes) + ",\n";
  configString += "\t" + fieldNames.maxGames + ": " +
                  std::to_string(sdkConfig.maxGames) + ",\n";
  configString += "\t" + fieldNames.maxMatchesPerGame + ": " +
                  std::to_string(sdkConfig.maxMatchesPerGame) + ",\n";
  configString += "\t" + fieldNames.minPacketSizeForSendingInBytes + ": " +
                  std::to_string(sdkConfig.minPacketSizeForSendingInBytes) +
                  ",\n";
  configString += "\t" + fieldNames.packetTimeoutInMilliseconds + ": " +
                  std::to_string(sdkConfig.packetTimeoutInMilliseconds) + ",\n";
  configString +=
      "\t" + fieldNames.gameCloseGraceAfterMarkEndInMilliseconds + ": " +
      std::to_string(sdkConfig.gameCloseGraceAfterMarkEndInMilliseconds) +
      ",\n";
  configString += "\t" + fieldNames.liveGameTimeoutInMilliseconds + ": " +
                  std::to_string(sdkConfig.liveGameTimeoutInMilliseconds) +
                  ",\n";

  configString += "\t" + fieldNames.hyperModeFeatureEnabled + ": " +
                  std::to_string(sdkConfig.hyperModeFeatureEnabled) + ",\n";
  configString += "\t" + fieldNames.hyperModeMaxThreads + ": " +
                  std::to_string(sdkConfig.hyperModeMaxThreads) + ",\n";
  configString += "\t" + fieldNames.hyperModeAtBufferPercentage + ": " +
                  std::to_string(sdkConfig.hyperModeAtBufferPercentage) + ",\n";
  configString += "\t" + fieldNames.hyperModeUpperPercentageBound + ": " +
                  std::to_string(sdkConfig.hyperModeUpperPercentageBound) +
                  ",\n";
  configString +=
      "\t" + fieldNames.hyperModeThreadCreationStaggerMilliseconds + ": " +
      std::to_string(sdkConfig.hyperModeThreadCreationStaggerMilliseconds) +
      "\n";

  return configString;
}

}  // namespace GetgudSDK