#include "Logger.h"
#include <fstream>
#include <iostream>
#include "../config/Config.h"
#include "../utils/Utils.h"

#ifdef __linux__
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

namespace GetgudSDK {
	Logger logger;  // const initialization at this stage

	extern Config sdkConfig;

	long Logger::GetFileSize() {
		struct stat stat_buf;
		int rc = stat(sdkConfig.logsFilePath.c_str(), &stat_buf);
		return rc == 0 ? stat_buf.st_size : -1;
	}

	/**
	 * ManageConfigFileSize:
	 *
	 * Make sure that config file size doesn't overcome limits
	 * put in config
	 **/
	void Logger::ManageConfigFileSize() {
		long fileSize = GetFileSize();
		if (fileSize != -1 && fileSize < (long)sdkConfig.logFileSizeInBytes) {
			return;
		}
		else if (fileSize != -1) {
			if (sdkConfig.circularLogFile) {
				//resize file by copy and rename
				std::string line;
				std::ifstream fin;
				std::string tempName = sdkConfig.logsFilePath + "temp";

				fin.open(sdkConfig.logsFilePath.c_str());
				// contents of path must be copied to a temp file then
				// renamed back to the path file
				std::ofstream temp;
				temp.open(tempName);
				int linesDeleted = 0;
				while (getline(fin, line)) {
					// write all lines to temp other than the line marked for erasing
					if (linesDeleted < (long)sdkConfig.linesDeletionAmount) {
						linesDeleted++;
						continue;
					}
					else {
						temp << line << std::endl;
					}
				}

				temp.close();
				fin.close();

				// required conversion for remove and rename functions
				remove(sdkConfig.logsFilePath.c_str());
				if (rename(tempName.c_str(), sdkConfig.logsFilePath.c_str())) {
					return;
				}
			}
			else {
				//delete file data
				std::ofstream ofs;
				ofs.open(std::string(sdkConfig.logsFilePath).c_str(),
					std::ofstream::out | std::ofstream::trunc);
				ofs.close();
			}

		}
	}

	/**
	 * WriteToFile:
	 *
	 * Write the whole log string into the output file so it can be later accessed
	 * by client for debugging purposes
	 **/
	void Logger::WriteToFile(std::string outString) {
		if (sdkConfig.logsFilePath.empty())
			return;

		ManageConfigFileSize();

		m_writeMutex.lock();
		std::ofstream logsFile;
		logsFile.open(std::string(sdkConfig.logsFilePath).c_str(),
			std::ios_base::app);
		logsFile << outString;
		logsFile.close();
		m_writeMutex.unlock();
	}

	/**
	 * TODO: delete this
	 **/
	void Logger::WriteActionToFile(std::string fileName, std::string outString) {
		std::string filePath = "C:\\Users\\artkulak444\\Desktop\\gg-cpp-sdk\\matches\\";

		m_writeMutex.lock();
		std::ofstream logsFile;
		logsFile.open(std::string(filePath + fileName + ".txt").c_str(),
			std::ios_base::app);
		logsFile << outString;
		logsFile.close();
		m_writeMutex.unlock();
	}

	/**
	 * Log:
	 *
	 * Central logging operation. Decided if to log string or not
	 **/
	void Logger::Log(LogType logType, std::string logString) {
		if (!sdkConfig.logToFile)
			return;
		std::string messageTag;
		std::string current_time = GetCurrentTimeString();
		std::string messageEnd = "\n";
		if (logType == LogType::DEBUG && sdkConfig.logLevel >= LogLevel::FULL) {
			messageTag = "::Debug-> ";
			WriteToFile(current_time + messageTag + logString + messageEnd);
		}
		else if (logType == LogType::WARN &&
			sdkConfig.logLevel >= LogLevel::WARN_AND_ERROR) {
			messageTag = "::Warning-> ";
			WriteToFile(current_time + messageTag + logString + messageEnd);
		}
		else if (logType == LogType::_ERROR &&
			sdkConfig.logLevel >= LogLevel::_ERROR) {
			messageTag = "::Error-> ";
			WriteToFile(current_time + messageTag + logString + messageEnd);
		}
		else if (logType == LogType::FATAL &&
			sdkConfig.logLevel >= LogLevel::FATAL) {
			messageTag = "::Fatal-> ";
			WriteToFile(current_time + messageTag + logString + messageEnd);
		}
	}
}  // namespace GetgudSDK