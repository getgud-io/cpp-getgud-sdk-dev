#include "pch.h"
#include "../../include/actions/PositionActionData.h"
#include "../config/Config.h"
#include "../utils/Utils.h"
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
      m_position(position),
      m_rotation(rotation) {}

/**
 * PositionActionData:
 *
 **/
PositionActionData::PositionActionData(const PositionActionData& data)
    : BaseActionData(data), m_position(data.m_position), m_rotation(data.m_rotation) {}

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
  actionString += CutDecimalNumber(std::to_string(m_position.X)) + "~" +
                  CutDecimalNumber(std::to_string(m_position.Y)) + "~" +
                  CutDecimalNumber(std::to_string(m_position.Z)) + "~";
  actionString += CutDecimalNumber(std::to_string(m_rotation.Pitch)) + "~" +
                  CutDecimalNumber(std::to_string(m_rotation.Roll));

  return actionString;
}

/**
 * ToStringMeta:
 *
 * ToString, but for logging purposes
 **/
std::string PositionActionData::ToStringMeta() {
  std::string actionMetaString = BaseActionData::ToStringMeta();

  actionMetaString += "Position:" + std::to_string(m_position.X) + ", " +
                      std::to_string(m_position.Y) + ", " +
                      std::to_string(m_position.Z) + ";";
  actionMetaString += "Rotation:" + std::to_string(m_rotation.Pitch) + ", " +
                      std::to_string(m_rotation.Roll) + ";";

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