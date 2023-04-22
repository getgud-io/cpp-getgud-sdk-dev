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

		bool dispose_required = false;

		std::string private_key = "c3b6ff90-5b1e-11ed-9c6e-1d473e0b061e"; //will be replaced by config

	public: //sdk functions
		bool add_game(GameData game_data);

		bool set_end_game(std::string game_guid);

		bool add_match(std::string game_guid, MatchData match_data);

		bool add_report(std::string match_guid, ReportData match_data);

		bool add_chat_message(std::string match_guid, ChatMessageData match_data);

		void dispose();

	public: //GameSender functions
		bool add_actions(std::string match_guid, std::deque<BaseActionData*>& actions_buffer);
		bool add_sorted_actions(std::string match_guid, std::deque<BaseActionData*>& actions_buffer);
		void get_game_stream(std::string& stream_out, bool erase);

#ifdef _DEBUG
		std::deque<GameData>& get_buffer();
		std::map<std::string, MatchData>* get_matches(std::string game_guid);
#endif
	};
}