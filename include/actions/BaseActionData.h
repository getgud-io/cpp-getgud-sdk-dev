#pragma once

#include <string>
#include <map>

#include "Helpers.h"

namespace GetGudSdk
{

	class BaseActionData
	{
	public:
		int id = 0;
		int size = 0;
		Actions action_type = Actions::none; //replace to enum
		long action_time_epoch = 0;
		std::string player_guid;
		//std::string game_guid; doesnt use on this stage
		std::string match_guid;

	public:
		BaseActionData(BaseData data);
		BaseActionData() = delete;
		BaseActionData(BaseActionData&) = delete;
		~BaseActionData();
		bool operator()(BaseActionData& a, BaseActionData& b);
		virtual std::map<std::string, std::string> get_data();
		virtual std::string get_action_stream();
	};
}