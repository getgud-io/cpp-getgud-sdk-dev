#include "pch.h"
#include "../../include/actions/PositionActionData.h"
#include "../config/Config.h"
#include "../utils/Utils.h"
#include "../utils/Utils.h"

namespace GetgudSDK {
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
  m_orientation({ position, rotation }) {}

/**
 * PositionActionData:
 *
 **/
PositionActionData::PositionActionData(const PositionActionData& data)
    : BaseActionData(data), m_orientation(data.m_orientation) {}

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
  actionString += std::to_string(m_actionTimeEpoch) + ",";
  actionString += "P,";
  actionString += m_playerGuid + ",";
  actionString += ShortenDecimalNumber(std::to_string(m_orientation.position.X)) + "~" +
                  ShortenDecimalNumber(std::to_string(m_orientation.position.Y)) + "~" +
                  ShortenDecimalNumber(std::to_string(m_orientation.position.Z)) + "~";
  actionString += ShortenDecimalNumber(std::to_string(m_orientation.rotation.Yaw)) + "~" +
    ShortenDecimalNumber(std::to_string(m_orientation.rotation.Pitch));
  if (m_orientation.rotation.Roll != -1000.f)
  {
    actionString += "~" + ShortenDecimalNumber(std::to_string(m_orientation.rotation.Roll));
  }

  return actionString;
}

/**
 * ToStringMeta:
 *
 * ToString, but for logging purposes
 **/
std::string PositionActionData::ToStringMeta() {
  std::string actionMetaString = BaseActionData::ToStringMeta();

  actionMetaString += "Position:" + std::to_string(m_orientation.position.X) + ", " +
                      std::to_string(m_orientation.position.Y) + "," +
                      std::to_string(m_orientation.position.Z) + ";";
  actionMetaString += "Rotation:" + std::to_string(m_orientation.rotation.Yaw) + "," +
    std::to_string(m_orientation.rotation.Pitch);

  if (m_orientation.rotation.Roll != -1000.f)
  {
    actionMetaString += "," +std::to_string(m_orientation.rotation.Roll);
  }
  
  actionMetaString += ";";

  return actionMetaString;
}

/**
 * Clone:
 *
 **/
PositionActionData* PositionActionData::Clone() {
  return new PositionActionData(*this);
}

Orientation& PositionActionData::getOrientation()
{
  return m_orientation;
}

}  // namespace GetgudSDK