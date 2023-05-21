#include "../../include/actions/SpawnActionData.h"
#include "../config/Config.h"
#include "../utils/Utils.h"

using namespace GetGudSdk;

extern Config sdkConfig;

/**
 * SpawnActionData:
 *
 **/
SpawnActionData::SpawnActionData(std::string matchGuid,
                                 long long actionTimeEpoch,
                                 std::string playerGuid,
                                 std::string characterGuid,
                                 int teamId,
                                 float initialHealth,
                                 PositionF position,
                                 RotationF rotation)
    : BaseActionData({Actions::Spawn, actionTimeEpoch, playerGuid, matchGuid}),
      position(position),
      rotation(rotation),
      initialHealth(initialHealth),
      teamId(teamId),
      characterGuid(characterGuid) {}

/**
 * SpawnActionData:
 *
 **/
SpawnActionData::SpawnActionData(const SpawnActionData& data)
    : BaseActionData(data),
      position(data.position),
      rotation(data.rotation),
      initialHealth(data.initialHealth),
      teamId(data.teamId),
      characterGuid(data.characterGuid) {}

/**
 * ~SpawnActionData:
 *
 **/
SpawnActionData::~SpawnActionData() {}

/**
 * IsValid:
 *
 * Check if action is valid, if action is not valid we will delete the
 * game!
 **/
bool SpawnActionData::IsValid() {
  bool isActionValid = BaseActionData::IsValid();
  isActionValid &= Validator::ValidateStringLength(characterGuid, 1, 36);
  isActionValid &= Validator::ValidateStringChars(characterGuid);

  return isActionValid;
}

/**
 * ToString:
 *
 * For sending action stream to Getgud
 **/
std::string SpawnActionData::ToString() {
  std::string actionString;
  actionString += std::to_string(actionTimeEpoch) + ",";
  actionString += "S,";
  actionString += playerGuid + ",";
  actionString += characterGuid + ",";
  actionString += std::to_string(teamId) + ",";
  actionString += std::to_string(initialHealth) + ",";
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
std::string SpawnActionData::ToStringMeta() {
  std::string actionMetaString = BaseActionData::ToStringMeta();

  actionMetaString += "Action characterGuid: " + characterGuid + "\n";
  actionMetaString += "Action teamId: " + std::to_string(teamId) + "\n";
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
SpawnActionData* SpawnActionData::Clone() {
  return new SpawnActionData(*this);
}