#include "SpawnActionData.h"
#include "../config/Config.h"
#include "../utils/Utils.h"
#include <sstream>

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
	std::string teamGuid,
	float initialHealth,
	PositionF position,
	RotationF rotation)
	: BaseActionData({ Actions::Spawn, actionTimeEpoch, playerGuid, matchGuid }),
	m_position(position),
	m_rotation(rotation),
	m_initialHealth(initialHealth),
	m_teamGuid(teamGuid),
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
	m_teamGuid(data.m_teamGuid),
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
	isActionValid &= Validator::ValidateStringLength(m_teamGuid, 1, 36);
	isActionValid &= Validator::ValidateStringChars(m_teamGuid);

	return isActionValid;
}

/**
 * ToString:
 *
 * For sending action stream to Getgud
 **/
void SpawnActionData::ToString(std::ostringstream& oss) {
	oss << m_actionTimeEpoch << ","  // Append m_actionTimeEpoch
		<< "S,"  // Append constant string "S"
		<< m_playerGuid << ","  // Append m_playerGuid
		<< m_characterGuid << ","  // Append m_characterGuid
		<< m_teamGuid << ","  // Append m_teamGuid
		<< m_initialHealth << ","  // Append initial health
		<< m_position.X << "~"  // Append X coordinate
		<< m_position.Y << "~"  // Append Y coordinate
		<< m_position.Z << "~"  // Append Z coordinate
		<< m_rotation.Yaw << "~"  // Append Yaw rotation
		<< m_rotation.Pitch;  // Append Pitch rotation

	// Check for optional Roll value
	if (m_rotation.Roll != -1000.f) {
		oss << "~" << m_rotation.Roll;  // Append Roll if valid
	}

	oss << ",";
}

/**
 * Clone:
 *
 **/
SpawnActionData* SpawnActionData::Clone() {
	return new SpawnActionData(*this);
}