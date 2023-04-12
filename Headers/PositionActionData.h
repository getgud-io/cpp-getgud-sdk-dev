#pragma once
#include "BaseActionData.h"

#include <vector>

namespace GG_SDK
{
	class PositionActionData : public BaseActionData
	{
	public:
		std::vector<float> position;

	public:
		PositionActionData(std::string _player_guid, std::string _game_guid, std::string _match_guid,
			int _action_type, long _action_time_epoch, std::vector<float> _position);
		PositionActionData(BaseData _data_info, std::vector<float> _position);
		PositionActionData(PositionActionData&) = delete;
		PositionActionData() = delete;
		~PositionActionData();

		std::map<std::string, std::string> get_data() override;
	};
}