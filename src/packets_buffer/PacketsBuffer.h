#pragma once
#include <mutex>
#include <deque>
#include "GameData.h"

namespace GetGudSdk
{
	class PacketsBuffer
	{
	private:
		std::mutex buffer_locker;
		std::deque<GameData> buffer;

		std::mutex end_game_locker;
		std::deque<std::pair<std::chrono::system_clock::time_point, std::string>>
			end_game_buffer; //first value - delay, second value - guid

		bool dispose_required = false;

	public:
		bool push_game(GameData game_data);

		bool push_end_game(std::string game_guid);

		bool push_match(std::string game_guid, MatchData match_data);

		void dispose();
	};
}