#pragma once

#include <string>
#include <map>

namespace GetGudSdk
{
	enum Actions : unsigned int
	{
		none = 0,
		Attack,
		Damage,
		Death,
		Heal,
		Position,
		Spawn
	};

	struct GameData
	{
		std::string game_guid; //auto-generated value
		int title_id = 0;
		std::string server_name;
		std::string game_mode;
	};

	struct MatchData
	{
		std::string match_guid; //auto-generated value
		std::string game_guid;
		std::string match_mode;
		std::string map_name;
	};

	struct BaseData
	{
		Actions action_type = Actions::none; //replace to enum
		long action_time_epoch = 0;
		std::string player_guid;
		std::string match_guid;
	};

	class BaseActionData
	{
	public:
		int id = 0;
		int size = 0;
		Actions action_type = Actions::none; //replace to enum
		long action_time_epoch = 0;
		std::string player_guid;
		std::string game_guid;
		std::string match_guid;

	public:
		BaseActionData(BaseData data);
		BaseActionData() = delete;
		BaseActionData(BaseActionData&) = delete;
		~BaseActionData();
		virtual std::map<std::string, std::string> get_data();
	};
}