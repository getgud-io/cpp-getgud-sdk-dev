#pragma once

#include "../../include/actions/BaseActionData.h"
#include "ChatMessageData.h"
#include "ReportData.h"
#include <deque>
#include <mutex>

namespace GetGudSdk
{
	class MatchData
	{
	private:
		std::string match_guid;
		std::string match_mode;
		std::string map_name;
		unsigned int actions_size = 0;
		unsigned int size_in_bytes = 0;
		//need reports list
		//need message list
		std::deque<BaseActionData*> match_actions;

	public:
		MatchData(std::string _match_guid, std::string _match_mode, std::string _map_name);
		~MatchData();
		std::string& get_match_guid();
		std::string& get_map_name();
		std::string& get_match_mode();

	public: //game sender functions
		bool push_actions(std::deque<BaseActionData*>& _actions);
		bool push_sorted_actions(std::deque<BaseActionData*>& _actions);
		std::deque<BaseActionData*>& get_actions();
		void get_sorted_actions(std::deque<BaseActionData*>& actions_out);
		void sort_actions();
	};
}