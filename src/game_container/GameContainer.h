#pragma once
#include <mutex>
#include <unordered_map>
#include <vector>
#include "../utils/Utils.h"
#include "GameData.h"

namespace GetgudSDK {
	class GameContainer {
	private:
		// fast access map for when we need the GameData via game guid
		std::unordered_map<std::string, GameData*> m_gameMap;

		// fast access map for when we need the MatchData via match guid - this is
		// abusing game encapsulation but needed for speed
		std::unordered_map<std::string, MatchData*> m_matchMap;

		// holds the GameData and the order of the games as they need to be processed
		// (first game to be processed resides at the beginning of the vector)
		std::vector<GameData*> m_gameVector;

		// helpers and data holders
		unsigned int m_gameContainerSizeInBytes = 0;
		FilledAverage m_averageSize;

	public:
		std::mutex m_gameContainerMutex;

	public:
		std::string AddGame(int titleId,
			std::string privateKey,
			std::string serverGuid,
			std::string gameMode,
			std::string serverLocation);
		std::string AddMatch(std::string gameGuid,
			std::string matchMode,
			std::string mapName,
			std::string customField);
		bool AddActions(std::deque<BaseActionData*>& actionVector);
		bool AddInMatchReport(ReportInfo reportData);
		bool AddChatMessage(std::string matchGuid, ChatMessageInfo chatData);
		unsigned int GetSizeInBytes();
		unsigned int GetAverageSizeInBytes();
		GameData* PopNextGameToProcess();
		bool MarkEndGame(std::string gameGuid);
		void SentGameMarkedAsEnded(std::string gameGuid);
		void MarkGameMatchesAsNotInteresting(std::string gameGuid, std::vector<std::string>& matchGuids);
		bool SetMatchWinTeam(std::string matchGuid, std::string teamGuid);
		bool DeleteGame(std::string gameGuid, bool externalCall);
		void Dispose();
		std::unordered_map<std::string, MatchData*>& GetMatchMap();
		bool SendThrottleCheckForMatch(std::string& packet);

	private:
		bool DeleteGame(std::string gameGuid,
			bool externalCall,
			std::vector<MatchData*>& matchPtrVector);
	};
}  // namespace GetgudSDK