#include "GameData.h"
#include "../config/Config.h"
#include "../logger/Logger.h"
#include "../utils/Utils.h"
#include "../utils/Validator.h"
#include "../utils/Sanitizer.h"

#ifdef __linux__
#include <limits.h>
#include <stdio.h>
#endif

namespace GetgudSDK {
	extern Config sdkConfig;
	extern Logger logger;

	namespace {
		int gamesAmount = 0;
		int totalCreatedGames = 0;
	}  // namespace

	/**
	 * GameData:
	 *
	 **/
	GameData::GameData(int titleId,
		std::string privateKey,
		std::string serverGuid,
		std::string gameMode,
		std::string serverLocation)
		: m_titleId(titleId),
		m_privateKey(privateKey),
		m_serverGuid(serverGuid),
		m_gameMode(gameMode),
		m_serverLocation(serverLocation) {
		m_gameGuid = GenerateGuid();
		m_startGameTimer = std::chrono::system_clock::now();
		m_lastUpdateTime = std::chrono::system_clock::now();
		gamesAmount++;
		totalCreatedGames++;
	}

	/**
	 * GameData:
	 *
	 **/
	GameData::GameData(const GameData& data)
		: m_matchMap(data.m_matchMap),
		m_matchGuidVector(data.m_matchGuidVector),
		m_titleId(data.m_titleId),
		m_privateKey(data.m_privateKey),
		m_gameGuid(data.m_gameGuid),
		m_serverGuid(data.m_serverGuid),
		m_gameMode(data.m_gameMode),
		m_isGameMarkedAsEnded(data.m_isGameMarkedAsEnded),
		m_sizeInBytes(data.m_sizeInBytes),
		m_startGameTimer(data.m_startGameTimer),
		m_lastUpdateTime(data.m_lastUpdateTime) {
		gamesAmount++;
		totalCreatedGames++;
	}

	/**
	 * ~GameData:
	 *
	 **/
	GameData::~GameData() {
		gamesAmount--;
	}

	/**
	 * Clone:
	 *
	 * We need clone when we do Game and Match slicing so we can split match actions
	 * to the original match and match copy that will be sent to Getgud
	 **/
	GameData* GameData::Clone(bool isWithActions) {
		// Clone some metadata variables
		GameData* cloneGameData =
			new GameData(m_titleId, m_privateKey, m_serverGuid, m_gameMode, m_serverLocation);
		cloneGameData->m_gameGuid = m_gameGuid;
		cloneGameData->m_isGameMarkedAsEnded = m_isGameMarkedAsEnded;
		cloneGameData->m_sizeInBytes = m_sizeInBytes;
		cloneGameData->m_startGameTimer = m_startGameTimer;
		cloneGameData->m_lastUpdateTime = m_lastUpdateTime;
		cloneGameData->m_sentGameMarkedAsEnded = m_sentGameMarkedAsEnded;

		std::string matchGuid;

		// run through all the game's matches and clone them as well
		for (int index = 0; index < m_matchGuidVector.size(); index++) {
			matchGuid = m_matchGuidVector[index];
			auto matchData_it = m_matchMap.find(matchGuid);
			if (matchData_it == m_matchMap.end())
				continue;
			auto* cloneMatchPtr = matchData_it->second->Clone(isWithActions);
			if (cloneMatchPtr == nullptr)
				continue;
			std::pair<std::string, MatchData*> matchGuidPair(matchGuid, cloneMatchPtr);
			cloneGameData->m_matchMap.insert(matchGuidPair);
			cloneGameData->m_matchGuidVector.push_back(matchGuid);
		}

		return cloneGameData;
	}

	void GameData::SentGameMarkedAsEnded() {

		m_sentGameMarkedAsEnded = true;
	}

	void GameData::MarkGameMatchesAsNotInteresting(std::vector<std::string>& matchGuids) {

		std::string matchGuid;

		for (int index = 0; index < matchGuids.size(); index++) {
			matchGuid = matchGuids[index];
			auto matchData_it = m_matchMap.find(matchGuid);
			if (matchData_it == m_matchMap.end())
				continue;

			matchData_it->second->SetThrottleCheckResults(true, false);
			logger.Log(LogType::WARN, std::string("Match with the following Guid was marked as Not Interesting: " + matchGuid));
		}
	}

	void GameData::SetGameMatchesIncompleteState(std::vector<std::string>& matchGuids, MatchCompletionState state) {

		std::string matchGuid;

		for (int index = 0; index < matchGuids.size(); index++) {
			matchGuid = matchGuids[index];
			auto matchData_it = m_matchMap.find(matchGuid);
			if (matchData_it == m_matchMap.end())
				continue;

			matchData_it->second->SetMatchIncompleteState(state);
			logger.Log(LogType::WARN, std::string("Match with the following Guid was marked as incomplete: " + matchGuid + " with the state: " + std::to_string((int)state)));
		}
	}

	/**
	 * MarkGameAsEnded:
	 *
	 **/
	void GameData::MarkGameAsEnded() {
		m_isGameMarkedAsEnded = true;
		logger.Log(LogType::DEBUG,
			std::string("Mark end game for game guid: " + m_gameGuid));
	}

	bool GameData::IsGameMarkedAsEnded() {
		return m_isGameMarkedAsEnded;
	}

	bool GameData::DidSendGameMarkedAsEnded() {
		return m_sentGameMarkedAsEnded;
	}

	/**
	 * AddMatch:
	 *
	 **/
	MatchData* GameData::AddMatch(std::string matchMode, std::string mapName, std::string customField) {

		// make sure the game has enough room for another match
		if (m_matchGuidVector.size() > sdkConfig.maxMatchesPerGame) {
			logger.Log(LogType::WARN, std::string("GameData::AddMatch->Live matches per game limit reached - cannot add a new match to game with guid: " + m_gameGuid));
			return nullptr;
		}

		// create the new match with the passed parameters and insert it to the game's match map
		MatchData* matchData = new MatchData(m_gameGuid, matchMode, mapName, customField);
		if (!matchData->IsValid()) {
			logger.Log(LogType::WARN, std::string("GameData::AddMatch->One or more of the Match's paraemters are not valid - Match will not start. Match paraemters: matchMode: " + matchMode + " | mapName: " + mapName + " | gameGuid: " + m_gameGuid));
			delete matchData;
			return nullptr;
		}
		std::pair<std::string, MatchData*> matchGuidPair(matchData->GetMatchGuid(), matchData);
		m_matchMap.insert(matchGuidPair);
		m_matchGuidVector.push_back(matchData->GetMatchGuid());

		m_lastUpdateTime = std::chrono::system_clock::now();

		return matchData;
	}

	/**
	 * IsGameEligibleForProcessing:
	 *
	 * Checks if game is eligible for processing for PopNextGameToProcess of
	 * GameContainer purposes, it means that the game packet COULD be sent
	 * to Getgud
	 **/
	bool GameData::IsGameEligibleForProcessing() {
		unsigned int gameSizeInBytes = GetGameSizeInBytes();

		// check if this game's size so far is big enough to be sent
		if (gameSizeInBytes > sdkConfig.minPacketSizeForSendingInBytes)
			return true;

		// check if the game is marked as ended AND enough time has passed since the
		// marking (to give time for actions to assimilate before closing)
		else if (m_isGameMarkedAsEnded == true &&
			m_lastUpdateTime +
			ToSystemDuration(
				sdkConfig.gameCloseGraceAfterMarkEndInMilliseconds) <
			std::chrono::system_clock::now())
			return true;

		// check if this game's packet has been waiting for long long enough without
		// any data coming in to it
		else if (m_lastUpdateTime + ToSystemDuration(
			sdkConfig.packetTimeoutInMilliseconds) <
			std::chrono::system_clock::now())
			return true;

		// none of the conditions of eligibility are met, this game is not elidible
		// for processing (it's not big enough, it's not closed be the client and it's
		// not old enough)
		return false;
	}

	/**
	 * SliceGame:
	 *
	 * Devide game into 2 games, original game and it's copy taking ratio of
	 * actions inside the matches to the copy. This is used when game is too large
	 * for sending the packet
	 **/
	GameData* GameData::SliceGame(int sizeToSliceInBytes) {
		// only clone the metadata of the game, without any actions
		GameData* cloneGameData = this->Clone(false);
		// if we are cloning we might have more packets and this is not the last one
		cloneGameData->m_isGameMarkedAsEnded = false;
		std::string matchGuid;

		// calculate the action percentage that we need to take from each match in
		// order to create a packet of the passed parameter size
		unsigned int gameSizeInBytes = GetGameSizeInBytes();
		float ratio = ((float)sizeToSliceInBytes / (float)gameSizeInBytes);

		logger.Log(
			LogType::DEBUG,
			"Packet too large to send, slicing Game with guid " +
			m_gameGuid);

		// run through all the game's matches and fetch the calculated ratio of their
		// actions
		for (int index = 0; index < m_matchGuidVector.size(); index++) {
			matchGuid = m_matchGuidVector[index];
			auto matchData_it = m_matchMap.find(matchGuid);
			if (matchData_it == m_matchMap.end())
				continue;

			auto cloneMatchData_it = cloneGameData->m_matchMap.find(matchGuid);
			std::vector<BaseActionData*> slicedActions;
			std::vector<ReportData*> slicedReports;
			std::vector<ChatMessageData*> slicedChatMessages;

			// grabs the ratio of full actions of the match that we will write
			// into the match copy
			matchData_it->second->SliceMatch(ratio, slicedActions);
			cloneMatchData_it->second->AddActions(slicedActions);

			// slice reports 
			matchData_it->second->SliceMatchReportVector(
				sdkConfig.maxReportsToSendAtOnce, slicedReports);
			for (auto& report : slicedReports)
				cloneMatchData_it->second->AddInMatchReport(report);

			// slice chat
			matchData_it->second->SliceMatchChatMessageVector(
				sdkConfig.maxChatMessagesToSendAtOnce, slicedChatMessages);
			for (auto& chatMessage : slicedChatMessages)
				cloneMatchData_it->second->AddChatMessage(chatMessage);
		}

		return cloneGameData;
	}

	/**
	 * CanDeleteGame:
	 *
	 * Check if we can delete the game because the client ended it, conditions:
	 * 1. Game was marked as ended by the client
	 * 2. Enough time has passed since the marking (to give time for actions to
	 * assimilate before closing)
	 * 3. Game is empty and has no action to process
	 * OR there has been enough time when have passed since game received last
	 * actions
	 **/
	bool GameData::CanDeleteGame() {
		unsigned int gameSizeInBytes = GetGameSizeInBytes();
		// Check if game is ended, has no actions and grace time to add remaining
		// actions has passed
		if (m_isGameMarkedAsEnded == true && gameSizeInBytes == 0 &&
			m_lastUpdateTime + ToSystemDuration(
				sdkConfig.gameCloseGraceAfterMarkEndInMilliseconds) <
			std::chrono::system_clock::now())
			return true;

		// Check if this game did not receive any actions for a very long long time,
		// indicating it's probably closed
		else if (gameSizeInBytes == 0 && m_lastUpdateTime + ToSystemDuration(
			sdkConfig.liveGameTimeoutInMilliseconds) <
			std::chrono::system_clock::now())
			return true;

		// game is not elidible for deletion - it did not meet the above conditions
		return false;
	}

	/**
	 * GetGameGuid:
	 *
	 **/
	std::string GameData::GetGameGuid() {
		return m_gameGuid;
	}

	/**
	 * GetTitleId:
	 *
	 **/
	int GameData::GetTitleId() {
		return m_titleId;
	}

	/**
	 * GetPrivateKey:
	 *
	 **/
	std::string GameData::GetPrivateKey() {
		return m_privateKey;
	}

	/**
	 * UpdateLastUpdateTime:
	 *
	 **/
	void GameData::UpdateLastUpdateTime() {
		m_lastUpdateTime = std::chrono::system_clock::now();
	}

	/**
	 * GetGameMatchGuids:
	 *
	 **/
	void GameData::GetGameMatchGuids(std::vector<std::string>& matchGuidVectorOut) {
		std::string matchGuid;

		for (int index = 0; index < m_matchGuidVector.size(); index++) {
			matchGuid = m_matchGuidVector[index];
			matchGuidVectorOut.push_back(matchGuid);
		}
	}

	/**
	 * GetGameMatch:
	 *
	 **/
	MatchData* GameData::GetGameMatch(std::string matchGuid) {
		auto foundMatch_it = m_matchMap.find(matchGuid);
		MatchData* MatchOut = nullptr;
		if (foundMatch_it != m_matchMap.end())
			MatchOut = foundMatch_it->second;

		return MatchOut;
	}

	/**
	 * GameToString:
	 *
	 **/
	void GameData::GameToString(std::string& gameOut, std::vector<std::string>& matchGuids) {

		std::string lastPacket = "false";
		if (m_isGameMarkedAsEnded == true)
			lastPacket = "true";

		bool containsMatch = false;

		gameOut += "{";
		gameOut += "\"privateKey\":\"" + m_privateKey + "\",";
		gameOut += "\"titleId\":" + std::to_string(m_titleId) + ",";
		gameOut += "\"gameGuid\":\"" + m_gameGuid + "\",";
		if (m_gameMode.size() > 0) gameOut += "\"gameMode\":\"" + m_gameMode + "\",";
		if (m_serverGuid.size() > 0) gameOut += "\"serverGuid\":\"" + m_serverGuid + "\",";
		if (m_serverLocation.size() > 0) gameOut += "\"serverLocation\":\"" + m_serverLocation + "\",";
		gameOut += "\"gameLastPacket\":" + lastPacket + ",";
		gameOut += "\"matches\":[\n";

		// run through all the game's matches, get their match_out string and append
		// them to the game
		for (int index = 0; index < m_matchGuidVector.size(); index++) {
			auto matchGuid = m_matchGuidVector[index];
			auto matchData_it = m_matchMap.find(matchGuid);
			if (matchData_it == m_matchMap.end() || matchData_it->second == nullptr)
				continue;

			std::string matchString;
			matchData_it->second->MatchToString(matchString);
			// in case MatchToString returned anything
			// if it is not isInteresting it will not return anything
			if (matchString.size() > 0) {
				gameOut += matchString;
				gameOut += ",";
				containsMatch = true;
				matchGuids.push_back(matchGuid);
			}
		}
		if (containsMatch || (m_isGameMarkedAsEnded == true && m_sentGameMarkedAsEnded == false)) {
			gameOut.pop_back();  // pop the last delimiter

			gameOut += "]}";
		}
		else {
			gameOut.clear();
		}
	}

	/**
	 * GetGameSizeInBytes:
	 *
	 **/
	unsigned int GameData::GetGameSizeInBytes() {
		unsigned int totalGameSize = 0;

		// traverse through all the game's matches, get their size and add it together
		// to get the total size of the current GameData object
		for (int index = 0; index < m_matchGuidVector.size(); index++) {
			auto matchGuid_it = m_matchGuidVector[index];
			auto matchData_it = m_matchMap.find(matchGuid_it);
			if (matchData_it != m_matchMap.end())
				totalGameSize += matchData_it->second->GetMatchSizeInBytes();
		}

		return totalGameSize;
	}

	/**
	 * GetNumberOfGameReportsAndMessages:
	 *
	 **/
	unsigned int GameData::GetNumberOfGameReportsAndMessages() {
		unsigned int numberOfReportAndMessages = 0;

		// traverse through all the game's matches, get their size and add it together
		// to get the total size of the current GameData object
		for (int index = 0; index < m_matchGuidVector.size(); index++) {
			auto matchGuid_it = m_matchGuidVector[index];
			auto matchData_it = m_matchMap.find(matchGuid_it);
			if (matchData_it == m_matchMap.end())
				continue;
			numberOfReportAndMessages +=
				matchData_it->second->GetNumberOfMatchReportsAndMessages();
		}

		return numberOfReportAndMessages;
	}

	/**
	 * GetMatchMap:
	 *
	 **/
	std::unordered_map<std::string, MatchData*>& GameData::GetMatchMap() {
		return m_matchMap;
	}

	/**
	 * GetServerGuid:
	 *
	 **/
	std::string GameData::GetServerGuid() {
		return m_serverGuid;
	}

	/**
	 * GetGameMode:
	 *
	 **/
	std::string GameData::GetGameMode() {
		return m_gameMode;
	}

	/**
	 * GetServerLocation:
	 *
	 **/
	std::string GameData::GetServerLocation() {
		return m_serverLocation;
	}

	/**
	 * Dispose:
	 *
	 **/
	void GameData::Dispose() {
		for (auto& matchData_it : m_matchMap) {
			if (matchData_it.second == nullptr)
				continue;
			matchData_it.second->Dispose();
			delete matchData_it.second;
			matchData_it.second = nullptr;
		}

		m_matchMap.clear();
		m_matchGuidVector.clear();
	}

	bool GameData::IsValid() {
		bool isCoreValid = Validator::ValidateStringLength(m_privateKey, 1, 100);
		isCoreValid &= Validator::ValidateStringChars(m_privateKey);
		isCoreValid &= Validator::ValidateItemValue(m_titleId, 1, INT_MAX);

		// Sanitize non-core fields (serverGuid, gameMode, serverLocation are optional but sanitized if present)
		if (!m_serverGuid.empty()) {
		    Sanitizer::SanitizeStringChars(m_serverGuid);
		    Sanitizer::SanitizeStringLength(m_serverGuid, 36);
		}
		if (!m_gameMode.empty()) {
		    Sanitizer::SanitizeStringChars(m_gameMode);
		    Sanitizer::SanitizeStringLength(m_gameMode, 36);
		}
		if (!m_serverLocation.empty()) {
		    Sanitizer::SanitizeStringChars(m_serverLocation);
		    Sanitizer::SanitizeStringLength(m_serverLocation, 36);
		}

		return isCoreValid;
	}
}  // namespace GetgudSDK
