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
		std::mutex games_buffer_locker;
		std::deque<GameData> games;
		std::map<std::string, std::deque<ChatMessageData>> chat_messages;
		std::map<std::string, std::deque<ReportData>> reports;
		unsigned int game_container_size = 0;
		bool dispose_required = false;


	// Accesible through SDK
	public:
		/**
		* add_game:
		* @game_data: game metadata
		*  
		* Add new game to the game container
		**/
		bool add_game(GameData game_data);

		/**
		* set_end_game:
		* @game_guid: guid of the game that should be ended
		*  
		* End game by game_guid
		**/
		bool set_end_game(std::string game_guid);

		/**
		* add_match:
		* @game_guid: guid of the game to which we are adding match
		* @match_data: metadata of the match we are adding
		*  
		* Add new match to existing game packet
		**/
		bool add_match(std::string game_guid, MatchData match_data);

		/**
		* add_report:
		* @match_guid: guid of the match to which we are adding report
		* @report_data: report metadata we are adding
		*  
		* Add report to specific match guid in the packet
		**/
		bool add_report(std::string match_guid, ReportData match_data);

		/**
		* add_chat_message:
		* @match_guid: guid of the match to which we are adding report
		* @message_data: chat metadata we are adding
		*  
		* Add chat message to specific match guid in the packet
		**/
		bool add_chat_message(std::string match_guid, ChatMessageData match_data);

		/**
		* check_games_limit:
		*  
		* Check if we have more open games in SDK than is allowed by config
		* TODO: should not be accessible by SDK user!!
		**/
		bool check_games_limit();

		/**
		* check_matches_limit:
		*  
		* Check if we have more open matches in SDK than is allowed by config
		* TODO: wrong logic, we should add mathes from all games togetger
		* TODO: should not be accessible by SDK user!!
		**/
		bool check_matches_limit(std::string game_guid);

		/**
		* dispose_oldest_game:
		*  
		* Pick the oldest game and dispose it
		* TODO: should be replaced by queue
		* TODO: should not be accessible by SDK user!!
		**/
		bool dispose_oldest_game();

		/**
		* dispose:
		*  
		* TODO: should not be accessible by SDK user!!
		**/
		void dispose();

	// TODO: should be private or accesible only through GameSender
	public: 
		/**
		* add_actions:
		* @match_guid: guid of the match to which we are adding actions
		* @actions_buffer: deque with all actions
		*  
		* Add all actions to the match data
		**/
		bool add_actions(std::string match_guid, std::deque<BaseActionData*>& actions_buffer);
		
		/**
		* get_game_stream:
		* @stream_out: guid of the match to which we are adding actions
		* @erase: deque with all actions
		*  
		* Creates game packet for the first game in games (buffer). Can delete game packets if needed
		* TODO: why first game in games? how are they stored
		**/
		void get_game_stream(std::string& stream_out, bool erase);

		/**
		* get_container_size:
		*
		**/
		unsigned int get_container_size();

		/**
		* get_game_to_send:
		*
		* Determines the next game packet to be sent to Getgud
		**/
		GameData get_game_to_send();

#ifdef _DEBUG
		/**
		* get_buffer:
		*
		**/
		std::deque<GameData>& get_buffer();

		/**
		* get_matches:
		* @game_guid: guid of the game for which we need to get matches
		* 
		* Get matches of specific game guid
		**/
		std::map<std::string, MatchData>* get_matches(std::string game_guid);
#endif
	};
}