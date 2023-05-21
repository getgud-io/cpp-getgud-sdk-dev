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

namespace GetGudSdk {
Config sdkConfig;
extern Logger logger;

/**
 * LoadSettings:
 *
 * Get values of variables from the config file
 **/
void Config::LoadSettings() {
  char* logsFilePathHolder = std::getenv("LOG_FILE_PATH");
  if (logsFilePathHolder == nullptr) {
    logger.Log(
        LogType::FATAL,
        std::string("Error: environment variable LOG_FILE_PATH is required"));
    return;
  }
  std::string _logsFilePath(logsFilePathHolder);
  logsFilePath = _logsFilePath;

  // TODO: check what will the values be if config path is wrong
  std::map<std::string, std::string> configData = ReadUserConfigFile();

  // Read all Getgud API URLs
  sdkConfig.streamGameURL = GetConfigValue<std::string>(
      configData, sdkConfigFieldNames.streamGameURL);
  sdkConfig.updatePlayersURL = GetConfigValue<std::string>(
      configData, sdkConfigFieldNames.updatePlayersURL);
  sdkConfig.sendReportsURL = GetConfigValue<std::string>(
      configData, sdkConfigFieldNames.sendReportsURL);
  sdkConfig.throttleCheckUrl = GetConfigValue<std::string>(
      configData, sdkConfigFieldNames.throttleCheckUrl);

  // Read report sender variables
  unsigned int reportsMaxBufferSizeInBytes = GetConfigValue<int>(
      configData, sdkConfigFieldNames.reportsMaxBufferSizeInBytes);
  if (Validator::ValidateItemValue(reportsMaxBufferSizeInBytes, 0U,
                                   10000000U)) {
    sdkConfig.reportsMaxBufferSizeInBytes = reportsMaxBufferSizeInBytes;
  }

  unsigned int maxReportsToSendAtOnce = GetConfigValue<int>(
      configData, sdkConfigFieldNames.maxReportsToSendAtOnce);
  if (Validator::ValidateItemValue(maxReportsToSendAtOnce, 0U, 100U)) {
    sdkConfig.maxReportsToSendAtOnce = maxReportsToSendAtOnce;
  }

  // Read player updeter variables
  unsigned int playersMaxBufferSizeInBytes = GetConfigValue<int>(
      configData, sdkConfigFieldNames.playersMaxBufferSizeInBytes);
  if (Validator::ValidateItemValue(playersMaxBufferSizeInBytes, 0U,
                                   10000000U)) {
    sdkConfig.playersMaxBufferSizeInBytes = playersMaxBufferSizeInBytes;
  }

  unsigned int maxPlayerUpdatesToSendAtOnce = GetConfigValue<int>(
      configData, sdkConfigFieldNames.maxPlayerUpdatesToSendAtOnce);
  if (Validator::ValidateItemValue(maxPlayerUpdatesToSendAtOnce, 0U, 100U)) {
    sdkConfig.maxPlayerUpdatesToSendAtOnce = maxPlayerUpdatesToSendAtOnce;
  }

  // Read game sender variables
  unsigned int gameSenderSleepIntervalMilliseconds = GetConfigValue<int>(
      configData, sdkConfigFieldNames.gameSenderSleepIntervalMilliseconds);
  if (Validator::ValidateItemValue(gameSenderSleepIntervalMilliseconds, 0U,
                                   5000U)) {
    sdkConfig.gameSenderSleepIntervalMilliseconds =
        gameSenderSleepIntervalMilliseconds;
  }
  unsigned int apiTimeoutMilliseconds = GetConfigValue<int>(
      configData, sdkConfigFieldNames.apiTimeoutMilliseconds);
  if (Validator::ValidateItemValue(apiTimeoutMilliseconds, 0U, 20000U)) {
    sdkConfig.apiTimeoutMilliseconds = apiTimeoutMilliseconds;
  }
  unsigned int apiWaitTimeMilliseconds = GetConfigValue<int>(
      configData, sdkConfigFieldNames.apiWaitTimeMilliseconds);
  if (Validator::ValidateItemValue(apiWaitTimeMilliseconds, 0U, 20000U)) {
    sdkConfig.apiWaitTimeMilliseconds = apiWaitTimeMilliseconds;
  }
  unsigned int packetMaxSizeInBytes =
      GetConfigValue<int>(configData, sdkConfigFieldNames.packetMaxSizeInBytes);
  if (Validator::ValidateItemValue(packetMaxSizeInBytes, 0U, 2000000U)) {
    sdkConfig.packetMaxSizeInBytes = packetMaxSizeInBytes;
  }

  // Read action buffer variables
  unsigned int actionsBufferMaxSizeInBytes = GetConfigValue<int>(
      configData, sdkConfigFieldNames.actionsBufferMaxSizeInBytes);
  if (Validator::ValidateItemValue(actionsBufferMaxSizeInBytes, 500U,
                                   100000000U)) {
    sdkConfig.actionsBufferMaxSizeInBytes = actionsBufferMaxSizeInBytes;
  }

  // Read game container variables
  unsigned int gameContainerMaxSizeInBytes = GetConfigValue<int>(
      configData, sdkConfigFieldNames.gameContainerMaxSizeInBytes);
  if (Validator::ValidateItemValue(gameContainerMaxSizeInBytes, 500U,
                                   500000000U)) {
    sdkConfig.gameContainerMaxSizeInBytes = gameContainerMaxSizeInBytes;
  }
  unsigned int maxGames =
      GetConfigValue<int>(configData, sdkConfigFieldNames.maxGames);
  if (Validator::ValidateItemValue(maxGames, 1U, 100U)) {
    sdkConfig.maxGames = maxGames;
  }
  unsigned int maxMatchesPerGame =
      GetConfigValue<int>(configData, sdkConfigFieldNames.maxMatchesPerGame);
  if (Validator::ValidateItemValue(maxMatchesPerGame, 1U, 100U)) {
    sdkConfig.maxMatchesPerGame = maxMatchesPerGame;
  }
  unsigned int minPacketSizeForSendingInBytes = GetConfigValue<int>(
      configData, sdkConfigFieldNames.minPacketSizeForSendingInBytes);
  if (Validator::ValidateItemValue(minPacketSizeForSendingInBytes, 500U,
                                   1500000U)) {
    sdkConfig.minPacketSizeForSendingInBytes = minPacketSizeForSendingInBytes;
  }
  unsigned int packetTimeoutInMilliseconds = GetConfigValue<int>(
      configData, sdkConfigFieldNames.packetTimeoutInMilliseconds);
  if (Validator::ValidateItemValue(packetTimeoutInMilliseconds, 500U, 10000U)) {
    sdkConfig.packetTimeoutInMilliseconds = packetTimeoutInMilliseconds;
  }
  unsigned int gameCloseGraceAfterMarkEndInMilliseconds = GetConfigValue<int>(
      configData, sdkConfigFieldNames.gameCloseGraceAfterMarkEndInMilliseconds);
  if (Validator::ValidateItemValue(gameCloseGraceAfterMarkEndInMilliseconds, 0U,
                                   30000U)) {
    sdkConfig.gameCloseGraceAfterMarkEndInMilliseconds =
        gameCloseGraceAfterMarkEndInMilliseconds;
  }
  unsigned int liveGameTimeoutInMilliseconds = GetConfigValue<int>(
      configData, sdkConfigFieldNames.liveGameTimeoutInMilliseconds);
  if (Validator::ValidateItemValue(liveGameTimeoutInMilliseconds, 0U,
                                   300000U)) {
    sdkConfig.liveGameTimeoutInMilliseconds = liveGameTimeoutInMilliseconds;
  }

  // Read hyper mode variables
  bool hyperModeFeatureEnabled = GetConfigValue<bool>(
      configData, sdkConfigFieldNames.hyperModeFeatureEnabled);
  sdkConfig.hyperModeFeatureEnabled = hyperModeFeatureEnabled;

  unsigned int hyperModeMaxThreads =
      GetConfigValue<int>(configData, sdkConfigFieldNames.hyperModeMaxThreads);
  if (Validator::ValidateItemValue(hyperModeMaxThreads, 1U, 20U)) {
    sdkConfig.hyperModeMaxThreads = hyperModeMaxThreads;
  }
  unsigned int hyperModeAtBufferPercentage = GetConfigValue<int>(
      configData, sdkConfigFieldNames.hyperModeAtBufferPercentage);
  if (Validator::ValidateItemValue(hyperModeAtBufferPercentage, 10U, 90U)) {
    sdkConfig.hyperModeAtBufferPercentage = hyperModeAtBufferPercentage;
  }
  unsigned int hyperModeUpperPercentageBound = GetConfigValue<int>(
      configData, sdkConfigFieldNames.hyperModeUpperPercentageBound);
  if (Validator::ValidateItemValue(hyperModeUpperPercentageBound, 30U, 90U) &&
      (sdkConfig.hyperModeAtBufferPercentage < hyperModeUpperPercentageBound)) {
    sdkConfig.hyperModeUpperPercentageBound = hyperModeUpperPercentageBound;
  }
  unsigned int hyperModeThreadCreationStaggerMilliseconds = GetConfigValue<int>(
      configData,
      sdkConfigFieldNames.hyperModeThreadCreationStaggerMilliseconds);
  if (Validator::ValidateItemValue(hyperModeThreadCreationStaggerMilliseconds,
                                   0U, 10000U)) {
    sdkConfig.hyperModeThreadCreationStaggerMilliseconds =
        hyperModeThreadCreationStaggerMilliseconds;
  }

  // Read other variables
  LogLevel logLevel =
      GetConfigValue<LogLevel>(configData, sdkConfigFieldNames.logLevel);
  sdkConfig.logLevel = logLevel;
}

/**
 * ReadUserConfigFile:
 *
 * Read all config values from the file
 **/
std::map<std::string, std::string> Config::ReadUserConfigFile() {
  std::map<std::string, std::string> output_map;

  char* configPathHolder = std::getenv("CONFIG_PATH");
  if (configPathHolder == nullptr) {
    logger.Log(
        LogType::FATAL,
        std::string("Error: environment variable CONFIG_PATH is required"));
    return output_map;
  }
  std::string configPath(configPathHolder);

  std::string next_key;
  std::string next_value;

  bool key_started = false;
  bool key_ended = false;
  bool value_started = false;

  std::ifstream file_stream(configPath);
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

// TODO will be crash here, because int can be pushed here
// as well as any struct or class
// TODO output value as a parameter is better
template <>
int Config::GetConfigValue<int>(std::map<std::string, std::string>& configData,
                                std::string configKey) {
  int configValueOut;
  std::string configValue;

  auto configIter = configData.find(configKey);
  if (configIter != configData.end())
    configValue = configIter->second;

  if (!configValue.empty())
    configValueOut = std::stoi(configValue);
  else
    configValueOut = 0;

  return configValueOut;
}

/**
 * GetConfigValue:
 *
 **/
template <>
std::string Config::GetConfigValue<std::string>(
    std::map<std::string, std::string>& configData,
    std::string configKey) {
  std::string configValue;

  auto configIter = configData.find(configKey);
  if (configIter != configData.end())
    configValue = configIter->second;

  return configValue;
}

/**
 * GetConfigValue:
 *
 **/
template <>
bool Config::GetConfigValue<bool>(
    std::map<std::string, std::string>& configData,
    std::string configKey) {
  bool configValueOut;
  std::string configValue;

  auto configIter = configData.find(configKey);
  if (configIter != configData.end())
    configValue = configIter->second;

  if (!configValue.empty()) {
    if (configValue == "true") {
      configValueOut = true;
    } else if (configValue == "false") {
      configValueOut = false;
    } else {
      throw std::runtime_error("Invalid boolean value for " + configKey);
    }
  } else {
    configValueOut = false;
  }

  return configValueOut;
}

/**
 * GetConfigValue:
 *
 **/
template <>
LogLevel Config::GetConfigValue<LogLevel>(
    std::map<std::string, std::string>& configData,
    std::string configKey) {
  LogLevel configValueOut;
  std::string configValue;

  auto configIter = configData.find(configKey);
  if (configIter != configData.end())
    configValue = configIter->second;

  if (!configValue.empty()) {
    if (configValue == "FATAL") {
      configValueOut = LogLevel::FATAL;
    } else if (configValue == "_ERROR") {
      configValueOut = LogLevel::_ERROR;
    } else if (configValue == "WARN_AND_ERROR") {
      configValueOut = LogLevel::WARN_AND_ERROR;
    } else if (configValue == "FULL") {
      configValueOut = LogLevel::FULL;
    } else {
      configValueOut = LogLevel::FULL;
    }
  } else {
    configValueOut = LogLevel::FULL;
  }

  return configValueOut;
}

}  // namespace GetGudSdk