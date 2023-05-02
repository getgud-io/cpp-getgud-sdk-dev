#pragma once

#include "../actions_buffer/ActionsBuffer.h"
#include "../game_container/GameContainer.guy.h"
#include <thread>
#include <chrono>
#include <unordered_map>

#define CURL_STATICLIB
#include <curl/curl.h>

namespace GetGudSdk
{	
	struct WaitTime
	{
		unsigned int average_amount = 0;
		std::chrono::duration<double, std::micro> average;
		std::chrono::duration<double, std::micro> min;
		std::chrono::duration<double, std::micro> max;
	};

	struct TimeLog
	{
		WaitTime actions_locker;
		WaitTime packets_locker;
		WaitTime actions_produce;
		WaitTime packets_produce;
		WaitTime send_produce;
	};

	class GameSender
	{

	public: 

		GameSender();
		void start(int sleep_interval_milli);
		~GameSender();
		dispose();	
		
	private:

		std::thread updater_thread;
		bool thread_working = false;
		TimeLog time_log; 
		curl_slist* headers = NULL; 
		CURL* curl = nullptr; 
		bool stop_required = false; 
		
	private:
		
		void init_curl();
		void send_next_game();
		CURLcode send_game_packet(std::string& packet);

	};
}