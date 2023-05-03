#pragma once

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

	class PlayerUpdater
	{

	public: 

		PlayerUpdater();
		void start(int sleep_interval_milli);
		bool add_players(std::vector<PlayerInfo*> player_vector);
		~PlayerUpdater();
		dispose();	
		
	private:

		// holds all the reports being sent by the client to the SDK
		std::vector<ReportData*> player_vector;

		// helpers and data holders
		std::thread updater_thread;
		bool thread_working = false;
		TimeLog time_log; 
		curl_slist* headers = NULL; 
		CURL* curl = nullptr; 
		bool stop_required = false;
		std::mutex player_updater_mutex; 
		
	private:
		
		void init_curl();
		void send_next_player_batch();
		CURLcode send_update_players_packet(std::string& packet);
	};
}