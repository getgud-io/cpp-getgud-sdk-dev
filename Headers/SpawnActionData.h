#pragma once
#include "BaseActionData.h"

#include <vector>

namespace GG_SDK
{
	class SpawnActionData : public BaseActionData
	{
	public:
		std::vector<float> position;
		int initial_health = 0;
		int team_id = 0;
		std::string character_guid;

	public:
		SpawnActionData(std::string _player_guid, std::string _game_guid, std::string _match_guid,
			int _action_type, long _action_time_epoch, std::vector<float> _position,
			int _initial_health, int team_id, std::string character_guid);
		SpawnActionData(BaseData data_info, std::vector<float> _position,
			int _initial_health, int team_id, std::string character_guid);
		SpawnActionData(SpawnActionData&) = delete;
		SpawnActionData() = delete;
		~SpawnActionData();

		std::map<std::string, std::string> get_data() override;
	};
}