#include "PositionActionData.h"
#include "../config/Config.h"
#include "../utils/Utils.h"
#include "../utils/Utils.h"
#include <sstream>

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
			{ Actions::Position, actionTimeEpoch, playerGuid, matchGuid }),
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
	void PositionActionData::ToString(std::ostringstream& oss) {

		oss << m_actionTimeEpoch << ","     // Append m_actionTimeEpoch
			<< "P,"                        // Append constant string "P"
			<< m_playerGuid << ","          // Append m_playerGuid
			<< m_orientation.position.X << "~" // Append X coordinate
			<< m_orientation.position.Y << "~" // Append Y coordinate
			<< m_orientation.position.Z << "~" // Append Z coordinate
			<< m_orientation.rotation.Yaw << "~" // Append Yaw rotation
			<< m_orientation.rotation.Pitch; // Append Pitch rotation

		// Check for optional Roll value
		if (m_orientation.rotation.Roll != -1000.f) {
			oss << "~" << m_orientation.rotation.Roll; // Append Roll if valid
		}

		// Check if this is the first position of this player in the packet's action stream
		if (m_isPlayerFirstPosition == true) {
			oss << "F"; // Append an indication that this is the player's first position at the end of  the oriantation parameters
		}

		oss << ",";
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