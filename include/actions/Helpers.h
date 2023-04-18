#pragma once

#include <string>

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

	struct PositionF
	{
		float x = 0;
		float y = 0;
		float z = 0;
	};

	struct RotationF
	{
		float Pitch = 0;
		float Roll = 0;
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
}