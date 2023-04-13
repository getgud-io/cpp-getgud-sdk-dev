#include "..\..\Headers\SpawnActionData.h"

using namespace GetGudSdk;

SpawnActionData::SpawnActionData(BaseData _data_info, std::vector<float> _position,
	float _initial_health, int _team_id, std::string _character_guid) : BaseActionData(_data_info)
{
	position = _position;
	initial_health = _initial_health;
	team_id = _team_id;
	character_guid = _character_guid;
};

SpawnActionData::SpawnActionData(std::string _player_guid, std::string _game_guid, std::string _match_guid,
	Actions _action_type, long _action_time_epoch, std::vector<float> _position,
	float _initial_health, int _team_id, std::string _character_guid) :
	BaseActionData({ _action_type, _action_time_epoch, _player_guid, _game_guid, _match_guid })
{
	position = _position;
	initial_health = _initial_health;
	team_id = _team_id;
	character_guid = _character_guid;
};

SpawnActionData::~SpawnActionData()
{

};

std::map<std::string, std::string> SpawnActionData::get_data()
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
	data["initial_health"] = std::to_string(initial_health);
	data["team_id"] = std::to_string(team_id);
	data["character_guid"] = character_guid;

	return data;
};
