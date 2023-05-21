#include "../../include/actions/PositionActionData.h"
#include "../config/Config.h"
#include "../utils/Utils.h"

namespace GetGudSdk {
/**
 * GetPositionActionSize:
 *
 * Return avg size of position data for calculating Game Container
 * and Action Buffer size because they consist of actions 99% of the time
 **/
unsigned int GetPositionActionSize() {
  int size = 0;
  size += 40 * sizeof(char);  // matchGuid size
  size += sizeof(long);       // actionTimeEpoch size
  size += 40 * sizeof(char);  // gameGuid size
  size += sizeof(float) * 3;  // position size
  size += sizeof(float) * 2;  // rotation size
  return size;
}

extern Config sdkConfig;

/**
 * PositionActionData:
 *
 **/
PositionActionData::PositionActionData(std::string matchGuid,
                                       long long actionTimeEpoch,
                                       std::string playerGuid,
                                       PositionF position,
                                       RotationF rotation)
    : BaseActionData(
          {Actions::Position, actionTimeEpoch, playerGuid, matchGuid}),
      position(position),
      rotation(rotation) {}

/**
 * PositionActionData:
 *
 **/
PositionActionData::PositionActionData(const PositionActionData& data)
    : BaseActionData(data), position(data.position), rotation(data.rotation) {}

/**
 * ~PositionActionData:
 *
 **/
PositionActionData::~PositionActionData() {}

/**
 * IsValid:
 *
 * Check if action is valid, if action is not valid we will delete the
 * game!
 **/
bool PositionActionData::IsValid() {
  bool isActionValid = BaseActionData::IsValid();
  return isActionValid;
}

/**
 * ToString:
 *
 * For sending action stream to Getgud
 **/
std::string PositionActionData::ToString() {
  std::string actionString;
  actionString += std::to_string(actionTimeEpoch) + ",";
  actionString += "P,";
  actionString += playerGuid + ",";
  actionString += std::to_string(position.X) + "~" +
                  std::to_string(position.Y) + "~" +
                  std::to_string(position.Z) + "~";
  actionString +=
      std::to_string(rotation.Pitch) + "~" + std::to_string(rotation.Roll);

  return actionString;
}

/**
 * ToStringMeta:
 *
 * ToString, but for logging purposes
 **/
std::string PositionActionData::ToStringMeta() {
  std::string actionMetaString = BaseActionData::ToStringMeta();

  actionMetaString += "Action position <x,y,z>: " + std::to_string(position.X) +
                      ", " + std::to_string(position.Y) + ", " +
                      std::to_string(position.Z) + "\n";
  actionMetaString +=
      "Action rotation <pitch, roll>: " + std::to_string(rotation.Pitch) +
      ", " + std::to_string(rotation.Roll) + "\n";

  return actionMetaString;
}

/**
 * Clone:
 *
 **/
PositionActionData* PositionActionData::Clone() {
  return new PositionActionData(*this);
}
}  // namespace GetGudSdk