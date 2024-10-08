#include "DamageActionData.h"
#include "../config/Config.h"
#include "../utils/Validator.h"
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
	 * Check if action is valid, if action is not valid we will delete the
	 * game!
	 **/
	bool DamageActionData::IsValid() {
		bool isActionValid = BaseActionData::IsValid();
		isActionValid &= Validator::ValidateStringLength(m_victimPlayerGuid, 1, 36);
		isActionValid &= Validator::ValidateStringChars(m_victimPlayerGuid);
		isActionValid &= Validator::ValidateStringLength(m_weaponGuid, 1, 36);
		isActionValid &= Validator::ValidateStringChars(m_weaponGuid);

		return isActionValid;
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
