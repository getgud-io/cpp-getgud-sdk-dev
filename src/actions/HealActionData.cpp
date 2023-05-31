#include "pch.h"
#include "../../include/actions/HealActionData.h"
#include "../config/Config.h"
#include "../utils/Validator.h"
#include "../utils/Utils.h"

using namespace GetGudSdk;

extern Config sdkConfig;

/**
 * HealActionData:
 *
 **/
HealActionData::HealActionData(std::string matchGuid,
                               long long actionTimeEpoch,
                               std::string playerGuid,
                               const float healthGained)
    : BaseActionData({Actions::Heal, actionTimeEpoch, playerGuid, matchGuid}),
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
HealActionData::~HealActionData(){

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
std::string HealActionData::ToString() {
  std::string actionString;
  actionString += std::to_string(m_actionTimeEpoch) + ",";
  actionString += "H,";
  actionString += m_playerGuid + ",";
  actionString += CutDecimalNumber(std::to_string(m_healthGained));

  return actionString;
}

/**
 * ToStringMeta:
 *
 * ToString, but for logging purposes
 **/
std::string HealActionData::ToStringMeta() {
  std::string actionMetaString = BaseActionData::ToStringMeta();

  actionMetaString +=
      "Action health gained: " + std::to_string(m_healthGained) + "\n";

  return actionMetaString;
}

/**
 * Clone:
 *
 **/
HealActionData* HealActionData::Clone() {
  return new HealActionData(*this);
}