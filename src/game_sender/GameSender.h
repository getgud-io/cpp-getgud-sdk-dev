#pragma once

#include "../actions_buffer/ActionsBuffer.h"
#include "../game_container/GameContainer.h"
#include <thread>
#include <chrono>
#include <unordered_map>

#define CURL_STATICLIB
#include <curl/curl.h>

namespace GetGudSdk
{	
	// TODO: descriptions
	struct WaitTime
	{
		unsigned int average_amount = 0;
		std::chrono::duration<double, std::micro> average;
		std::chrono::duration<double, std::micro> min;
		std::chrono::duration<double, std::micro> max;
	};

	// TODO: descriptions
	struct TimeLog
	{
		WaitTime actions_locker;
		WaitTime packets_locker;
		WaitTime actions_produce;
		WaitTime packets_produce;
		WaitTime send_produce;
		//get_data required
	};

	class GameSender
	{
	private:
		std::thread updater_thread;
		bool thread_working = false;
		TimeLog time_log; //calculate time of operations
		curl_slist* headers = NULL; // TODO: descriptions
		CURL* curl = nullptr; 

	private: 
		bool stop_required = false; // TODO: descriptions
		
	private:
		/**
		* split_actions_by_match:
		* @actions_in: actions to be splitted by match, each action has match_guid
		* @actions_out: match_id and all match actions buffer
		* 
		* Split all actions from actions_in into actions_out
		* TODO: rename variables
		**/
		void split_actions_by_match(std::deque<BaseActionData*>& actions_in,
			std::unordered_map<std::string, std::deque<BaseActionData*>>& actions_out);

	private: 
		/**
		* check_actions:
		* 
		* Checks if there are any more actions in the action buffer that this game sender
		* can grab
		**/
		void check_actions();

		/**
		* update_games:
		* 
		* Check if there are any ended games that we can send.
		* TODO: the name is bad, should be send_ready_game_packet 
		**/
		void update_games();

		/**
		* append_splitted_actions:
		* @actions_in: actions splitted by match
		* 
		* Takes actions already splitted by match and puts them into respective packet part
		**/
		void append_splitted_actions(std::unordered_map<std::string, std::deque<BaseActionData*>>& actions_it);
		
		/**
		* send_packet:
		* @packet: packet ready to be sent
		* 
		* Take packet ready to be sent and send it via curl
		**/
		CURLcode send_packet(std::string& packet);

		/**
		* init_curl:
		* 
		* Prepare curl for sending game packets to Getgud
		**/
		void init_curl();

		/**
		* start_thread:
		* 
		* Prepare curl for sending game packets to Getgud
		**/
		void start_thread();

	// TODO: should not be accessible in SDK
	public: 
		/**
		* GameSender:
		* 
		* Starts new game sender thread
		**/
		GameSender();

		/**
		* GameSender:
		* 
		* Stop game sender thread
		**/
		~GameSender();
		
		/**
		* update_loop:
		* 
		* One iteration of game sender. Check for new actions, check if there are any games ready
		* for sending
		**/
		void update_loop();
	};
}