#include "CustomEventActionData.h"
#include "../config/Config.h"
#include "../utils/Validator.h"
#include "../utils/Sanitizer.h"
#include "../utils/Utils.h"
#include <limits>
#include <sstream>

namespace GetgudSDK {

	extern Config sdkConfig;

	/**
	 * CustomEventActionData:
	 *
	 **/
	CustomEventActionData::CustomEventActionData(std::string matchGuid,
		long long actionTimeEpoch,
		std::string playerGuid,
		std::string customEventGuid,
		int version,
		const std::string& payload)
		: BaseActionData({ Actions::CustomEvent, actionTimeEpoch,
			playerGuid.empty() ? Values::g_PvE : playerGuid, matchGuid }),
		m_customEventGuid(customEventGuid), m_version(version),
		m_payloadBase64(Base64Encode(reinterpret_cast<const unsigned char*>(payload.data()),
			static_cast<unsigned int>(payload.size()))) {

	};

	/**
	 * CustomEventActionData:
	 *
	 **/
	CustomEventActionData::CustomEventActionData(const CustomEventActionData& data)
		: BaseActionData(data), m_customEventGuid(data.m_customEventGuid),
		m_version(data.m_version), m_payloadBase64(data.m_payloadBase64) {}


	/**
	 * ~CustomEventActionData:
	 *
	 **/
	CustomEventActionData::~CustomEventActionData() {

	};

	/**
	 * IsValid:
	 *
	 * Check if core action data is valid. Sanitize non-core fields.
	 **/
	bool CustomEventActionData::IsValid() {
		// Core validations (playerGuid, matchGuid, timestamp, actionType)
		bool isCoreValid = BaseActionData::IsValid();

		isCoreValid &= Validator::ValidateItemValue(m_version, 0, std::numeric_limits<int>::max());

		// An empty payload would serialize as an empty trailing field, which the
		// stream parsers trim away and then misread the action as too short.
		isCoreValid &= Validator::ValidateStringLength(m_payloadBase64, 1, sdkConfig.sdkValidatorConfig.maxCustomEventPayloadBase64Length);

		// Sanitize non-core fields
		Sanitizer::SanitizeStringChars(m_customEventGuid);
		Sanitizer::SanitizeStringLength(m_customEventGuid, 36);

		return isCoreValid;
	}

	/**
	 * ToString:
	 *
	 * For sending action stream to Getgud
	 **/
	void CustomEventActionData::ToString(std::ostringstream& oss) {
		oss << m_actionTimeEpoch << ","   // Append m_actionTimeEpoch
			<< "C,"                       // Append constant string "C"
			<< m_playerGuid << ","        // Append m_playerGuid
			<< m_customEventGuid << ","   // Append m_customEventGuid
			<< m_version << ","           // Append m_version
			<< m_payloadBase64 << ",";    // Append base64 encoded payload
	}

	/**
	 * Clone:
	 *
	 **/
	CustomEventActionData* CustomEventActionData::Clone() {
		return new CustomEventActionData(*this);
	}
}  // namespace GetgudSDK
