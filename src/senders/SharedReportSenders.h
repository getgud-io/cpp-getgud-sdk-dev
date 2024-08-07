#pragma once
#include <mutex>
#include <vector>

namespace GetgudSDK {

	class ReportSender;

	/**
	 * Shared data structure that stores all known player updaters for easy access
	 **/
	 //TODO LATER: make it using vector
	struct SharedReportSenders {
		int reportSendersCount = 0;
		ReportSender* reportSender;
		std::mutex reportSendersMutex;
	};
}  // namespace GetgudSDK