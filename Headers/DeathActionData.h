#pragma once

#include "BaseActionData.h"

namespace GG_SDK
{
	class DeathActionData : public BaseActionData
	{
	public:
		DeathActionData(std::string _player_guid, std::string _game_guid, std::string _match_guid,
			int _action_type, long _action_time_epoch) : BaseActionData(BaseData{}) {};
		DeathActionData(BaseData _data_info) : BaseActionData(_data_info) {};
		DeathActionData(DeathActionData&) = delete;
		DeathActionData() = delete;
		~DeathActionData() {};

		std::map<std::string, std::string> get_data() override { return std::map<std::string, std::string>(); };
	};
}