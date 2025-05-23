#pragma once

#include <chrono>
#include <random>
#include <string>
#include <vector>

#include "../../include/actions/Helpers.h"

namespace GetgudSDK {
	/**
	 * FilledAverage:
	 *
	 * Used to calculate avg size of Action Buffer and Game Container for
	 * better hyper mode managing
	 **/
	struct FilledAverage {
		int filledAverageSize = 0;
		unsigned int ticks = 0;
		int totalSize = 0;
		std::chrono::system_clock::time_point lastUpdate =
			std::chrono::system_clock::now();
		void UpdateSize(unsigned int size);
	};
	std::string GenerateGuid();
	std::string GetCurrentTimeString();
	std::string ShortenDecimalNumber(std::string decimalIn);
	std::chrono::system_clock::duration ToSystemDuration(int ms);
}  // namespace GetgudSDK