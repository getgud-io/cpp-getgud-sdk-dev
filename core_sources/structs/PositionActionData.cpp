#include "..\..\Headers\PositionActionData.h"

using namespace GetGudSdk;

PositionActionData::PositionActionData(BaseData _data_info, std::vector<float> _position) : BaseActionData(_data_info)
{
	position = _position;

};

PositionActionData::PositionActionData(std::string _player_guid, std::string _game_guid, std::string _match_guid,
	Actions _action_type, long _action_time_epoch, std::vector<float> _position) :
	BaseActionData({ _action_type, _action_time_epoch, _player_guid, _game_guid, _match_guid })
{
	position = _position;
};

PositionActionData::~PositionActionData()
{

};

std::map<std::string, std::string> PositionActionData::get_data()
{
	std::map<std::string, std::string> data;

	data["player_guid"] = player_guid;
	data["game_guid"] = game_guid;
	data["match_guid"] = match_guid;
	data["action_type"] = std::to_string((int)action_type);
	data["action_time_epoch"] = std::to_string(action_time_epoch);

	//making string in format "position" "[1,2,3....n]"
	std::string output_position = "[";
	for (auto coordinate : position)
	{
		output_position += std::to_string(coordinate) + ",";
	}
	output_position[output_position.size() - 1] = ']';

	data["position"] = output_position;

	return data;
};
