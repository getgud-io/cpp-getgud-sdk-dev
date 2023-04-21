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

	struct ReportInfo
	{
		std::string match_guid;
		std::string reporter_name;
		int reporter_type = 0;
		int reporter_sub_type = 0;
		std::string suspected_player_name;
		int tb_type = 0;
		int tb_sub_type = 0;
		long tb_time_epoch = 0;
		int suggested_toxicity_score = 0;
		long reported_time_epoch = 0;
	};

	struct BaseData
	{
		Actions action_type = Actions::none; //replace to enum
		long action_time_epoch = 0;
		std::string player_guid;
		std::string match_guid;
	};
}