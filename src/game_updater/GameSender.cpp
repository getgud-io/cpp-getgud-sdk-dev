#include "GameSender.h"

namespace GetGudSdk
{
	GameSender game_updater;

	GameSender::GameSender()
	{
		updater_thread = std::thread([&]() { UpdateLoop(); });//implementation of loop required
	}

	GameSender::~GameSender()
	{
		stop_required = true;
		updater_thread.join();
	}

	void GameSender::UpdateLoop()
	{
		//check actions buffer
			//get actions and push
			//send packets
		//check packets timer
		//check messages
	}
}