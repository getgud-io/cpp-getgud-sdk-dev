#pragma once
#include "BaseActionData.h"

#include <vector>

namespace GetGudSdk
{
	class SpawnActionData : public BaseActionData
	{
	public:
		PositionF position;
		RotationF rotation;
		float initial_health = 0;
		int team_id = 0;
		std::string character_guid;

	public:
		SpawnActionData(std::string _player_guid, std::string _match_guid,
			long _action_time_epoch, PositionF _position, RotationF _rotation,
			float _initial_health, int _team_id, std::string _character_guid);
		SpawnActionData(BaseData _data_info, PositionF _position, RotationF _rotation,
			float _initial_health, int _team_id, std::string _character_guid);
		SpawnActionData(SpawnActionData&) = delete;
		SpawnActionData() = delete;
		~SpawnActionData();

		std::map<std::string, std::string> get_data() override;
	};
}