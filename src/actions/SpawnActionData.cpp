#include "SpawnActionData.h"
#include "../config/Config.h"
#include "../utils/Utils.h"

using namespace GetgudSDK;

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
      m_position(position),
      m_rotation(rotation),
      m_initialHealth(initialHealth),
      m_teamId(teamId),
      m_characterGuid(characterGuid) {}

/**
 * SpawnActionData:
 *
 **/
SpawnActionData::SpawnActionData(const SpawnActionData& data)
    : BaseActionData(data),
      m_position(data.m_position),
      m_rotation(data.m_rotation),
      m_initialHealth(data.m_initialHealth),
      m_teamId(data.m_teamId),
      m_characterGuid(data.m_characterGuid) {}

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
  isActionValid &= Validator::ValidateStringLength(m_characterGuid, 1, 36);
  isActionValid &= Validator::ValidateStringChars(m_characterGuid);

  return isActionValid;
}

/**
 * ToString:
 *
 * For sending action stream to Getgud
 **/
std::string SpawnActionData::ToString() {
  std::string actionString;
  actionString += std::to_string(m_actionTimeEpoch) + ",";
  actionString += "S,";
  actionString += m_playerGuid + ",";
  actionString += m_characterGuid + ",";
  actionString += std::to_string(m_teamId) + ",";
  actionString += ShortenDecimalNumber(std::to_string(m_initialHealth)) + ",";
  actionString += ShortenDecimalNumber(std::to_string(m_position.X)) + "~" +
                  ShortenDecimalNumber(std::to_string(m_position.Y)) + "~" +
                  ShortenDecimalNumber(std::to_string(m_position.Z)) + "~";
  actionString += ShortenDecimalNumber(std::to_string(m_rotation.Yaw)) + "~" +
    ShortenDecimalNumber(std::to_string(m_rotation.Pitch));
  if (m_rotation.Roll != -1000.f)
  {
    actionString += "~" +
      ShortenDecimalNumber(std::to_string(m_rotation.Roll));
  }

  return actionString;
}

/**
 * ToStringMeta:
 *
 * ToString, but for logging purposes
 **/
std::string SpawnActionData::ToStringMeta() {
  std::string actionMetaString = BaseActionData::ToStringMeta();


  actionMetaString += "Character guid:" + m_characterGuid + ";";
  actionMetaString += "Team id:" + std::to_string(m_teamId) + ";";
  actionMetaString += "Health:" + std::to_string(m_initialHealth)  + ";";
  actionMetaString += "Position:" + std::to_string(m_position.X) + "," +
                      std::to_string(m_position.Y) + "," +
                      std::to_string(m_position.Z) + ";";
  actionMetaString += "Rotation:" + std::to_string(m_rotation.Yaw) + "," +
                      std::to_string(m_rotation.Pitch) + "," +
                      std::to_string(m_rotation.Roll);

  if (m_rotation.Roll != -1000.f)
  {
    actionMetaString += "," + std::to_string(m_rotation.Roll);
  }

  actionMetaString += ";";

  return actionMetaString;
}

/**
 * Clone:
 *
 **/
SpawnActionData* SpawnActionData::Clone() {
  return new SpawnActionData(*this);
}