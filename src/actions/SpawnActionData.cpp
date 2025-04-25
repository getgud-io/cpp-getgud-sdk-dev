#include "SpawnActionData.h"
#include "../config/Config.h"
#include "../utils/Validator.h"
#include "../utils/Sanitizer.h"
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
 * Check if core action data is valid. Sanitize non-core fields.
 **/
bool SpawnActionData::IsValid() {
	// Core validations (playerGuid, matchGuid, timestamp, actionType)
	bool isCoreValid = BaseActionData::IsValid();

	// Sanitize non-core fields
	Sanitizer::SanitizeStringChars(m_characterGuid);
	Sanitizer::SanitizeStringLength(m_characterGuid, 36);
	Sanitizer::SanitizeStringChars(m_teamGuid);
	Sanitizer::SanitizeStringLength(m_teamGuid, 36);

	return isCoreValid;
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