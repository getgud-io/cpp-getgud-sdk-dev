#pragma once

#include "MatchData.h"

#include <map>

namespace GetGudSdk
{
	class GameData
	{
	private:
		std::string game_guid;
		std::string game_mode;
		std::string server_name;
		std::chrono::system_clock::time_point end_game_timer;
		bool game_ended = false;
		int title_id = 0;
		//std::map<std::string, MatchData> matches;	//binary buffer with O(log n) search
													//key = match_guid, value = MatchData
	public:
		GameData(std::string _game_guid, int _title_id, std::string _server_name, std::string _game_mode);
		~GameData();
		
	public:
		std::string get_game_guid();
		bool set_end_game(std::chrono::system_clock::time_point end_time);
		bool push_match(MatchData match_data);
		std::map<std::string, MatchData> matches;
	};
}