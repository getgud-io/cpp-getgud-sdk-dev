#pragma once
#include <mutex>
#include <deque>
#include <map>
#include "GameData.h"

namespace GetGudSdk
{
	class GameContainer
	{
	private:
		
		// fast access map for when we need the GameData via game guid 
		std::unordered_map<std::string, std::deque<GameData*>> aGame_map;
		
		// fast access map for when we need the MatchData via match guid
		std::unordered_map<std::string, std::deque<MatchData*>> aMatch_map;
		
		// holds the order of the games as they need to be processed (first game to be processed resides at the end of the vector)
		std::vector<GameData*> aGame_vector;

		// helpers and data holders
		unsigned int aGame_container_size = 0;
		bool aDispose_required = false;
		std::mutex aGame_container_mutex;

	public:

		bool AddGame(int titleId, std::string privateKey, std::string serverGuid, std::string gameMode);

		bool AddMatch(std::string gameGuid, std::string matchMode, std::string mapName);

		bool AddActions(std::string matchGuid, std::vector<BaseActionData*>& actionVector);

		bool AddInMatchReport(std::string matchGuid, ReportData reportData);

		bool AddChatMessage(std::string matchGuid, ChatMessageData matchData);

		GameData PopNextGameDataToSend();

		bool MarkEndGame(std::string gameGuid);

		bool DeleteGame(std::string gameGuid);

		void Dispose();
	};
}