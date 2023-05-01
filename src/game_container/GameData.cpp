#include "GameData.h"
#include "../../include/config/Config.h"

namespace GetGudSdk
{
extern Config sdk_config;

	/**
	* GameData:
	* @_game_guid: guid of the game we are creating
	* @_title_id: title of the game we are creating
	* @_server_name: server name where the game was running
	* @_game_mode: game mode of the game
	*  
	* Game data is part of GameContainer and basically represents one packet that will be sent to
	* GetGud. Every game has certain metadata associated with it as well as N (0+) matches.
	**/
	GameData::GameData(std::string _game_guid, int _title_id, std::string _server_name, std::string _game_mode)
		: game_guid(_game_guid), title_id(_title_id), server_name(_server_name), game_mode(_game_mode)
	{
		// calculate size of the game metadata so it can be used later for total size assesment
		calculate_size();
		start_game_timer = std::chrono::system_clock::now();
	}

	GameData::~GameData()
	{

	}
	
	/**
	* calculate_size:
	*
	* Calculate size of game metadata
	**/
	void GameData::calculate_size()
	{
		size_in_bytes += game_guid.size();
		size_in_bytes += game_mode.size();
		size_in_bytes += server_name.size();
		size_in_bytes += sizeof(end_game_timer);
		size_in_bytes += sizeof(game_ended);
		size_in_bytes += sizeof(title_id);
		size_in_bytes += sizeof(size_in_bytes);
	}

	/**
	* get_game_ended:
	*
	* Check if current game is marked as ended. TODO: is_game_ended
	**/
	bool GameData::get_game_ended()
	{
		// TODO: std::chrono::system_clock::now() > end_game_timer
		// this should include delay like end_game_timer + delay_end_game_time
		if (game_ended && std::chrono::system_clock::now() > end_game_timer) 
			return true;
		else
			return false;
	}

	/**
	* get_game_guid:
	*
	**/
	std::string GameData::get_game_guid()
	{
		return game_guid;
	}

	/**
	* get_end_by_time:
	* @inactive_limit_in_sec: Max allowed time in seconds not to update game with anything
	*
	* Check if last game update was more than inactive_limit_in_sec ago
	* Which means that the game is inactivte
	**/
	bool GameData::get_end_by_time(unsigned int inactive_limit_in_sec)
	{
		if (last_update_time + std::chrono::seconds(inactive_limit_in_sec) < std::chrono::system_clock::now())
			return true; 
		else
			return false; 
	}

	/**
	* set_end_game:
	* @end_time: end time of the game we are ending
	*
	**/
	bool GameData::set_end_game(std::chrono::system_clock::time_point end_time)
	{
		// TODO: we do not need this condition, this is extra operation
		if (game_ended)
		{
			return true;
		}
		game_ended = true;
		end_game_timer = end_time;
		
		return true;
	}

	/**
	* add_match:
	* @match_data: metadata of the match we are adding to the game
	*
	* Add match instance to the game metadata object
	**/
	bool GameData::add_match(MatchData match_data)
	{
		bool result = true; // TODO: rename this variable
		std::string& key = match_data.get_match_guid(); // TODO: not key but guid

		auto match_it = matches.find(key); 

		// If match already exists in the game we don't want to add it second time
		if (match_it != matches.end()) 
		{
			result = false;
			return result;
		}

		// If match doesn't exist already add it to game matches data struct
		matches.emplace(std::make_pair(key, std::move(match_data))); 

		return result;
	}

	/**
	* game_to_string:
	* @game_out: metadata of the match we are adding to the game
	*
	* Create a full game packet of the game metadata, this includes all matches
	* and reports and chat for each match of the current game packet
	* TODO: rename to something like game_packet_to_string
	**/
	void GameData::game_to_string(std::string& game_out)
	{
    	std::string private_key = sdk_config.private_key;
		std::string last_packet;
		if (game_ended)
			last_packet = "true";
		else
			last_packet = "false";
    
		game_out += "{ \n";
    	game_out += "	\"privateKey\": \"" + private_key + "\",\n";
		game_out += "	\"titleId\": \"" + std::to_string(title_id) + "\",\n";
		game_out += "	\"gameGuid\": " + game_guid + ",\n";
		game_out += "	\"gameMode\": \"" + game_mode + "\",\n";
		game_out += "	\"gameLastPacket\": " + last_packet + ",\n";
		game_out += "	\"matches\":\n	[\n";

		for (auto& match : matches)
		{
			// TODO: add reports and chat!!!
			game_out += "	{\n";
			game_out += "	\"matchGuid\": \"" + match.first + "\",\n"; 
			game_out += "	\"mapName\": \"" + match.second.get_map_name() + "\",\n";
			game_out += "	\"matchMode\": \"" + match.second.get_match_mode() + "\",\n";
			game_out += "	\"matchActionSteam\": \""; 

			//TODO: std::deque<BaseActionData*> sorted_actions have 2 choises, should be tested
			//TODO: match.second.get_sorted_actions(sorted_actions); //make a new buffer and copy actions to sorted buffer
			match.second.sort_actions();
			for (auto& action : match.second.get_actions())
			{
				game_out += action->get_action_stream() + ",";
			}

			//TODO: game_out[game_out.size() - 1] = '\"'; //replace last ',' symbol
			game_out += "\",\n"; 
			game_out += "	},\n";
		}

		// deletes last '\n' and last ',' symbol
		game_out.pop_back(); 
		game_out.pop_back();

		game_out += "\n	]\n"; 

    game_out += "}";

	};

	/**
	* get_matches_buffer:
	*
	* Return all the matches for this game packet
	**/
	std::map<std::string, MatchData>& GameData::get_matches_buffer()
	{
		return matches;
	}


	/**
	* dispose_matches:
	*
	* Delete every match in the game packet
	**/
	void GameData::dispose_matches()
	{
		for (auto& match : matches)
		{
			match.second.dispose_actions();
		}
	}

	/**
	* update_game:
	*
	* Update size of the game packet
	* TODO: rename to update_game_packet_size
	**/
	void GameData::update_game()
	{
		last_update_time = std::chrono::system_clock::now();

		size_in_bytes = 0;
		calculate_size();

		for (auto& match : matches)
		{
			size_in_bytes += match.second.get_match_size();
		}
	}
	
	/**
	* get_game_size:
	*
	* Get size of the game metadata
	* TODO: is it size of just game metadata or the whole game packet?
	**/
	unsigned int GameData::get_game_size()
	{
		return size_in_bytes;
	}

	/**
	* get_start_time:
	*
	* Get start time of the game
	**/
	std::chrono::system_clock::time_point GameData::get_start_time()
	{
		return start_game_timer;
	}
}