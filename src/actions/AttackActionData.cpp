#include "AttackActionData.h"
#include "../config/Config.h"
#include "../utils/Validator.h"
#include "../utils/Sanitizer.h"
#include <sstream>

namespace GetgudSDK {

	extern Config sdkConfig;

	/**
	 * AttackActionData:
	 *
	 **/
	AttackActionData::AttackActionData(std::string matchGuid,
		long long actionTimeEpoch,
		std::string playerGuid,
		std::string weaponGuid)
		: BaseActionData({ Actions::Attack, actionTimeEpoch, playerGuid, matchGuid }),
		m_weaponGuid(weaponGuid) {

	};

	/**
	 * AttackActionData:
	 *
	 **/
	AttackActionData::AttackActionData(const AttackActionData& data)
		: BaseActionData(data), m_weaponGuid(data.m_weaponGuid) {}


	/**
	 * ~AttackActionData:
	 *
	 **/
	AttackActionData::~AttackActionData() {

	};

	/**
	 * IsValid:
	 *
	 * Check if core action data is valid. Sanitize non-core fields.
	 **/
	bool AttackActionData::IsValid() {
		// Core validations (playerGuid, matchGuid, timestamp, actionType)
		bool isCoreValid = BaseActionData::IsValid();

		// Sanitize non-core fields
		Sanitizer::SanitizeStringChars(m_weaponGuid); // Assuming standard disallowed chars for weaponGuid
		Sanitizer::SanitizeStringLength(m_weaponGuid, 36); // Max length 36

		return isCoreValid;
	}

	/**
	 * ToString:
	 *
	 * For sending action stream to Getgud
	 **/
	void AttackActionData::ToString(std::ostringstream& oss) {
		oss << m_actionTimeEpoch << ","  // Append m_actionTimeEpoch
			<< "A,"                      // Append constant string "A"
			<< m_playerGuid << ","       // Append m_playerGuid
			<< m_weaponGuid << ",";             // Append m_weaponGuid
	}

	/**
	 * Clone:
	 *
	 **/
	AttackActionData* AttackActionData::Clone() {
		return new AttackActionData(*this);
	}

}  // namespace GetgudSDK