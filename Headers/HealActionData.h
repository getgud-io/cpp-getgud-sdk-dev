#pragma once
#include "BaseActionData.h"

namespace GetGudSdk
{
	class HealActionData : public BaseActionData
	{
	public:
		float health_gained = 0;

	public:
		HealActionData(std::string _player_guid, std::string _game_guid, std::string _match_guid,
			Actions _action_type, long _action_time_epoch, float _health_gained);
		HealActionData(BaseData _data_info, float _health_gained);
		HealActionData(HealActionData&) = delete;
		HealActionData() = delete;
		~HealActionData();

		std::map<std::string, std::string> get_data() override;
	};
}