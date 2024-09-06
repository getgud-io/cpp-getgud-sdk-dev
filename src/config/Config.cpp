#include "Config.h"
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
	 * Config:
	 *
	 * Initialize default constructor
	 **/
	Config::Config() { }


	/**
	 * LoadSettings:
	 *
	 * Get values of variables from the config file
	 **/
	bool Config::LoadSettings(std::string configFile, bool passAsContent) {
		char* logsFilePathHolder = std::getenv("GETGUD_LOG_FILE_PATH");
		if (logsFilePathHolder == nullptr) {
			// Environment variable LOG_FILE_PATH is required to work
			logger.Log(LogType::DEBUG, std::string("Config::LoadSettings->Environment variable GETGUD_LOG_FILE_PATH is empty - log file will be created in the root folder"));
		}
		else {
			logsFilePath = std::string(logsFilePathHolder);
		}

		std::map<std::string, std::string> configData = ReadUserConfigFile(configFile, passAsContent);

		if (configData.empty())
			return false;

		bool valueReadResult = false;

		// Read logger parameters
		GetConfigValue(configData, sdkConfigFieldNames.logToFile, sdkConfig.logToFile);

		unsigned int _logFileSizeInBytes = 0;
		valueReadResult = GetConfigValue(
			configData, sdkConfigFieldNames.logFileSizeInBytes, _logFileSizeInBytes);
		valueReadResult &=
			Validator::ValidateItemValue(_logFileSizeInBytes, 0U, 100000000U);
		if (valueReadResult) {
			sdkConfig.logFileSizeInBytes = _logFileSizeInBytes;
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
		unsigned int _reportsMaxBufferSizeInBytes = 0;

		valueReadResult = GetConfigValue(
			configData, sdkConfigFieldNames.reportsMaxBufferSizeInBytes,
			_reportsMaxBufferSizeInBytes);
		valueReadResult &=
			Validator::ValidateItemValue(_reportsMaxBufferSizeInBytes, 0U, 10000000U);
		if (valueReadResult) {
			sdkConfig.reportsMaxBufferSizeInBytes = _reportsMaxBufferSizeInBytes;
		}

		unsigned int _maxReportsToSendAtOnce = 0;
		valueReadResult =
			GetConfigValue(configData, sdkConfigFieldNames.maxReportsToSendAtOnce,
				_maxReportsToSendAtOnce);
		valueReadResult &=
			Validator::ValidateItemValue(_maxReportsToSendAtOnce, 0U, 100U);
		if (valueReadResult) {
			sdkConfig.maxReportsToSendAtOnce = _maxReportsToSendAtOnce;
		}

		// Read player updeter variables
		unsigned int _playersMaxBufferSizeInBytes = 0;
		valueReadResult = GetConfigValue(
			configData, sdkConfigFieldNames.playersMaxBufferSizeInBytes,
			_playersMaxBufferSizeInBytes);
		valueReadResult &=
			Validator::ValidateItemValue(_playersMaxBufferSizeInBytes, 0U, 10000000U);
		if (valueReadResult) {
			sdkConfig.playersMaxBufferSizeInBytes = _playersMaxBufferSizeInBytes;
		}

		unsigned int _maxPlayerUpdatesToSendAtOnce = 0;
		valueReadResult = GetConfigValue(
			configData, sdkConfigFieldNames.maxPlayerUpdatesToSendAtOnce,
			_maxPlayerUpdatesToSendAtOnce);
		valueReadResult &=
			Validator::ValidateItemValue(_maxPlayerUpdatesToSendAtOnce, 0U, 100U);
		if (valueReadResult) {
			sdkConfig.maxPlayerUpdatesToSendAtOnce = _maxPlayerUpdatesToSendAtOnce;
		}

		// Read chat message variables
		unsigned int _maxChatMessagesToSendAtOnce = 0;
		valueReadResult = GetConfigValue(
			configData, sdkConfigFieldNames.maxChatMessagesToSendAtOnce,
			_maxChatMessagesToSendAtOnce);
		valueReadResult &=
			Validator::ValidateItemValue(_maxChatMessagesToSendAtOnce, 0U, 100U);
		if (valueReadResult) {
			sdkConfig.maxChatMessagesToSendAtOnce = _maxChatMessagesToSendAtOnce;
		}

		// Read game sender variables
		unsigned int _gameSenderSleepIntervalMilliseconds = 0;
		valueReadResult = GetConfigValue(
			configData, sdkConfigFieldNames.gameSenderSleepIntervalMilliseconds,
			_gameSenderSleepIntervalMilliseconds);
		valueReadResult &= Validator::ValidateItemValue(
			_gameSenderSleepIntervalMilliseconds, 0U, 5000U);
		if (valueReadResult) {
			sdkConfig.gameSenderSleepIntervalMilliseconds =
				_gameSenderSleepIntervalMilliseconds;
		}
		unsigned int _apiTimeoutMilliseconds = 0;
		valueReadResult =
			GetConfigValue(configData, sdkConfigFieldNames.apiTimeoutMilliseconds,
				_apiTimeoutMilliseconds);
		valueReadResult &=
			Validator::ValidateItemValue(_apiTimeoutMilliseconds, 0U, 20000U);
		if (valueReadResult) {
			sdkConfig.apiTimeoutMilliseconds = _apiTimeoutMilliseconds;
		}
		unsigned int _apiTimeoutRetries = 3;
		valueReadResult =
			GetConfigValue(configData, sdkConfigFieldNames.apiTimeoutRetries,
				_apiTimeoutRetries);
		valueReadResult &=
			Validator::ValidateItemValue(_apiTimeoutRetries, 1U, 20U);
		if (valueReadResult) {
			sdkConfig.apiTimeoutRetries = _apiTimeoutRetries;
		}
		unsigned int _packetMaxSizeInBytes = 0;
		valueReadResult =
			GetConfigValue(configData, sdkConfigFieldNames.packetMaxSizeInBytes,
				_packetMaxSizeInBytes);
		valueReadResult &=
			Validator::ValidateItemValue(_packetMaxSizeInBytes, 0U, 4000000U);
		if (valueReadResult) {
			sdkConfig.packetMaxSizeInBytes = _packetMaxSizeInBytes;
		}

		// Read action buffer variables
		unsigned int _actionsBufferMaxSizeInBytes = 0;
		valueReadResult = GetConfigValue(
			configData, sdkConfigFieldNames.actionsBufferMaxSizeInBytes,
			_actionsBufferMaxSizeInBytes);
		valueReadResult &= Validator::ValidateItemValue(_actionsBufferMaxSizeInBytes,
			500U, 100000000U);
		if (valueReadResult) {
			sdkConfig.actionsBufferMaxSizeInBytes = _actionsBufferMaxSizeInBytes;
		}

		// Read game container variables
		unsigned int _gameContainerMaxSizeInBytes = 0;
		valueReadResult = GetConfigValue(
			configData, sdkConfigFieldNames.gameContainerMaxSizeInBytes,
			_gameContainerMaxSizeInBytes);
		valueReadResult &= Validator::ValidateItemValue(_gameContainerMaxSizeInBytes,
			500U, 500000000U);
		if (valueReadResult) {
			sdkConfig.gameContainerMaxSizeInBytes = _gameContainerMaxSizeInBytes;
		}
		unsigned int _maxGames = 0;
		valueReadResult =
			GetConfigValue(configData, sdkConfigFieldNames.maxGames, _maxGames);
		valueReadResult &= Validator::ValidateItemValue(_maxGames, 1U, 100U);
		if (valueReadResult) {
			sdkConfig.maxGames = _maxGames;
		}
		unsigned int _maxMatchesPerGame = 0;
		valueReadResult = GetConfigValue(
			configData, sdkConfigFieldNames.maxMatchesPerGame, _maxMatchesPerGame);
		valueReadResult &= Validator::ValidateItemValue(_maxMatchesPerGame, 1U, 100U);
		if (valueReadResult) {
			sdkConfig.maxMatchesPerGame = _maxMatchesPerGame;
		}
		unsigned int _minPacketSizeForSendingInBytes = 0;
		valueReadResult = GetConfigValue(
			configData, sdkConfigFieldNames.minPacketSizeForSendingInBytes,
			_minPacketSizeForSendingInBytes);
		valueReadResult &= Validator::ValidateItemValue(
			_minPacketSizeForSendingInBytes, 500U, 4000000U);
		if (valueReadResult) {
			sdkConfig.minPacketSizeForSendingInBytes = _minPacketSizeForSendingInBytes;
		}
		unsigned int _packetTimeoutInMilliseconds = 0;
		valueReadResult = GetConfigValue(
			configData, sdkConfigFieldNames.packetTimeoutInMilliseconds,
			_packetTimeoutInMilliseconds);
		valueReadResult &=
			Validator::ValidateItemValue(_packetTimeoutInMilliseconds, 500U, 120000U);
		if (valueReadResult) {
			sdkConfig.packetTimeoutInMilliseconds = _packetTimeoutInMilliseconds;
		}
		unsigned int _gameCloseGraceAfterMarkEndInMilliseconds = 0;
		valueReadResult = GetConfigValue(
			configData, sdkConfigFieldNames.gameCloseGraceAfterMarkEndInMilliseconds,
			_gameCloseGraceAfterMarkEndInMilliseconds);
		valueReadResult &= Validator::ValidateItemValue(
			_gameCloseGraceAfterMarkEndInMilliseconds, 0U, 200000U);
		if (valueReadResult) {
			sdkConfig.gameCloseGraceAfterMarkEndInMilliseconds =
				_gameCloseGraceAfterMarkEndInMilliseconds;
		}
		unsigned int _liveGameTimeoutInMilliseconds = 0;
		valueReadResult = GetConfigValue(
			configData, sdkConfigFieldNames.liveGameTimeoutInMilliseconds,
			_liveGameTimeoutInMilliseconds);
		valueReadResult &=
			Validator::ValidateItemValue(_liveGameTimeoutInMilliseconds, 0U, 300000U);
		if (valueReadResult) {
			sdkConfig.liveGameTimeoutInMilliseconds = _liveGameTimeoutInMilliseconds;
		}

		// Read hyper mode variables
		GetConfigValue(configData, sdkConfigFieldNames.hyperModeFeatureEnabled,
			sdkConfig.hyperModeFeatureEnabled);

		unsigned int _hyperModeMaxThreads = 0;
		valueReadResult = GetConfigValue(
			configData, sdkConfigFieldNames.hyperModeMaxThreads, _hyperModeMaxThreads);
		valueReadResult &= Validator::ValidateItemValue(_hyperModeMaxThreads, 1U, 20U);
		if (valueReadResult) {
			sdkConfig.hyperModeMaxThreads = _hyperModeMaxThreads;
		}
		unsigned int _hyperModeAtBufferPercentage = 0;
		valueReadResult = GetConfigValue(
			configData, sdkConfigFieldNames.hyperModeAtBufferPercentage,
			_hyperModeAtBufferPercentage);
		valueReadResult &=
			Validator::ValidateItemValue(_hyperModeAtBufferPercentage, 10U, 90U);
		if (valueReadResult) {
			sdkConfig.hyperModeAtBufferPercentage = _hyperModeAtBufferPercentage;
		}
		unsigned int _hyperModeUpperPercentageBound = 0;
		valueReadResult = GetConfigValue(
			configData, sdkConfigFieldNames.hyperModeUpperPercentageBound,
			_hyperModeUpperPercentageBound);
		valueReadResult &=
			Validator::ValidateItemValue(_hyperModeUpperPercentageBound, 30U, 90U);
		valueReadResult &=
			sdkConfig.hyperModeAtBufferPercentage < _hyperModeUpperPercentageBound;
		if (valueReadResult) {
			sdkConfig.hyperModeUpperPercentageBound = _hyperModeUpperPercentageBound;
		}
		unsigned int _hyperModeThreadCreationStaggerMilliseconds = 0;
		valueReadResult = GetConfigValue(
			configData,
			sdkConfigFieldNames.hyperModeThreadCreationStaggerMilliseconds,
			_hyperModeThreadCreationStaggerMilliseconds);
		valueReadResult &= Validator::ValidateItemValue(
			_hyperModeThreadCreationStaggerMilliseconds, 0U, 10000U);
		if (valueReadResult) {
			sdkConfig.hyperModeThreadCreationStaggerMilliseconds =
				_hyperModeThreadCreationStaggerMilliseconds;
		}

		GetConfigValue(configData, sdkConfigFieldNames.logLevel, sdkConfig.logLevel);

		return true;
	}

	/**
	 * ReadUserConfigFile:
	 *
	 * Read all config values from the file
	 **/
	std::map<std::string, std::string> Config::ReadUserConfigFile(std::string configFile, bool passAsContent) {
		std::map<std::string, std::string> output_map;

		std::string contentString;

		if (passAsContent == false && !configFile.empty())
		{
			// Loading config file from provided path
			configFilePath = configFile;
		}
		else if (passAsContent == true && !configFile.empty())
		{
			// Loading config file from a content string
			contentString = configFile;
		}
		else
		{
			char* configPathHolder = std::getenv("GETGUD_CONFIG_PATH");
			if (configPathHolder != nullptr) {

				// Loading config file from GETGUD_CONFIG_PATH Environment Parameter
				configFilePath = std::string(configPathHolder);
			}
		}

		if (contentString.empty())
		{
			std::ifstream file_stream(configFilePath);
			char next_character = 0;

			if (file_stream.is_open()) {
				while (file_stream.good()) {
					file_stream.get(next_character);
					contentString += next_character;
				}
			}
			else {
				logger.Log(LogType::WARN, std::string("Config file was not found, thus cannot be loaded.There are four ways to load the config file:" 
					"\n1) You can provide a full path to the config file using the Init(configFileFullPath) method" 
					"\n2) You can provide the config file content as string using the Init(configFile, passConfigAsConect = true) method." 
					"\n3) You can provide a config file full path using the GETGUD_CONFIG_PATH environment variable and call the Init() method" 
					"\n4) You can place the config file where the GetgudSDK is running from and call the Init() method. The config file Will be searched in the reletaive path: ./config.json"));
			}

			file_stream.close();
		}

		std::string next_key;
		std::string next_value;

		bool key_started = false;
		bool key_ended = false;
		bool value_started = false;

		for (int i = 0; i < contentString.size(); i++) {
			char next_character = contentString[i];

			if (next_character == '\t')
				continue;

			if (key_started && next_character != '\"' && next_character != ',' &&
				next_character != '\n') {
				next_key += next_character;
			}
			else if (value_started && next_character != ' ' &&
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
				}
				else if (key_ended && value_started) {
					value_started = false;
				}
				else if (!key_ended && !key_started) {
					key_started = true;
				}
				else if (!key_ended && key_started) {
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
			}
			else if (configIter->second == "false") {
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
			}
			else if (configValue == "_ERROR") {
				outValue = LogLevel::_ERROR;
			}
			else if (configValue == "WARN_AND_ERROR") {
				outValue = LogLevel::WARN_AND_ERROR;
			}
			else if (configValue == "FULL") {
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
		configString += "\t" + fieldNames.apiTimeoutRetries + ": " +
			std::to_string(sdkConfig.apiTimeoutRetries) + ",\n";
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
