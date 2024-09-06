#include "HealActionData.h"
#include "../config/Config.h"
#include "../utils/Validator.h"
#include "../utils/Utils.h"
#include <sstream>

using namespace GetgudSDK;

extern Config sdkConfig;

/**
 * HealActionData:
 *
 **/
HealActionData::HealActionData(std::string matchGuid,
	long long actionTimeEpoch,
	std::string playerGuid,
	const float healthGained)
	: BaseActionData({ Actions::Heal, actionTimeEpoch, playerGuid, matchGuid }),
	m_healthGained(healthGained) {}

/**
 * HealActionData:
 *
 **/
HealActionData::HealActionData(const HealActionData& data)
	: BaseActionData(data), m_healthGained(data.m_healthGained) {}

/**
 * ~HealActionData:
 *
 **/
HealActionData::~HealActionData() {

};

/**
 * IsValid:
 *
 * Check if action is valid, if action is not valid we will delete the
 * game!
 **/
bool HealActionData::IsValid() {
	bool isActionValid = BaseActionData::IsValid();
	return isActionValid;
}

/**
 * ToString:
 *
 * For sending action stream to Getgud
 **/
void HealActionData::ToString(std::ostringstream& oss) {
	oss << m_actionTimeEpoch << ","  // Append m_actionTimeEpoch
		<< "H,"                      // Append constant string "H"
		<< m_playerGuid << ","       // Append m_playerGuid
		<< ShortenDecimalNumber(std::to_string(m_healthGained)) << ","; // Append shortened decimal number of m_healthGained
}

/**
 * Clone:
 *
 **/
HealActionData* HealActionData::Clone() {
	return new HealActionData(*this);
}