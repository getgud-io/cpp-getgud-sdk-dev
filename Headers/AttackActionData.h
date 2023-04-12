#pragma once
#include "BaseActionData.h"

namespace GG_SDK
{
	class AttackActionData : public BaseActionData
	{
	public:
		std::string weapon_guid;

	public:
		AttackActionData(std::string _player_guid, std::string _game_guid, std::string _match_guid,
			int _action_type, long _action_time_epoch, std::string _weapon_guid);
		AttackActionData(BaseData _data_info, std::string _weapon_guid);
		AttackActionData(AttackActionData&) = delete;
		AttackActionData() = delete;
		~AttackActionData();

		std::map<std::string, std::string> get_data() override;
	};
}