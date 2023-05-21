#include "../../include/actions/BaseActionData.h"
#include <mutex>
#include "../config/Config.h"
#include "../utils/Validator.h"

#ifdef _DEBUG
std::mutex counterMutex;
#endif

namespace GetGudSdk {

unsigned int GetEmptyActionSize() {
  int size = 0;
  size += 40 * sizeof(char);  // matchGuid size
  size += sizeof(long);       // actionTimeEpoch size
  return size;
}

extern Config sdkConfig;
/**
 * BaseActionData:
 * @data: BaseData, structure contains all core metadata for each action
 *
 * Constructor of Base Action, all other actions inherit from BaseAction
 **/
BaseActionData::BaseActionData(BaseData data, bool _isEmpty) {
  actionType = data.ActionType;
  actionTimeEpoch = data.ActionTimeEpoch;
  playerGuid = data.PlayerGuid;
  matchGuid = data.MatchGuid;
  isEmpty = _isEmpty;

   if (isEmpty) {
    sdkConfig.emptyActionsAmount++;
    sdkConfig.totalCreatedEmptyActions++;
  } else {
    sdkConfig.actionsAmount++;
    sdkConfig.totalCreatedActions++;
  }
  // TODO: we need to make it long long and
                                   // when it is
                          // full refresh the var
};

BaseActionData::BaseActionData(const BaseActionData& data) {
  actionType = data.actionType;
  actionTimeEpoch = data.actionTimeEpoch;
  playerGuid = data.playerGuid;
  matchGuid = data.matchGuid;
  isEmpty = data.isEmpty;
  if (isEmpty) {
    sdkConfig.emptyActionsAmount++;
    sdkConfig.totalCreatedEmptyActions++;
  } else {
    sdkConfig.actionsAmount++;
    sdkConfig.totalCreatedActions++;
  }
}

/**
 * BaseActionData:
 *
 **/
BaseActionData::~BaseActionData() {
  if (isEmpty) {
    sdkConfig.emptyActionsAmount--;
  } else {
    sdkConfig.actionsAmount--;
  }
};

/**
 * IsValid:
 *
 * Check if action is valid, if action is not valid we will delete the
 * game!
 **/
bool BaseActionData::IsValid() {
  // TODO add flag so that actions can mark for game deletion themselves
  // TODO ValidateStringChars shoudn't be used as a GUID validator, guid is
  // letters, numbers and dashes only
  // TODO validate actionType (can be pushed wrong using cast from int)
  bool isActionValid = Validator::ValidateStringLength(playerGuid, 1, 36);
  isActionValid &= Validator::ValidateStringChars(playerGuid);
  isActionValid &= Validator::ValidateStringLength(matchGuid, 1, 36);
  isActionValid &= Validator::ValidateStringChars(matchGuid);
  isActionValid &= Validator::ValidateItemValue(
      actionTimeEpoch, sdkConfig.sdkValidatorConfig.minActionTimeEpochTime,
      sdkConfig.sdkValidatorConfig.maxActionTimeEpochTime);

  // empty action is passed when game container or action buffer are full
  // for game deletion purposes
  isActionValid &= !isEmpty;

  return isActionValid;
}

/**
 * ToString:
 *
 * For sending action stream to Getgud
 **/
std::string BaseActionData::ToString() {
  std::string actionString;

  return actionString;
}

/**
 * ToStringMeta:
 *
 * ToString, but for logging purposes
 **/
std::string BaseActionData::ToStringMeta() {
  std::string actionMetaString;

  std::string actionTypeStr;

  switch (actionType) {
    case Actions::None:
      actionTypeStr = "None";
      break;
    case Actions::Attack:
      actionTypeStr = "Attack";
      break;
    case Actions::Damage:
      actionTypeStr = "Damage";
      break;
    case Actions::Death:
      actionTypeStr = "Death";
      break;
    case Actions::Heal:
      actionTypeStr = "Heal";
      break;
    case Actions::Position:
      actionTypeStr = "Position";
      break;
    case Actions::Spawn:
      actionTypeStr = "Spawn";
      break;
    default:
      actionTypeStr = "None";
      break;
  }

  actionMetaString += "\n";
  actionMetaString += "Action type: " + actionTypeStr + "\n";
  actionMetaString += "Action time: " + std::to_string(actionTimeEpoch) + "\n";
  actionMetaString += "Action match guid: " + matchGuid + "\n";
  actionMetaString += "Action player guid: " + playerGuid + "\n";

  return actionMetaString;
}

/**
 * Clone:
 *
 **/
BaseActionData* BaseActionData::Clone() {
  return new BaseActionData(*this);
}

}  // namespace GetGudSdk