#include "DeathActionData.h"
#include "../config/Config.h"
#include "../utils/Validator.h"

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
	 * Check if action is valid, if action is not valid we will delete the
	 * game!
	 **/
	bool DeathActionData::IsValid() {
		bool isActionValid = BaseActionData::IsValid();

		return isActionValid;
	}

	/**
	 * ToString:
	 *
	 * For sending action stream to Getgud
	 **/
	std::string DeathActionData::ToString() {
		std::string actionString;
		actionString += std::to_string(m_actionTimeEpoch) + ",";
		actionString += "DD,";
		actionString += m_playerGuid + ",";
		actionString += m_attackerGuid;

		return actionString;
	}

	/**
	 * Clone:
	 *
	 **/
	DeathActionData* DeathActionData::Clone() {
		return new DeathActionData(*this);
	}
}  // namespace GetgudSDK