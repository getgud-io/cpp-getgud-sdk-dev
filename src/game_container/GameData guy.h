#pragma once

#include "MatchData.h"
#include <map>

namespace GetGudSdk
{
	class GameData
	{
	private:

		// this map will hold all the matches that are associated with this game
		std::unordered_map<std::string, std::deque<MatchData*>> aMatch_map;

		// the different meta data holders of the game 
		int aTitle_id = 0;
		std::string aPrivate_key;
		std::string aGame_guid;
		std::string aServer_guid;
		std::string aGame_mode;
		bool is_game_marked_as_ended = false;
		unsigned int size_in_bytes = 0;
		std::chrono::system_clock::time_point end_game_timer;
		std::chrono::system_clock::time_point last_update_time;

	private:

		void calculate_size();

	public:

		// initialization constructor 
		GameData(int titleId, std::string privateKey, std::string serverGuid, std::string gameMode);

		Dispose();
			
		bool MarkGameAsEnded();		
		
		bool AddMatch(MatchData matchData);
	
		bool IsGameElidibleForProcessing();

		std::string GetGameGuid();

		void GameToString(std::string& game_out);

		unsigned int GetGameSize();
	};
}