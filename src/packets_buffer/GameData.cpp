#include "GameData.h"

namespace GetGudSdk
{
	GameData::GameData(std::string _game_guid, int _title_id, std::string _server_name, std::string _game_mode)
		: game_guid(_game_guid), title_id(_title_id), server_name(_server_name), game_mode(_game_mode)
	{

	}

	GameData::~GameData()
	{
		//pointers delete function.
	}

	std::string GameData::get_game_guid()
	{
		return game_guid;
	}

	bool GameData::set_end_game(std::chrono::system_clock::time_point end_time)
	{
		if (game_ended) //if game ended already then return true
		{
			return true;
		}

		game_ended = true;

		end_game_timer = end_time;
		
		return true;
	}

	bool GameData::push_match(MatchData match_data)
	{
		bool result = true;
		//push key, match_data pair
		std::string& key = match_data.get_match_guid();

		auto match_it = matches.find(key); //find match guid

		if (match_it != matches.end()) //check if guid already exists
		{
			result = false; //return error
			return result;
		}

		matches.emplace(std::make_pair(key, std::move(match_data))); //add new guid and match pair

		return result;
	}
}