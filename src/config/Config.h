#pragma once
#include <cstddef>
#include <limits>
#include <map>
#include <string>
#include "../../include/actions/Helpers.h"
#include "../utils/Validator.h"

namespace GetGudSdk {

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
 * SdkApiFieldNames:
 *
 * Define all field names that can be used in JSON body of all API requests
 * sent to Getgud
 * TODO: check if we need those
 **/
struct SdkApiFieldNames {
  const std::string privateKey = "privateKey";
  const std::string titleId = "titleId";
  const std::string gameGuid = "gameGuid";
  const std::string gameMode = "gameMode";
  const std::string gameLastPacket = "gameLastPacket";
  struct Match {
    const std::string matchGuid = "matchGuid";
    const std::string mapName = "mapName";
    const std::string matchMode = "matchMode";
    const std::string matchActionStream = "matchActionStream";
  } match;
  struct Report {
    const std::string matchGuid = "matchGuid";
    const std::string reporterName = "reporterName";
    const std::string reporterType = "reporterType";
    const std::string reporterSubType = "reporterSubType";
    const std::string suspectedPlayerGuid = "suspectedPlayerGuid";
    const std::string tbType = "tbType";
    const std::string tbSubType = "tbSubType";
    const std::string tbTimeEpoch = "tbTimeEpoch";
    const std::string suggestedToxicityScore = "suggestedToxicityScore";
    const std::string reportedTimeEpoch = "reportedTimeEpoch";
  } report;
  struct Message {
    const std::string playerGuid = "playerGuid";
    const std::string messageTimeEpoch = "messageTimeEpoch";
    const std::string message = "message";
  } message;
  struct PlayerUpdate {
    const std::string playerName = "playerName";
    const std::string playerNickname = "playerNickname";
    const std::string playerEmail = "playerEmail";
    const std::string playerRank = "playerRank";
    const std::string playerJoinDateEpoch = "playerJoinDateEpoch";
  };
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

  // Report sender fields
  std::string reportsMaxBufferSizeInBytes = "reportsMaxBufferSizeInBytes";
  std::string maxReportsToSendAtOnce = "maxReportsToSendAtOnce";

  // Player updater fields
  std::string playersMaxBufferSizeInBytes = "playersMaxBufferSizeInBytes";
  std::string maxPlayerUpdatesToSendAtOnce = "maxPlayerUpdatesToSendAtOnce";

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

  // Other sdk fields
  std::string logType = "logType";
  std::string logLevel = "logLevel";
};

/**
 * Config:
 *
 **/
class Config {
#ifdef _DEBUG
 public:
  int actionsAmount = 0;
  int totalCreatedActions = 0;
  int emptyActionsAmount = 0;
  int totalCreatedEmptyActions = 0;
#endif

 public:
  std::string streamGameURL;
  std::string updatePlayersURL;
  std::string sendReportsURL;
  std::string throttleCheckUrl;

  const SdkValidatorConfig sdkValidatorConfig;
  const SdkApiFieldNames sdkApiFieldNames;
  const SdkConfigFieldNames sdkConfigFieldNames;

  // Report sender default values
  unsigned int reportsMaxBufferSizeInBytes = 1000000;
  unsigned int maxReportsToSendAtOnce = 100;

  // Player updater default values
  unsigned int playersMaxBufferSizeInBytes = 1000000;
  unsigned int maxPlayerUpdatesToSendAtOnce = 100;

  // Game sender default values
  unsigned int gameSenderSleepIntervalMilliseconds = 1000;
  unsigned int apiTimeoutMilliseconds = 1000;
  long long apiWaitTimeMilliseconds = 1000;
  unsigned int packetMaxSizeInBytes = 1500000;
  unsigned int bufferAvgSizeCalcDelayMilliseconds = 2000;

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
  std::string logsFilePath;
  LogLevel logLevel = LogLevel::FULL;

  void LoadSettings();

 private:
  std::map<std::string, std::string> ReadUserConfigFile();

  template <typename T>
  T GetConfigValue(std::map<std::string, std::string>& configData,
                   std::string configKey);

  template <>
  int GetConfigValue<int>(std::map<std::string, std::string>& configData,
                          std::string configKey);

  template <>
  std::string GetConfigValue<std::string>(
      std::map<std::string, std::string>& configData,
      std::string configKey);

  template <>
  bool GetConfigValue<bool>(std::map<std::string, std::string>& configData,
                            std::string configKey);

  template <>
  LogLevel GetConfigValue<LogLevel>(
      std::map<std::string, std::string>& configData,
      std::string configKey);
};
}  // namespace GetGudSdk