#include "../../include/actions/DamageActionData.h"
#include "../config/Config.h"
#include "../utils/Validator.h"

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
      damageDone(damageDone),
      weaponGuid(weaponGuid),
      victimPlayerGuid(victimPlayerGuid) {}

/**
 * DamageActionData:
 *
 **/
DamageActionData::DamageActionData(const DamageActionData& data)
    : BaseActionData(data),
      damageDone(data.damageDone),
      weaponGuid(data.weaponGuid),
      victimPlayerGuid(data.victimPlayerGuid) {}

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
  isActionValid &= Validator::ValidateStringLength(victimPlayerGuid, 1, 36);
  isActionValid &= Validator::ValidateStringChars(victimPlayerGuid);
  isActionValid &= Validator::ValidateStringLength(weaponGuid, 1, 3);
  isActionValid &= Validator::ValidateStringChars(weaponGuid);

  return isActionValid;
}

/**
 * ToString:
 *
 * For sending action stream to Getgud
 **/
std::string DamageActionData::ToString() {
  std::string actionString;
  actionString += std::to_string(actionTimeEpoch) + ",";
  actionString += "D,";
  actionString += playerGuid + ",";
  actionString += victimPlayerGuid + ",";
  actionString += std::to_string(damageDone) + ",";
  actionString += weaponGuid;

  return actionString;
}

/**
 * ToStringMeta:
 *
 * ToString, but for logging purposes
 **/
std::string DamageActionData::ToStringMeta() {
  std::string actionMetaString = BaseActionData::ToStringMeta();

  actionMetaString += "Action victim player guid: " + victimPlayerGuid + "\n";
  actionMetaString += "Action weapon guid: " + weaponGuid + "\n";
  actionMetaString +=
      "Action damage done: " + std::to_string(damageDone) + "\n";

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