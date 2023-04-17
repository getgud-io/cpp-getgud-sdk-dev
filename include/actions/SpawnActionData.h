#pragma once
#include "BaseActionData.h"

#include <vector>

namespace GetGudSdk
{
	class SpawnActionData : public BaseActionData
	{
	public:
		std::vector<float> position;
		float initial_health = 0;
		int team_id = 0;
		std::string character_guid;

	public:
		SpawnActionData(std::string _player_guid, std::string _match_guid,
			long _action_time_epoch, std::vector<float> _position,
			float _initial_health, int _team_id, std::string _character_guid);
		SpawnActionData(BaseData _data_info, std::vector<float> _position,
			float _initial_health, int _team_id, std::string _character_guid);
		SpawnActionData(SpawnActionData&) = delete;
		SpawnActionData() = delete;
		~SpawnActionData();

		std::map<std::string, std::string> get_data() override;
	};
}