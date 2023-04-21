#pragma once

#include "..\actions_buffer\ActionsBuffer.h"
#include "..\game_container\GameContainer.h"
#include <thread>
#include <chrono>

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
		//get_data required
	};

	class GameSender
	{
	private: //main data
		std::thread updater_thread;
		TimeLog time_log; //calculate time of operations
		//timer
		//config
		//logger
	private: //parameters
		bool stop_required;
		
	private:
		void CheckActions();
		void SendPacket(std::string& packet);

	public: //main functions
		GameSender();
		~GameSender();

		void UpdateLoop();
	};
}