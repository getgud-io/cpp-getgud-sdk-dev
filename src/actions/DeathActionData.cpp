#include "DeathActionData.h"
#include "../config/Config.h"
#include "../utils/Validator.h"
#include "../utils/Sanitizer.h"
#include <sstream>

namespace GetgudSDK {

	extern Config sdkConfig;

	/**
	 * DeathActionData:
	 *
	 **/
	DeathActionData::DeathActionData(std::string matchGuid,
		long long actionTimeEpoch,
		std::string playerGuid,
		std::string attackerGuid)
		: BaseActionData({ Actions::Death, actionTimeEpoch, playerGuid, matchGuid }),
		m_attackerGuid(attackerGuid) {

	}

	/**
	 * DeathActionData:
	 *
	 **/
	DeathActionData::DeathActionData(const DeathActionData& data)
		: BaseActionData(data), m_attackerGuid(data.m_attackerGuid) {}

	/**
	 * ~DeathActionData:
	 *
	 **/
	DeathActionData::~DeathActionData() {

	};

	/**
	 * IsValid:
	 *
	 * Check if core action data is valid. Sanitize non-core fields.
	 **/
	bool DeathActionData::IsValid() {
		// Core validations (playerGuid, matchGuid, timestamp, actionType)
		bool isCoreValid = BaseActionData::IsValid();

		isCoreValid &= Validator::ValidateStringLength(m_attackerGuid, 1, 36);
		isCoreValid &= Validator::ValidateStringChars(m_attackerGuid);
		

		return isCoreValid;
	}

	/**
	 * ToString:
	 *
	 * For sending action stream to Getgud
	 **/
	void DeathActionData::ToString(std::ostringstream& oss) {
		oss << m_actionTimeEpoch << ","  // Append m_actionTimeEpoch
			<< "DD,"                     // Append constant string "DD"
			<< m_playerGuid << ","       // Append m_playerGuid
			<< m_attackerGuid << ",";           // Append m_attackerGuid
	}

	/**
	 * Clone:
	 *
	 **/
	DeathActionData* DeathActionData::Clone() {
		return new DeathActionData(*this);
	}
}  // namespace GetgudSDK