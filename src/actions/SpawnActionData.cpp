#include "../../include/actions/SpawnActionData.h"

#include "../utils/Utils.h"

#include "../../include/config/Config.h"

using namespace GetGudSdk;

extern Config sdk_config;

/**
* SpawnActionData:
** @_data_info: Instance of the base data class which contains all the
* required fields
* @_position: <x,y,z> position of player at _action_time_epoch time
* @_rotation: <pitch, roll> of where player looks on the screen at
* _action_time_epoch time
* @_initial_health: initial health of the spawned player
* @_tead_id: team id of the spawned player
* @_character_guid: character guid of the spawned player
*
* Constructor of Spawn Action, this action is later
* passed to the action buffer
**/
SpawnActionData::SpawnActionData(BaseData _data_info, PositionF _position, RotationF _rotation,
	float _initial_health, int _team_id, std::string _character_guid) 
	: BaseActionData(_data_info) 
{
	position = _position;
	rotation = _rotation;
	initial_health = _initial_health;
	team_id = _team_id;
	character_guid = _character_guid;

	// calculate size of the action string so it can be used later for total size assesment
	calculate_size();
};

/**
* SpawnActionData:
* @_player_guid: guid of the player who performed attack action
* @_match_guid: guid of the match where the action took place
* @_action_time_epoch: timestamp of the action in the Epoch time in
* milliseconds. Example: 1643717696000
* @_position: <x,y,z> position of player at _action_time_epoch time
* @_rotation: <pitch, roll> of where player looks on the screen at
* _action_time_epoch time
* @_initial_health: initial health of the spawned player
* @_tead_id: team id of the spawned player
* @_character_guid: character guid of the spawned player
*
* Constructor of Spawn Action, this action is later
* passed to the action buffer
**/
SpawnActionData::SpawnActionData(std::string _player_guid, std::string _match_guid,
	long _action_time_epoch, PositionF _position, RotationF _rotation,
	float _initial_health, int _team_id, std::string _character_guid) :
	BaseActionData({ Actions::Spawn, _action_time_epoch, _player_guid, _match_guid }) 
{
	position = _position;
	rotation = _rotation;
	initial_health = _initial_health;
	team_id = _team_id;
	character_guid = _character_guid;

	// calculate size of the action string so it can be used later for total size assesment
	calculate_size();
};

/**
* SpawnActionData:
* @copy: Spawn action to copy
*
* Constructor of  Spawn Action, this action is later
* passed to the action buffer
**/
SpawnActionData::SpawnActionData(SpawnActionData& copy)
	: BaseActionData(copy)
{
	position = copy.position;
	rotation = copy.rotation;
	initial_health = copy.initial_health;
	team_id = copy.team_id;
	character_guid = copy.character_guid;
}


SpawnActionData::~SpawnActionData()
{

};

/**
* get_data:
*
* Fills action metadata struct from config and from fields set in action constructor
**/
std::map<std::string, std::string> SpawnActionData::get_data()
{
	std::map<std::string, std::string> data;
	const ActionStreamFileConfig& config = sdk_config.rest_api_keys; // TODO:

	// fill part of the action metadata from config
	data[config.player_guid] = player_guid;
	data[config.match_guid] = match_guid;
	data[config.action_type] = std::to_string((int)action_type);
	data[config.action_time_epoch] = std::to_string(action_time_epoch);
	data[config.position] = position_to_string(position, rotation);
	data[config.initial_health] = std::to_string(initial_health);
	data[config.team_id] = std::to_string(team_id);
	data[config.character_guid] = character_guid;

	return data;
};

/**
* get_action_stream:
*
* Transforms action metadata from map to string, so this data can be used when sending packets 
* to the server
**/
std::string SpawnActionData::get_action_stream()
{
	std::map<std::string, std::string> data = get_data();
	const ActionStreamFileConfig& config = sdk_config.rest_api_keys;
	std::string output_string;
	
	// Transform action into string in format 
	//<Timestamp>, SPAWN, <playerID>, <CharacterID>, <TeamID>, <Initial health>, <x~y~z~pitch~yaw>
	output_string += data[config.action_time_epoch] + ",";
	output_string += data[config.action_type] + ",";
	output_string += data[config.player_guid] + ",";
	output_string += data[config.character_guid] + ",";
	output_string += data[config.team_id] + ",";
	output_string += data[config.initial_health] + ",";
	output_string += data[config.position];

	return output_string;
};

/**
* clone:
*
**/
SpawnActionData* SpawnActionData::clone()
{
	return new SpawnActionData(*this);
}

/**
* calculate_size:
*
* Calculate size of the action string
**/
void SpawnActionData::calculate_size()
{	
	// We have part of the action variables in base data class
	BaseActionData::calculate_size(); 

	size += sizeof(position);
	size += sizeof(rotation);
	size += sizeof(initial_health);
	size += sizeof(team_id);
	size += character_guid.size();
}