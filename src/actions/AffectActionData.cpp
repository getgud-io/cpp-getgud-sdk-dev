#include "AffectActionData.h"
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
	AffectActionData::AffectActionData(std::string matchGuid,
		long long actionTimeEpoch,
		std::string playerGuid,
		std::string affectGuid,
		AffectState affectState)
		: BaseActionData({ Actions::Affect, actionTimeEpoch, playerGuid, matchGuid }),
		m_affectGuid(affectGuid), m_affectState(affectState) {

	};

	/**
	 * AttackActionData:
	 *
	 **/
	AffectActionData::AffectActionData(const AffectActionData& data)
		: BaseActionData(data), m_affectGuid(data.m_affectGuid), m_affectState(data.m_affectState) {}


	/**
	 * ~AttackActionData:
	 *
	 **/
	AffectActionData::~AffectActionData() {

	};

	/**
	 * IsValid:
	 *
	 * Check if core action data is valid. Sanitize non-core fields.
	 **/
	bool AffectActionData::IsValid() {
		// Core validations (playerGuid, matchGuid, timestamp, actionType)
		bool isCoreValid = BaseActionData::IsValid();

		// Core validation for enum range
		isCoreValid &= Validator::ValidateItemValue(static_cast<int>(m_affectState), 0, 3); 

		// Sanitize non-core fields
		Sanitizer::SanitizeStringChars(m_affectGuid); // Assuming standard disallowed chars for affectGuid
		Sanitizer::SanitizeStringLength(m_affectGuid, 36); // Max length 36

		return isCoreValid;
	}

	/**
	 * ToString:
	 *
	 * For sending action stream to Getgud
	 **/
	void AffectActionData::ToString(std::ostringstream& oss) {
		oss << m_actionTimeEpoch << ","   // Append m_actionTimeEpoch
			<< "AF,"                      // Append constant string "AF"
			<< m_playerGuid << ","        // Append m_playerGuid
			<< m_affectGuid << ","        // Append m_affectGuid
			<< static_cast<int>(m_affectState) << ",";  // Append m_affectState cast to int
	}

	/**
	 * Clone:
	 *
	 **/
	AffectActionData* AffectActionData::Clone() {
		return new AffectActionData(*this);
	}

}  // namespace GetgudSDK