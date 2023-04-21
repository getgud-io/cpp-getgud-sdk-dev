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
		std::mutex buffer_locker;
		std::deque<GameData> buffer;
		std::map<std::string, std::deque<ChatMessageData>> chat_messages;
		std::map<std::string, std::deque<ReportData>> reports;

		bool dispose_required = false;

	public:
		bool add_game(GameData game_data);

		bool set_end_game(std::string game_guid);

		bool add_match(std::string game_guid, MatchData match_data);

		bool add_report(std::string match_guid, ReportData match_data);

		bool add_chat_message(std::string match_guid, ChatMessageData match_data);

		void dispose();

#ifdef _DEBUG
		std::deque<GameData>& get_buffer();
		std::map<std::string, MatchData>* get_matches(std::string game_guid);
#endif
	};
}