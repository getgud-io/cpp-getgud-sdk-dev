#include "GameData.h"

namespace GetGudSdk
{
	GameData::GameData(std::string _game_guid, int _title_id, std::string _server_name, std::string _game_mode)
		: game_guid(_game_guid), title_id(_title_id), server_name(_server_name), game_mode(_game_mode)
	{

	}

	GameData::~GameData()
	{
		//verify that containers will be empty
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

	bool GameData::add_match(MatchData match_data)
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

	void GameData::game_to_string(std::string& game_out)
	{
		//{
		//		"privateKey": "c3b6ff90-5b1e-11ed-9c6e-1d473e0b061e",
		//		"titleId" : 24,
		//		"gameGuid" : "549cf69d-0d55-4849-b2d1-a49a4f0a0b1a",
		//		"gameMode" : "Guys_Mode!",
		//		"gameLastPacket" : true,
		//		"matches" :
		//		[

		game_out += "	\"titleId\": \"" + std::to_string(title_id) + "\",\n";
		game_out += "	\"gameGuid\": " + game_guid + ",\n";
		game_out += "	\"gameMode\": \"" + game_mode + "\",\n";
		game_out += "	\"gameLastPacket\": " + std::string("true") + ",\n";
		game_out += "	\"matches\":\n	[\n";

		for (auto& match : matches)
		{
			//	"matchGuid": "8a600280-33d2-44cf-9eb7-06a497fc219e",
			//	"mapName" : "Dust",
			//	"matchMode" : "night",
			game_out += "	{\n";
			game_out += "	\"matchGuid\": \"" + match.first + "\",\n"; //first value is match_guid
			game_out += "	\"mapName\": \"" + match.second.get_map_name() + "\",\n";
			game_out += "	\"matchMode\": \"" + match.second.get_match_mode() + "\",\n";
			game_out += "	\"matchActionSteam\": \""; //stream start

			//std::deque<BaseActionData*> sorted_actions; //have 2 choises, should be tested
			//match.second.get_sorted_actions(sorted_actions); //make a new buffer and copy actions to sorted buffer
			match.second.sort_actions();
			for (auto& action : match.second.get_actions())
			{
				game_out += action->get_action_stream() + ",";
			}
			//no need by syntax
			//game_out[game_out.size() - 1] = '\"'; //replace last ',' symbol
			game_out += "\",\n"; //stream end
			game_out += "	},\n";
		}
		game_out.pop_back(); //delete last '\n'
		game_out.pop_back(); //delete last ','

		game_out += "\n	]\n"; //end matches

	};

	std::map<std::string, MatchData>& GameData::get_matches_buffer()
	{
		return matches;
	}
}