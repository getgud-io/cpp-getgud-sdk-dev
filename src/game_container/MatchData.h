#pragma once

#include <deque>
#include <mutex>
#include <set>
#include <vector>
#include "../../include/actions/BaseActionData.h"
#include "ChatMessageData.h"
#include "ReportData.h"
#include <unordered_map>

namespace GetgudSDK {
	class MatchData {
	private:
		// holds all the actions (in order) that arrived from the client and are
		// waiting to be processed
		std::vector<BaseActionData*> m_actionVector;

		// holds all the in match reports that are waiting to be sent
		std::vector<ReportData*> m_reportVector;

		// holds all the chat massages that are waiting to be send
		std::vector<ChatMessageData*> m_chatMessageVector;

		std::string m_matchGuid;
		std::string m_gameGuid;
		std::string m_matchMode;
		std::string m_mapName;
		std::string m_matchWinTeamGuid;
		std::string m_customField;
		bool m_throttleChecked = false;
		bool m_isInteresting = false;
		std::set<std::string> m_playerGuids;
		unsigned int m_actionsCount = 0;
		unsigned int m_sizeInBytes = 0;

		// holds the player Key to player Guid mapping
		std::unordered_map<std::string, std::string> m_playerGuidMap;

		// used to generate new player keys
		int m_nextPlayerKey = 0;

		// holds the weapon Key to weapon Guid mapping
		std::unordered_map<std::string, std::string> m_weaponGuidMap;

		// used to generate new weapon keys
		int m_nextWeaponKey = 0;

		// holds the map action completion state
		MatchCompletionState m_matchCompletionState = MatchCompletionState::Complete;

	public:
		MatchData(std::string gameGuid, std::string matchMode, std::string mapName, std::string m_customField);
		MatchData(const MatchData& data);
		MatchData() = delete;
		~MatchData();
		MatchData* Clone(bool isWithActions);
		void AddActions(std::vector<BaseActionData*>& actionVector);
		void SliceMatch(float ratioToSlice,
			std::vector<BaseActionData*>& toActionVector);
		void SliceMatchReportVector(int numberToSlice,
			std::vector<ReportData*>& toReportVector);
		void SliceMatchChatMessageVector(int numberToSlice,
			std::vector<ChatMessageData*>& toChatMessageVector);
		bool AddInMatchReport(ReportInfo reportInfo);
		bool AddChatMessage(ChatMessageInfo chatInfo);
		bool AddInMatchReport(ReportData* reportData);
		bool AddChatMessage(ChatMessageData* chatMessageData);
		void MatchToString(std::string& match_out);
		unsigned int GetMatchSizeInBytes();
		unsigned int GetNumberOfMatchReportsAndMessages();
		std::string GetMatchGuid();
		std::string GetGameGuid();
		void SetThrottleCheckResults(bool inThrottleChecked, bool inIsInteresting);
		bool GetThrottleChecked();
		std::set<std::string> GetPlayerGuids();
		std::vector<ReportData*> GetReportVector();
		std::vector<ChatMessageData*> GetChatMessageVector();
		std::string GetMatchMode();
		std::string GetMapName();
		std::string GetCustomField();
		void SetMatchWinTeam(std::string teamGuid);
		std::string getPlayerKeyName(std::string& playerGuid);
		std::string getWeaponKeyName(std::string& weaponGuid);
		std::string  GetPlayerMapString();
		std::string  GetWeaponMapString();
		void SetMatchIncompleteState(MatchCompletionState state);
		void Dispose();
		bool IsValid();
	};
}  // namespace GetgudSDK
