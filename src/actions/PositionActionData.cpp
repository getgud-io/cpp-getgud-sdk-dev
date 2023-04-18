#include "..\..\include\actions\PositionActionData.h"

#include "..\utils\Utils.h"

using namespace GetGudSdk;

PositionActionData::PositionActionData(BaseData _data_info, PositionF _position, RotationF _rotation)
	: BaseActionData(_data_info) //fill the base data
{
	//fill the action specified data
	position = _position;
	rotation = _rotation;
};

PositionActionData::PositionActionData(std::string _player_guid, std::string _match_guid,
	long _action_time_epoch, PositionF _position, RotationF _rotation) :
	BaseActionData({ Actions::Position, _action_time_epoch, _player_guid, _match_guid }) //fill the base data
{
	//fill the action specified data
	position = _position;
	rotation = _rotation;
};

PositionActionData::~PositionActionData()
{

};

std::map<std::string, std::string> PositionActionData::get_data()
{
	//make a new buffer with <key, value> pairs
	std::map<std::string, std::string> data;

	//fill the buffer by key, value
	data["player_guid"] = player_guid;
	data["game_guid"] = game_guid;
	data["match_guid"] = match_guid;
	data["action_type"] = std::to_string((int)action_type);
	data["action_time_epoch"] = std::to_string(action_time_epoch);
	data["position"] = position_to_string(position, rotation);

	return data;
};
