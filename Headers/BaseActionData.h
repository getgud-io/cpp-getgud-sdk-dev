#pragma once

#include <string>
#include <map>

namespace GG_SDK
{
	struct BaseData
	{
		int action_type = 0; //replace to enum
		long action_time_epoch = 0;
		std::string player_guid;
		std::string game_guid;
		std::string match_guid;
	};

	class BaseActionData
	{
	public:
		int id = 0;
		int size = 0;
		int action_type = 0; //replace to enum
		long action_time_epoch = 0;
		std::string player_guid;
		std::string game_guid;
		std::string match_guid;

	public:
		BaseActionData(BaseData data) {};
		BaseActionData() = delete;
		BaseActionData(BaseActionData&) = delete;
		~BaseActionData() {};
		virtual std::map<std::string, std::string> get_data() {
			return std::map<std::string, std::string>();
		};
	};
}