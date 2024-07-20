#include "AffectActionData.h"
#include "../config/Config.h"
#include "../utils/Validator.h"

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
	 * Check if action is valid, if action is not valid we will delete the
	 * game!
	 **/
	bool AffectActionData::IsValid() {
		// basic validations are done in the base class first
		bool isActionValid = BaseActionData::IsValid();
		isActionValid &= Validator::ValidateStringLength(m_affectGuid, 1, 36);
		isActionValid &= Validator::ValidateStringChars(m_affectGuid);
		isActionValid &= Validator::ValidateItemValue(static_cast<int>(m_affectState), 0, 3);

		return isActionValid;
	}

	/**
	 * ToString:
	 *
	 * For sending action stream to Getgud
	 **/
	std::string AffectActionData::ToString() {
		std::string actionString;
		actionString += std::to_string(m_actionTimeEpoch) + ",";
		actionString += "AF,";
		actionString += m_playerGuid + ",";
		actionString += m_affectGuid + ",";
		actionString += std::to_string(static_cast<int>(m_affectState));

		return actionString;
	}

	/**
	 * Clone:
	 *
	 **/
	AffectActionData* AffectActionData::Clone() {
		return new AffectActionData(*this);
	}

}  // namespace GetgudSDK