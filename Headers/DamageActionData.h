#pragma once
#include "BaseActionData.h"

namespace GG_SDK
{
	class DamageActionData : public BaseActionData
	{
	public:
		int damage_done = 0;
		std::string victim_player_guid;
		std::string weapon_guid;

	public:
		DamageActionData(std::string _player_guid, std::string _game_guid, std::string _match_guid,
			int _action_type, long _action_time_epoch, int _damage_done, std::string _victim_player_guid,
			std::string _weapon_guid);
		DamageActionData(BaseData data_info, int _damage_done, std::string _victim_player_guid,
			std::string _weapon_guid);
		DamageActionData(DamageActionData&) = delete;
		DamageActionData() = delete;
		~DamageActionData();

		std::map<std::string, std::string> get_data() override;
	};
}