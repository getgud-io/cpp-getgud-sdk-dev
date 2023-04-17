#pragma once

#include "..\..\include\actions\BaseActionData.h"
#include <deque>
#include <mutex>
#include <chrono>
#include <map>

namespace GetGudSdk
{

	class SharedBuffer
	{
	private:
		std::mutex buffer_locker;
		std::deque<BaseActionData*> buffer;

		std::mutex game_buffer_locker;
		std::deque<GameData> game_buffer;

		std::mutex match_buffer_locker;
		std::deque<MatchData> match_buffer;

		std::mutex end_game_locker;
		std::deque<std::pair<std::chrono::system_clock::time_point, std::string>>
			end_game_buffer; //first value - delay, second value - guid

		bool dispose_required = false;

	public:
		bool get_first_action(BaseActionData* out_action);
		bool get_actions(std::deque<BaseActionData*>* out_buffer);
		bool push_action(BaseActionData* in_action);
		bool push_actions(std::deque<BaseActionData*>& in_buffer);

		bool push_game(GameData game_data);

		bool push_end_game(std::string game_guid);

		bool push_match(MatchData match_data);

		void dispose();
	};
}