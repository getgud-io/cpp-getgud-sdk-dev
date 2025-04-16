#include "DamageActionData.h"
#include "../config/Config.h"
#include "../utils/Validator.h"
#include "../utils/Sanitizer.h"
#include "../utils/Utils.h"
#include <sstream>

namespace GetgudSDK {

	extern Config sdkConfig;

	/**
	 * DamageActionData:
	 *
	 **/
	DamageActionData::DamageActionData(std::string matchGuid,
		long long actionTimeEpoch,
		std::string playerGuid,
		std::string victimPlayerGuid,
		float damageDone,
		std::string weaponGuid)
		: BaseActionData({ Actions::Damage, actionTimeEpoch, playerGuid, matchGuid }),
		m_weaponGuid(weaponGuid),
		m_victimPlayerGuid(victimPlayerGuid),
		m_damageDone(damageDone)
	       	{}

	/**
	 * DamageActionData:
	 *
	 **/
	DamageActionData::DamageActionData(const DamageActionData& data)
		: BaseActionData(data),
		m_weaponGuid(data.m_weaponGuid),
		m_victimPlayerGuid(data.m_victimPlayerGuid),
		m_damageDone(data.m_damageDone)
       	{}

	/**
	 * ~DamageActionData:
	 *
	 **/
	DamageActionData::~DamageActionData() {

	};

	/**
	 * IsValid:
	 *
	 * Check if core action data is valid. Sanitize non-core fields.
	 **/
	bool DamageActionData::IsValid() {
		// Core validations (playerGuid, matchGuid, timestamp, actionType)
		bool isCoreValid = BaseActionData::IsValid();

		// Core validation for numeric range (assuming damageDone is still checked)
		// isCoreValid &= Validator::ValidateItemValue(m_damageDone, 0.0f, FLT_MAX); // Example if needed

		// Sanitize non-core fields
		Sanitizer::SanitizeStringChars(m_victimPlayerGuid); // Victim GUID is treated like other GUIDs now
		Sanitizer::SanitizeStringLength(m_victimPlayerGuid, 36);
		Sanitizer::SanitizeStringChars(m_weaponGuid);
		Sanitizer::SanitizeStringLength(m_weaponGuid, 36);

		return isCoreValid;
	}

	/**
	 * ToString:
	 *
	 * For sending action stream to Getgud
	 **/
	void DamageActionData::ToString(std::ostringstream& oss) {
		oss << m_actionTimeEpoch << ","  // Append m_actionTimeEpoch
			<< "D,"                      // Append constant string "D"
			<< m_playerGuid << ","       // Append m_playerGuid
			<< m_victimPlayerGuid << "," // Append m_victimPlayerGuid
			<< m_damageDone << ","  // Append shortened decimal number
			<< m_weaponGuid << ",";             // Append m_weaponGuid
	}

	/**
	 * Clone:
	 *
	 **/
	DamageActionData* DamageActionData::Clone() {
		return new DamageActionData(*this);
	}
}  // namespace GetgudSDK
