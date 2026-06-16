#include "ChatMessageData.h"
#include "../utils/Validator.h"
#include "../utils/Sanitizer.h"
#include "../config/Config.h"

namespace GetgudSDK {
	extern Config sdkConfig;

	/**
	 * ChatMessageData:
	 *
	 **/
	ChatMessageData::ChatMessageData(std::string playerGuid,
		std::string message,
		long long messageTimeEpoch)
		: m_playerGuid(playerGuid),
		m_message(message),
		m_messageTimeEpoch(messageTimeEpoch) {}

	/**
	 * ToString:
	 *
	 * Used when we send chat messages to get good along with match actions
	 **/
	std::string ChatMessageData::ToString() {
		std::string messageString;
		messageString += "{";
		messageString += "	\"playerGuid\": \"" + m_playerGuid + "\",";
		messageString += "	\"message\": \"" + m_message + "\",";
		messageString +=
			"	\"messageTimeEpoch\": " + std::to_string(m_messageTimeEpoch);
		messageString += "}";

		return messageString;
	}

	bool ChatMessageData::IsValid() {
		// Core validations
		bool isCoreValid = Validator::ValidateStringLength(m_playerGuid, 1, 36);
		isCoreValid &= Validator::ValidateStringChars(m_playerGuid);
		isCoreValid &= Validator::ValidateItemValue(
			m_messageTimeEpoch,
			sdkConfig.sdkValidatorConfig.minActionTimeEpochTime,
			sdkConfig.sdkValidatorConfig.maxActionTimeEpochTime);

		// Sanitize message: length first, then allowed chars (replacing " and \)
		Sanitizer::SanitizeStringLength(m_message, 1024); // Truncate with ellipsis
		Sanitizer::SanitizeStringCharsSpecial(m_message); // Replace ", \, $, %, '

		return isCoreValid;
	}
}  // namespace GetgudSDK