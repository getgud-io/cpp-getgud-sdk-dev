#include "..\..\include\actions\SpawnActionData.h"

#include "..\utils\Utils.h"

using namespace GetGudSdk;

SpawnActionData::SpawnActionData(BaseData _data_info, PositionF _position, RotationF _rotation,
	float _initial_health, int _team_id, std::string _character_guid) 
	: BaseActionData(_data_info) //fill the base data
{
	//fill the action specified data
	position = _position;
	rotation = _rotation;
	initial_health = _initial_health;
	team_id = _team_id;
	character_guid = _character_guid;
};

SpawnActionData::SpawnActionData(std::string _player_guid, std::string _match_guid,
	long _action_time_epoch, PositionF _position, RotationF _rotation,
	float _initial_health, int _team_id, std::string _character_guid) :
	BaseActionData({ Actions::Spawn, _action_time_epoch, _player_guid, _match_guid }) //fill the base data
{
	//fill the action specified data
	position = _position;
	rotation = _rotation;
	initial_health = _initial_health;
	team_id = _team_id;
	character_guid = _character_guid;
};

SpawnActionData::~SpawnActionData()
{

};

std::map<std::string, std::string> SpawnActionData::get_data()
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
	data["initial_health"] = std::to_string(initial_health);
	data["team_id"] = std::to_string(team_id);
	data["character_guid"] = character_guid;

	return data;
};
