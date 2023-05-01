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
		std::chrono::system_clock::time_point last_update_time;
		std::chrono::system_clock::time_point start_game_timer;
		bool game_ended = false;
		int title_id = 0;
		unsigned int size_in_bytes = 0; // TODO: game_size_in_bytes
		std::map<std::string, MatchData> matches;

	private:
		/**
		* calculate_size:
		*
		* Calculate size of game metadata
		**/
		void calculate_size();

	public:
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
		GameData(std::string _game_guid, int _title_id, std::string _server_name, std::string _game_mode);
		
		GameData() {};
		~GameData();
		
	// SDK can access
	public: 
		/**
		* set_end_game:
		* @end_time: end time of the game we are ending
		*
		**/
		bool set_end_game(std::chrono::system_clock::time_point end_time);
		
		/**
		* add_match:
		* @match_data: metadata of the match we are adding to the game
		*
		* Add match instance to the game metadata object
		**/
		bool add_match(MatchData match_data);


	// TODO: private. GameSender access only
	public:
		/**
		* get_game_ended:
		*
		* Check if current game is marked as ended. TODO: is_game_ended
		**/
		bool get_game_ended();

		/**
		* get_end_by_time:
		* @inactive_limit_in_sec: Max allowed time in seconds not to update game with anything
		*
		* Check if last game update was more than inactive_limit_in_sec ago
		* Which means that the game is inactivte
		**/
		bool get_end_by_time(unsigned int inactive_limit_in_sec);

		/**
		* get_game_guid:
		*
		**/
		std::string get_game_guid();

		/**
		* get_matches_buffer:
		*
		* Return all the matches for this game packet
		**/
		std::map<std::string, MatchData>& get_matches_buffer();

		/**
		* game_to_string:
		* @game_out: metadata of the match we are adding to the game
		*
		* Create a full game packet of the game metadata, this includes all matches
		* and reports and chat for each match of the current game packet
		* TODO: rename to something like game_packet_to_string
		**/
		void game_to_string(std::string& game_out);

		/**
		* dispose_matches:
		*
		* Delete every match in the game packet
		**/
		void dispose_matches();

		/**
		* update_game:
		*
		* Update size of the game packet
		* TODO: rename to update_game_packet_size
		**/
		void update_game();

		/**
		* get_game_size:
		*
		* Get size of the game metadata
		* TODO: is it size of just game metadata or the whole game packet?
		**/
		unsigned int get_game_size();

		/**
		* get_start_time:
		*
		* Get start time of the game
		**/
		std::chrono::system_clock::time_point get_start_time();
	};
}