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

		bool dispose_required = false;

	public:
		bool push_game(GameData game_data);

		bool push_end_game(std::string game_guid);

		bool push_match(std::string game_guid, MatchData match_data);

		void dispose();

#ifdef _DEBUG
		std::deque<GameData>& get_buffer();
#endif
	};
}