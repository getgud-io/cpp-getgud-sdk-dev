#pragma once

#include "BaseActionData.h"

namespace GetGudSdk
{
	class DeathActionData : public BaseActionData
	{
	public:
		DeathActionData(std::string _player_guid, std::string _match_guid,
			long _action_time_epoch);
		DeathActionData(BaseData _data_info);
		DeathActionData(DeathActionData&) = delete;
		DeathActionData() = delete;
		~DeathActionData();

		std::map<std::string, std::string> get_data() override;
		std::string get_action_stream() override;
	};
}