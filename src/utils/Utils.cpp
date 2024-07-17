#include "Utils.h"

#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include "../config/Config.h"

std::random_device rd;
std::uniform_int_distribution<int> dist(0, 15);

namespace GetgudSDK {
	extern Config sdkConfig;

	/**
	 * UpdateSize:
	 *
	 * Updates the average size of Action Buffer or Game Container when attached
	 * We use this for better hypermode managing
	 **/
	void FilledAverage::UpdateSize(unsigned int size) {
		totalSize += size;
		ticks++;

		auto lastUpdateTime =
			lastUpdate +
			std::chrono::milliseconds(sdkConfig.bufferAvgSizeCalcDelayMilliseconds);

		// if the time for update comes we store avg size and start calculating
		// statics all over again, so every bufferAvgSizeCalcDelayMilliseconds
		// we have a new avg size value
		if (lastUpdateTime < std::chrono::system_clock::now()) {
			filledAverageSize = totalSize / ticks;
			totalSize = 0;
			ticks = 0;
			lastUpdate = std::chrono::system_clock::now();
		}
	}

	/**
	 * GenerateGuid:
	 *
	 * Generate a random guid of fixed length 36 (32 chars and 4 dashes)
	 **/
	std::string GenerateGuid() {
		std::string output;
		int guidStringLength = 32;

		for (int i = 0; i < guidStringLength; i++) {
			// this is relevant only for guid of length 36
			// we need to put dashes at hardcoded places
			if (i == 8 || i == 13 || i == 18 || i == 23) {
				output += "-";
				guidStringLength = guidStringLength + 1;
			}
			// if we are not putting dashes, then we are generating random char for the
			// guid
			else {
				int value = 0;
				// generate next random symbol
				value = dist(rd);
				// if the next generated symbol is number
				// int to char operation for number, + 48 comes
				// from ASCII table
				if (value <= 9)
					output += (value + 48);
				// if the symbol is letter
				// int to char operation for letter, 97-10
				// comes from ASCII table
				else
					output += (value + 97 - 10);
			}
		}

		return output;
	}

	/**
	 * GetCurrentTimeString:
	 *
	 * Get current time as string for logging purposes
	 **/
	std::string GetCurrentTimeString() {
		auto now = std::chrono::system_clock::now();
		auto in_time_t = std::chrono::system_clock::to_time_t(now);

		std::stringstream ss;
		ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
		return ss.str();
	}

	/**
	 * ShortenDecimalNumber:
	 *
	 * Trim trailing zeros and shorten incoming decimal number
	 **/
	std::string ShortenDecimalNumber(std::string decimalIn) {
		std::string decimalOut = decimalIn;
		bool dotFound = false;
		bool stopCut = false;
		int leaveDotFlag = 0;
		int numbersToDelete = 0;
		int numbersAfterDot = 0;
		// iterate over each char of the number from end
		for (int i = decimalOut.size() - 1; i >= 0; i--) {
			if (decimalOut[i] == '.') {
				dotFound = true;
				break;
			}
			else if (!stopCut && decimalOut[i] == '0') { // delete not needed 0s
				numbersToDelete++;
			}
			else if (decimalOut[i] != '0') {
				// if we found any non zero number before dot we stop
				// counting how many numbers to delete
				stopCut = true;
			}
			numbersAfterDot++;
		}

		int maxNumbersToDelete = numbersAfterDot - leaveDotFlag;
		// left minimum number and cat zeros as it's possible
		for (int i = 0; i < maxNumbersToDelete && i < numbersToDelete; i++) {
			decimalOut.pop_back();
		}
		if (dotFound) {
			if (decimalOut.size() > 6)
			{
				int firstNumId = -1;
				int leftNumbers = 6;// Cut 6 numerics in float
				for (int i = 0; i < decimalOut.size(); i++)
				{
					if (decimalOut[i] != '-' && decimalOut[i] != '.' && decimalOut[i] != '0' && firstNumId == -1)
					{
						firstNumId = i;
					}
					else if (decimalOut[i] == '.')
					{
						leftNumbers++;
					}
					else if (firstNumId != -1 && i - firstNumId >= leftNumbers)
					{
						decimalOut.pop_back();
						i--;
					}
				}
			}

			return decimalOut;
		}
		return decimalIn;
	}
}  // namespace GetgudSDK