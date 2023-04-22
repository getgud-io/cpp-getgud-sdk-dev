#pragma once
#include "BaseActionData.h"

namespace GetGudSdk
{
	class PositionActionData : public BaseActionData
	{
	public:
		PositionF position;
		RotationF rotation;

	public:
		PositionActionData(std::string _player_guid, std::string _match_guid,
			long _action_time_epoch, PositionF _position, RotationF _rotation);
		PositionActionData(BaseData _data_info, PositionF _position, RotationF _rotation);
		PositionActionData(PositionActionData&) = delete;
		PositionActionData() = delete;
		~PositionActionData();

		std::map<std::string, std::string> get_data() override;
		std::string get_action_stream() override;
	};
}