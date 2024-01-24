#pragma once
#include <cstddef>
#include <limits>
#include <map>
#include <string>
#include "../../include/actions/Helpers.h"
#include "../utils/Validator.h"

namespace GetgudSDK {

/**
 * SdkValidatorConfig:
 *
 * Some constants for parameter validation
 **/
struct SdkValidatorConfig {
  long long minActionTimeEpochTime = 1000000000000;
  long long maxActionTimeEpochTime = 2000000000000;
};

/**
 * SdkConfigFieldNames:
 *
 * Define names of the fields in SDK, so we can easily change names
 * in case we want a better name for a variable
 **/
struct SdkConfigFieldNames {
  std::string streamGameURL = "streamGameURL";
  std::string updatePlayersURL = "updatePlayersURL";
  std::string sendReportsURL = "sendReportsURL";
  std::string throttleCheckUrl = "throttleCheckUrl";

  //Logger fields
  std::string logToFile = "logToFile";
  std::string logFileSizeInBytes = "logFileSizeInBytes";
  std::string linesDeletionAmount = "linesDeletionAmount";
  std::string circularLogFile = "circularLogFile";
  std::string logType = "logType";
  std::string logLevel = "logLevel";

  // Report sender fields
  std::string reportsMaxBufferSizeInBytes = "reportsMaxBufferSizeInBytes";
  std::string maxReportsToSendAtOnce = "maxReportsToSendAtOnce";

  // Player updater fields
  std::string playersMaxBufferSizeInBytes = "playersMaxBufferSizeInBytes";
  std::string maxPlayerUpdatesToSendAtOnce = "maxPlayerUpdatesToSendAtOnce";

  // Match chat fields
  std::string maxChatMessagesToSendAtOnce = "maxChatMessagesToSendAtOnce";

  // Game sender fields
  std::string gameSenderSleepIntervalMilliseconds =
      "gameSenderSleepIntervalMilliseconds";
  std::string apiTimeoutMilliseconds = "apiTimeoutMilliseconds";
  std::string apiWaitTimeMilliseconds = "apiWaitTimeMilliseconds";
  std::string packetMaxSizeInBytes = "packetMaxSizeInBytes";

  // Action buffer fields
  std::string actionsBufferMaxSizeInBytes = "actionsBufferMaxSizeInBytes";

  // Game container fields
  std::string gameContainerMaxSizeInBytes = "gameContainerMaxSizeInBytes";
  std::string maxGames = "maxGames";
  std::string maxMatchesPerGame = "maxMatchesPerGame";
  std::string minPacketSizeForSendingInBytes = "minPacketSizeForSendingInBytes";
  std::string packetTimeoutInMilliseconds = "packetTimeoutInMilliseconds";
  std::string gameCloseGraceAfterMarkEndInMilliseconds =
      "gameCloseGraceAfterMarkEndInMilliseconds";
  std::string liveGameTimeoutInMilliseconds = "liveGameTimeoutInMilliseconds";

  // Hyper mode fields
  std::string hyperModeFeatureEnabled = "hyperModeFeatureEnabled";
  std::string hyperModeMaxThreads = "hyperModeMaxThreads";
  std::string hyperModeAtBufferPercentage = "hyperModeAtBufferPercentage";
  std::string hyperModeUpperPercentageBound = "hyperModeUpperPercentageBound";
  std::string hyperModeThreadCreationStaggerMilliseconds =
      "hyperModeThreadCreationStaggerMilliseconds";
};

/**
 * Config:
 *
 **/
class Config {
#ifdef _DEBUG
 public:
  long long actionsAmount = 0;
  long long totalCreatedActions = 0;
  long long emptyActionsAmount = 0;
  long long totalCreatedEmptyActions = 0;
  long long totalReportsSent = 0;
  long long totalChatSent = 0;
#endif

 public:
  std::string streamGameURL;
  std::string updatePlayersURL;
  std::string sendReportsURL;
  std::string throttleCheckUrl;

  const SdkValidatorConfig sdkValidatorConfig;
  const SdkConfigFieldNames sdkConfigFieldNames;

  // Logger default values
  bool logToFile = false;
  unsigned int logFileSizeInBytes = 10000000;
  unsigned int linesDeletionAmount = 30;
  bool circularLogFile = true;
  std::string logsFilePath = "./logs.txt";
  LogLevel logLevel = LogLevel::FULL;

  // Report sender default values
  unsigned int reportsMaxBufferSizeInBytes = 1000000;
  unsigned int maxReportsToSendAtOnce = 100;

  // Player updater default values
  unsigned int playersMaxBufferSizeInBytes = 1000000;
  unsigned int maxPlayerUpdatesToSendAtOnce = 100;

  // Chat messages default values
  unsigned int maxChatMessagesToSendAtOnce = 100;

  // Game sender default values
  unsigned int gameSenderSleepIntervalMilliseconds = 1000;
  unsigned int apiTimeoutMilliseconds = 1000;
  long long apiWaitTimeMilliseconds = 1000;
  unsigned int packetMaxSizeInBytes = 1500000;

  // Action buffer default values
  unsigned int actionsBufferMaxSizeInBytes = 2000000;

  // Game container default values
  unsigned int gameContainerMaxSizeInBytes = 2000000;
  unsigned int maxGames = 10;
  unsigned int maxMatchesPerGame = 50;
  unsigned int minPacketSizeForSendingInBytes = 1000000;
  unsigned int packetTimeoutInMilliseconds = 10000;
  unsigned int gameCloseGraceAfterMarkEndInMilliseconds = 1000;
  unsigned int liveGameTimeoutInMilliseconds = 60000;

  // Hyper mode default values
  bool hyperModeFeatureEnabled = false;
  unsigned int hyperModeMaxThreads = 5;
  unsigned int hyperModeAtBufferPercentage = 10;
  unsigned int hyperModeUpperPercentageBound = 90;
  unsigned int hyperModeThreadCreationStaggerMilliseconds = 500;

  // Other sdk default values
  unsigned int bufferAvgSizeCalcDelayMilliseconds = 1000;
  std::string configFilePath = "./config.json";

  bool LoadSettings();
  std::string ToString();

 private:
  std::map<std::string, std::string> ReadUserConfigFile();

 bool GetConfigValue(std::map<std::string, std::string>& configData,
                          std::string configKey, unsigned int& outValue);

 bool GetConfigValue(
      std::map<std::string, std::string>& configData,
      std::string configKey, std::string& outValue);

bool GetConfigValue(std::map<std::string, std::string>& configData,
                            std::string configKey, bool& outValue);

bool GetConfigValue(
      std::map<std::string, std::string>& configData,
      std::string configKey, LogLevel& outValue);
};
}  // namespace GetgudSDK