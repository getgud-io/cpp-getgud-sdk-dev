#include "MatchData.h"
#include <algorithm>
#include "../../include/actions/DamageActionData.h"
#include "../../include/actions/PositionActionData.h"
#include "../logger/Logger.h"
#include "../utils/Utils.h"
#include "./zip/Zip.h"
#include "../src/utils/Validator.h"
#include "../src/config/Config.h"
#include <sstream>
#include <iomanip> // For std::setw, std::setfill

namespace GetgudSDK {
	extern Config sdkConfig;
	extern class Zipper zipper;
	extern Logger logger;

	namespace {
		int matchesAmount = 0;        // matches counter
		int totalCreatedMatches = 0;  // total created matches;
	}  // namespace

	/**
	 * MatchData:
	 *
	 **/
	MatchData::MatchData(std::string gameGuid,
		std::string matchMode,
		std::string mapName,
		std::string customField)
		: m_gameGuid(gameGuid), m_matchMode(matchMode), m_mapName(mapName), m_customField(customField) {
		m_matchGuid = GenerateGuid();
		matchesAmount++;
		totalCreatedMatches++;
	}

	/**
	 * MatchData:
	 *
	 **/
	MatchData::MatchData(const MatchData& data)
		:
		m_actionVector(data.m_actionVector),
		m_reportVector(data.m_reportVector),
		m_chatMessageVector(data.m_chatMessageVector),
		m_matchGuid(data.m_matchGuid),
		m_gameGuid(data.m_gameGuid),
		m_matchMode(data.m_matchMode),
		m_mapName(data.m_mapName),
		m_customField(data.m_customField),
		m_throttleChecked(data.m_throttleChecked),
		m_isInteresting(data.m_isInteresting),
		m_playerGuids(data.m_playerGuids),
		m_actionsCount(data.m_actionsCount),
		m_sizeInBytes(data.m_sizeInBytes)
	{
		matchesAmount++;
		totalCreatedMatches++;
	}

	/**
	 * ~MatchData:
	 *
	 **/
	MatchData::~MatchData() {
		matchesAmount--;
	}

	/**
	 * Clone:
	 *
	 * We need clone when we do Game and Match slicing so we can split match actions
	 * to the original match and match copy that will be sent to Getgud
	 **/
	MatchData* MatchData::Clone(bool isWithActions) {
		MatchData* cloneMatchData = new MatchData(m_gameGuid, m_matchMode, m_mapName, m_customField);
		// Clone some metadata variables
		cloneMatchData->m_matchGuid = m_matchGuid;
		cloneMatchData->m_gameGuid = m_gameGuid;
		cloneMatchData->m_matchMode = m_matchMode;
		cloneMatchData->m_mapName = m_mapName;
		cloneMatchData->m_customField = m_customField;
		cloneMatchData->m_isInteresting = m_isInteresting;
		cloneMatchData->m_throttleChecked = m_throttleChecked;
		cloneMatchData->m_matchWinTeamGuid = m_matchWinTeamGuid;
		cloneMatchData->m_matchCompletionState = m_matchCompletionState;
		cloneMatchData->m_playerGuidMap = m_playerGuidMap;
		cloneMatchData->m_nextPlayerKey = m_nextPlayerKey;
		cloneMatchData->m_weaponGuidMap = m_weaponGuidMap;
		cloneMatchData->m_nextWeaponKey = m_nextWeaponKey;

		// Clone actions, reports and chat if needed
		if (isWithActions == true) {
			cloneMatchData->AddActions(m_actionVector);

			cloneMatchData->m_reportVector.insert(cloneMatchData->m_reportVector.end(),
				m_reportVector.begin(),
				m_reportVector.end());
			cloneMatchData->m_chatMessageVector.insert(
				cloneMatchData->m_chatMessageVector.end(), m_chatMessageVector.begin(),
				m_chatMessageVector.end());

			// only if we are going to clone the data (AKA - actions) should we also
			// update the size of the object
			cloneMatchData->m_actionsCount = m_actionsCount;
			cloneMatchData->m_sizeInBytes = m_sizeInBytes;
		}

		if (isWithActions == true && cloneMatchData->m_reportVector.empty() &&
			cloneMatchData->m_chatMessageVector.empty() &&
			cloneMatchData->m_actionVector.empty()) {
			delete cloneMatchData;
			cloneMatchData = nullptr;
		}

		return cloneMatchData;
	}

	/**
	 * AddActions:
	 *
	 * Add actions to the match
	 **/
	void MatchData::AddActions(std::vector<BaseActionData*>& incomingActionVector) {
		if (incomingActionVector.size() == 0)
			return;

		int newActionVectorSize = incomingActionVector.size();

		// add the newly arrived action vector to the current action vector, where all
		// the actions are waiting to be processed
		for (auto& actionPtr : incomingActionVector) {

			m_actionVector.push_back(actionPtr);
			// store unique player guids of the match
			m_playerGuids.insert(actionPtr->m_playerGuid);
		}

		// calculate the new size of this object (using deltas only)
		m_actionsCount += newActionVectorSize;
		m_sizeInBytes += GetPositionActionSize() * newActionVectorSize;

		logger.Log(LogType::DEBUG,
			std::string("Added " + std::to_string(newActionVectorSize) +
				" action(s) to Match guid " + m_matchGuid));
	}

	/**
	 * SliceMatch:
	 *
	 * Devide match into 2 matches, original match and it's copy taking ratio of
	 * actions to the copy. This is used when match is too large for the packet
	 **/
	void MatchData::SliceMatch(float ratioToSlice,
		std::vector<BaseActionData*>& toActionVector) {
		// calculate the number of actions we need to slice from this match
		// we take upper bound here so we will take at least 1 action always
		int actionsToTakeFromMatch = ceil(ratioToSlice * (float)m_actionVector.size());
		if (actionsToTakeFromMatch <= 0)
			return;

		// the the actions to the passed vector and erase them from this match's
		// vector
		toActionVector.insert(toActionVector.end(), m_actionVector.begin(),
			m_actionVector.begin() + actionsToTakeFromMatch);
		m_actionVector.erase(m_actionVector.begin(),
			m_actionVector.begin() + actionsToTakeFromMatch);

		// calculate the new size of this match after the slice
		m_actionsCount -= actionsToTakeFromMatch;
		m_sizeInBytes -= GetPositionActionSize() * actionsToTakeFromMatch;
	}

	/**
	 * SliceMatchReportVector:
	 *
	 * Devide match reports vector into 2 parts so we will not send too
	 * many reports at once
	 **/
	void MatchData::SliceMatchReportVector(int numberToSlice,
		std::vector<ReportData*>& toReportVector) {
		if (numberToSlice <= 0)
			return;

		if (m_reportVector.size() < numberToSlice)
			return;

		// insert reports to the output vector and erase them from this match's
		// vector
		toReportVector.insert(toReportVector.end(), m_reportVector.begin(),
			m_reportVector.begin() + numberToSlice);
		m_reportVector.erase(m_reportVector.begin(),
			m_reportVector.begin() + numberToSlice);
		m_reportVector.shrink_to_fit();
	}

	/**
	 * SliceMatchChatMessageVector:
	 *
	 * Devide chat message vector into 2 parts so we will not send too
	 * many chat messages at once
	 **/
	void MatchData::SliceMatchChatMessageVector(int numberToSlice,
		std::vector<ChatMessageData*>& toChatMessageVector) {
		if (numberToSlice <= 0)
			return;

		if (m_chatMessageVector.size() < numberToSlice)
			return;

		// insert chat messages to the output vector and erase them from this match's
		// vector
		toChatMessageVector.insert(toChatMessageVector.end(), m_chatMessageVector.begin(),
			m_chatMessageVector.begin() + numberToSlice);
		m_chatMessageVector.erase(m_chatMessageVector.begin(),
			m_chatMessageVector.begin() + numberToSlice);
		m_chatMessageVector.shrink_to_fit();
	}


	/**
	 * AddInMatchReport:
	 *
	 * Attach report to the match
	 **/
	bool MatchData::AddInMatchReport(ReportInfo reportInfo) {
		ReportData* reportData = new ReportData(reportInfo);
		if (!reportData->IsValid()) {
			delete reportData;
			return false;
		}
		// we do not have to pass titleId and privateKey here
		m_reportVector.push_back(reportData);
		logger.Log(LogType::DEBUG,
			std::string("Report added to Match with guid: " + m_matchGuid));
		return true;
	}

	/**
	 * AddInMatchReport:
	 *
	 * Attach report to the match
	 **/
	bool MatchData::AddInMatchReport(ReportData* reportData) {
		if (!reportData->IsValid()) {
			delete reportData;
			return false;
		}
		// we do not have to pass titleId and privateKey here
		m_reportVector.push_back(reportData);
		logger.Log(LogType::DEBUG,
			std::string("Report added to Match with guid: " + m_matchGuid));
		return true;
	}

	/**
	 * AddChatMessage:
	 *
	 * Add chat message to the match
	 **/
	bool MatchData::AddChatMessage(ChatMessageInfo chatInfo) {
		ChatMessageData* chatMessageData = new ChatMessageData(
			chatInfo.playerGuid, chatInfo.message, chatInfo.messageTimeEpoch);
		if (!chatMessageData->IsValid()) {
			delete chatMessageData;
			return false;
		}
		m_chatMessageVector.push_back(chatMessageData);
		logger.Log(
			LogType::DEBUG,
			std::string("Chat message added to Match with guid: " + m_matchGuid));
		return true;
	}

	/**
	 * AddChatMessage:
	 *
	 * Add chat message to the match
	 **/
	bool MatchData::AddChatMessage(ChatMessageData* chatMessageData) {
		if (!chatMessageData->IsValid()) {
			delete chatMessageData;
			return false;
		}
		m_chatMessageVector.push_back(chatMessageData);
		logger.Log(
			LogType::DEBUG,
			std::string("Chat message added to Match with guid: " + m_matchGuid));
		return true;
	}

	/**
	 * MatchToString:
	 *
	 * Convert this match to string for sending to Getgud. Will output empty string
	 * if the match is not interesting
	 **/
	void MatchData::MatchToString(std::string& matchOut) {

		if (m_actionVector.empty() && m_reportVector.empty() && m_chatMessageVector.empty()) {
			return;
		}

		if (!m_isInteresting) {
			return;
		}

		std::ostringstream oss;
		std::string compressedActionStream;
		std::ostringstream actionStream;

		oss << "{";
		oss << "\"matchGuid\":\"" << m_matchGuid << "\",";
		oss << "\"mapName\":\"" << m_mapName << "\",";

		if (!m_matchMode.empty()) oss << "\"matchMode\":\"" << m_matchMode << "\",";
		if (!m_matchWinTeamGuid.empty()) oss << "\"matchWinTeamGuid\":\"" << m_matchWinTeamGuid << "\",";
		if (!m_customField.empty()) oss << "\"customField\":\"" << m_customField << "\",";
		oss << "\"matchCompletionState\":" << static_cast<int>(m_matchCompletionState) << ",";
		oss << "\"matchActionStream\":\"";

		long long lastActionTimeEpoch = 0;
		Orientation playerLastPosition;
		Orientation tempOrientation;

		// holds the map of all the players last position
		std::unordered_map<std::string, Orientation> playerLastPositionMap;

		// convert actions to an action stream string output
		for (BaseActionData* nextAction : m_actionVector) {

			// first action in the packet should have a full timestamp
			// all action after that, should just have delta timestamps compared to the first action
			nextAction->m_actionTimeEpoch -= lastActionTimeEpoch;
			lastActionTimeEpoch += nextAction->m_actionTimeEpoch;

			if (nextAction->m_actionType == Actions::Position) {

				// this is a position action, store this position as the current last position of the player that preformed this action
				PositionActionData* positionAction = static_cast<PositionActionData*>(nextAction);
				playerLastPosition = positionAction->m_orientation;

				// check if this is the first position action this player has in the action stream
				auto orientation_it = playerLastPositionMap.find(nextAction->m_playerGuid);
				if (orientation_it == playerLastPositionMap.end()) {
					
					// mark the fact that the player received its first position action and mark this action as such in the stream
					playerLastPositionMap.emplace(nextAction->m_playerGuid, positionAction->m_orientation);
					positionAction->m_isPlayerFirstPosition = true;
				}
				else {

					// update the player's last position in the map and store the delta between this and the last position in the stream
					tempOrientation = positionAction->m_orientation;
					positionAction->m_orientation = positionAction->m_orientation - orientation_it->second;
					playerLastPositionMap[nextAction->m_playerGuid] = tempOrientation;
				}
			}

			nextAction->ToString(actionStream);
		}

		if (!actionStream.str().empty()) {

			std::string actionsStr = GetPlayerMapString() + GetWeaponMapString() + "," + actionStream.str();
			actionsStr.pop_back();  // Remove the last comma
			zipper.CompressString(actionsStr, compressedActionStream);
		}
		oss << compressedActionStream << "\"";

		// Convert all reports to JSON string
		if (!m_reportVector.empty()) {
			oss << ",\"reports\":[";
			for (const auto& report : m_reportVector) {
				oss << report->ToString() << ",";
#ifdef _DEBUG
				sdkConfig.totalReportsSent++;
#endif
			}
			oss.seekp(-1, std::ios_base::end); // Remove the last comma
			oss << "]";
		}

		// Convert all chat messages to JSON string
		if (!m_chatMessageVector.empty()) {
			oss << ",\"chat\":[";
			for (const auto& chatMessage : m_chatMessageVector) {
				oss << chatMessage->ToString() << ",";
#ifdef _DEBUG
				sdkConfig.totalChatSent++;
#endif
			}
			oss.seekp(-1, std::ios_base::end); // Remove the last comma
			oss << "]";
		}

		oss << "}";
		matchOut = oss.str();
	}

	std::string MatchData::GetPlayerMapString() {
		std::string result = "playerMap";

		for (const auto& entry : m_playerGuidMap) {
			result += "~G~" + entry.second + "~G~" + entry.first;
		}

		return result;
	}

	std::string MatchData::GetWeaponMapString() {
		std::string result = "~G~weaponMap~G~";

		for (const auto& entry : m_weaponGuidMap) {
			result += "~W~" + entry.second + "~W~" + entry.first;
		}

		return result;
	}

	void MatchData::SetMatchIncompleteState(MatchCompletionState state) {

		if (m_matchCompletionState != state) {
			if (m_matchCompletionState == MatchCompletionState::Complete) m_matchCompletionState = state;
			else m_matchCompletionState = MatchCompletionState::ActionDropAndLose;
		}
	}

	/**
	 * SetThrottleCheckResults:
	 *
	 * Update throttle check results for this match
	 **/
	void MatchData::SetThrottleCheckResults(bool inThrottleChecked,
		bool inIsInteresting) {
		m_throttleChecked = inThrottleChecked;
		m_isInteresting = inIsInteresting;

	}

	/**
	 * GetThrottleChecked:
	 *
	 * Check if match was throttle checked before
	 **/
	bool MatchData::GetThrottleChecked() {
		return m_throttleChecked;
	}

	/**
	 * GetMatchSizeInBytes:
	 *
	 **/
	unsigned int MatchData::GetMatchSizeInBytes() {
		return m_sizeInBytes;
	}

	/**
	 * GetPlayerGuids:
	 *
	 **/
	std::set<std::string> MatchData::GetPlayerGuids() {
		return m_playerGuids;
	}

	/**
	 * GetReportVector:
	 *
	 **/
	std::vector<ReportData*> MatchData::GetReportVector() {
		return m_reportVector;
	}

	/**
	 * GetChatMessageVector:
	 *
	 **/
	std::vector<ChatMessageData*> MatchData::GetChatMessageVector() {
		return m_chatMessageVector;
	}

	/**
	 * GetMatchMode:
	 *
	 **/
	std::string MatchData::GetMatchMode() {
		return m_matchMode;
	}

	/**
	 * GetMapName:
	 *
	 **/
	std::string MatchData::GetMapName() {
		return m_mapName;
	}

	/**
	* GetCustomField:
	*
	**/
	std::string MatchData::GetCustomField() {
		return m_customField;
	}

	std::string MatchData::getPlayerKeyName(std::string& playerGuid) {

		// Check if the playerGuid already has a key
		auto it = m_playerGuidMap.find(playerGuid);
		if (it != m_playerGuidMap.end()) {
			return it->second; // Return the existing key
		}

		// Generate a new key (a simple incrementing integer converted to a string)
		std::string newKey = std::to_string(m_nextPlayerKey++);

		// Store the new key in the map
		m_playerGuidMap.emplace(playerGuid, newKey);

		return newKey;
	}

	std::string MatchData::getWeaponKeyName(std::string& weaponGuid) {

		// Check if the weaponGuid already has a key
		auto it = m_weaponGuidMap.find(weaponGuid);
		if (it != m_weaponGuidMap.end()) {
			return it->second; // Return the existing key
		}

		// Generate a new key (a simple incrementing integer converted to a string)
		std::string newKey = std::to_string(m_nextWeaponKey++);

		// Store the new key in the map
		m_weaponGuidMap.emplace(weaponGuid, newKey);

		return newKey;
	}

	void MatchData::SetMatchWinTeam(std::string teamGuid) {
		m_matchWinTeamGuid = teamGuid;
	}

	/**
	 * GetNumberOfMatchReportsAndMessages:
	 *
	 **/
	unsigned int MatchData::GetNumberOfMatchReportsAndMessages() {
		return m_reportVector.size() + m_chatMessageVector.size();
	}

	/**
	 * GetMatchGuid:
	 *
	 **/
	std::string MatchData::GetMatchGuid() {
		return m_matchGuid;
	}

	/**
	 * GetGameGuid:
	 *
	 **/
	std::string MatchData::GetGameGuid() {
		return m_gameGuid;
	}

	/**
	 * Dispose:
	 *
	 **/
	void MatchData::Dispose() {
		for (auto& action : m_actionVector)
			delete action;
		m_actionVector.clear();
		for (auto& report : m_reportVector)
			delete report;
		m_reportVector.clear();
		m_reportVector.shrink_to_fit();
		for (auto& chatMessage : m_chatMessageVector)
			delete chatMessage;
		m_chatMessageVector.clear();
		m_playerGuids.clear();
	}

	bool MatchData::IsValid() {
		bool isActionValid = Validator::ValidateStringLength(m_gameGuid, 1, 36);
		isActionValid &= Validator::ValidateStringChars(m_gameGuid);
		isActionValid &= Validator::ValidateStringLength(m_matchMode, 0, 36);
		isActionValid &= Validator::ValidateStringChars(m_matchMode);
		isActionValid &= Validator::ValidateStringLength(m_mapName, 1, 36);
		isActionValid &= Validator::ValidateStringChars(m_mapName);
		isActionValid &= Validator::ValidateStringLength(m_customField, 0, 128);
		isActionValid &= Validator::ValidateStringChars(m_customField);
		return isActionValid;
	}

}  // namespace GetgudSDK
