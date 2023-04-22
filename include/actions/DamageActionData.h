#pragma once
#include "BaseActionData.h"

namespace GetGudSdk
{
	class DamageActionData : public BaseActionData
	{
	public:
		float damage_done = 0;
		std::string victim_player_guid;
		std::string weapon_guid;

	public:
		DamageActionData(std::string _player_guid, std::string _match_guid,
			long _action_time_epoch, float _damage_done, std::string _victim_player_guid,
			std::string _weapon_guid);
		DamageActionData(BaseData _data_info, float _damage_done, std::string _victim_player_guid,
			std::string _weapon_guid);
		DamageActionData(DamageActionData&) = delete;
		DamageActionData() = delete;
		~DamageActionData();

		std::map<std::string, std::string> get_data() override;
		std::string get_action_stream() override;
	};
}