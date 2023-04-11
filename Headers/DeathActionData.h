#pragma once

#include "BaseActionData.h"

class DeathActionData : public BaseActionData
{
public:
	DeathActionData(std::string _player_guid, std::string _game_guid, std::string _match_guid,
		int _action_type, long _action_time_epoch);
	DeathActionData() = delete;
	~DeathActionData();

	std::map<std::string, std::string> get_data() override;
};