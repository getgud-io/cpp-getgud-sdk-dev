#include "pch.h"
#include "../../include/actions/AttackActionData.h"
#include "../config/Config.h"
#include "../utils/Validator.h"

namespace GetGudSdk {

extern Config sdkConfig;

/**
 * AttackActionData:
 *
 **/
AttackActionData::AttackActionData(std::string matchGuid,
                                   long long actionTimeEpoch,
                                   std::string playerGuid,
                                   std::string weaponGuid)
    : BaseActionData({Actions::Attack, actionTimeEpoch, playerGuid, matchGuid}),
      m_weaponGuid(weaponGuid){

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
AttackActionData::~AttackActionData(){

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
  isActionValid &= Validator::ValidateStringLength(m_weaponGuid, 1, 12);
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
 * ToStringMeta:
 *
 * ToString, but for logging purposes
 **/
std::string AttackActionData::ToStringMeta() {
  std::string actionMetaString = BaseActionData::ToStringMeta();

  actionMetaString += "Action weapon guid: " + m_weaponGuid + "\n";

  return actionMetaString;
}

/**
 * Clone:
 *
 **/
AttackActionData* AttackActionData::Clone() {
  return new AttackActionData(*this);
}

}  // namespace GetGudSdk