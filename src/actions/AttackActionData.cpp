#include "AttackActionData.h"
#include "../config/Config.h"
#include "../utils/Validator.h"

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
	 * Check if action is valid, if action is not valid we will delete the
	 * game!
	 **/
	bool AttackActionData::IsValid() {
		// basic validations are done in the base class first
		bool isActionValid = BaseActionData::IsValid();
		isActionValid &= Validator::ValidateStringLength(m_weaponGuid, 1, 36);
		isActionValid &= Validator::ValidateStringChars(m_weaponGuid);

		return isActionValid;
	}

	/**
	 * ToString:
	 *
	 * For sending action stream to Getgud
	 **/
	std::string AttackActionData::ToString() {
		std::string actionString;
		actionString += std::to_string(m_actionTimeEpoch) + ",";
		actionString += "A,";
		actionString += m_playerGuid + ",";
		actionString += m_weaponGuid;

		return actionString;
	}

	/**
	 * Clone:
	 *
	 **/
	AttackActionData* AttackActionData::Clone() {
		return new AttackActionData(*this);
	}

}  // namespace GetgudSDK