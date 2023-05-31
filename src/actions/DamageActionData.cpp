#include "pch.h"
#include "../../include/actions/DamageActionData.h"
#include "../config/Config.h"
#include "../utils/Validator.h"
#include "../utils/Utils.h"

namespace GetGudSdk {

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
    : BaseActionData({Actions::Damage, actionTimeEpoch, playerGuid, matchGuid}),
      m_damageDone(damageDone),
      m_weaponGuid(weaponGuid),
      m_victimPlayerGuid(victimPlayerGuid) {}

/**
 * DamageActionData:
 *
 **/
DamageActionData::DamageActionData(const DamageActionData& data)
    : BaseActionData(data),
      m_damageDone(data.m_damageDone),
      m_weaponGuid(data.m_weaponGuid),
      m_victimPlayerGuid(data.m_victimPlayerGuid) {}

/**
 * ~DamageActionData:
 *
 **/
DamageActionData::~DamageActionData(){

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
  isActionValid &= Validator::ValidateStringLength(m_weaponGuid, 1, 3);
  isActionValid &= Validator::ValidateStringChars(m_weaponGuid);

  return isActionValid;
}

/**
 * ToString:
 *
 * For sending action stream to Getgud
 **/
std::string DamageActionData::ToString() {
  std::string actionString;
  actionString += std::to_string(m_actionTimeEpoch) + ",";
  actionString += "D,";
  actionString += m_playerGuid + ",";
  actionString += m_victimPlayerGuid + ",";
  actionString += CutDecimalNumber(std::to_string(m_damageDone)) + ",";
  actionString += m_weaponGuid;

  return actionString;
}

/**
 * ToStringMeta:
 *
 * ToString, but for logging purposes
 **/
std::string DamageActionData::ToStringMeta() {
  std::string actionMetaString = BaseActionData::ToStringMeta();

  actionMetaString += "Action victim player guid: " + m_victimPlayerGuid + "\n";
  actionMetaString += "Action weapon guid: " + m_weaponGuid + "\n";
  actionMetaString +=
      "Action damage done: " + std::to_string(m_damageDone) + "\n";

  return actionMetaString;
}

/**
 * Clone:
 *
 **/
DamageActionData* DamageActionData::Clone() {
  return new DamageActionData(*this);
}
}  // namespace GetGudSdk