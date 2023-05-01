#include "../../include/actions/PositionActionData.h"

#include "../utils/Utils.h"

#include "../../include/config/Config.h"

using namespace GetGudSdk;

extern Config sdk_config;

/**
* PositionActionData:
* @_data_info: Instance of the base data class which contains all the
* required fields
* @_position: <x,y,z> position of player at _action_time_epoch time
* @_rotation: <pitch, roll> of where player looks on the screen at
* _action_time_epoch time
*
* Constructor of  Position Action, this action is later
* passed to the action buffer
**/
PositionActionData::PositionActionData(BaseData _data_info, PositionF _position, RotationF _rotation)
	: BaseActionData(_data_info) 
{
	position = _position;
	rotation = _rotation;

	// calculate size of the action string so it can be used later for total size assesment
	calculate_size();
};

/**
* PositionActionData:
* @_player_guid: guid of the player who performed attack action
* @_match_guid: guid of the match where the action took place
* @_action_time_epoch: timestamp of the action in the Epoch time in
* milliseconds. Example: 1643717696000
* @_position: <x,y,z> position of player at _action_time_epoch time
* @_rotation: <pitch, roll> of where player looks on the screen at
* _action_time_epoch time
*
* Constructor of  Position Action, this action is later
* passed to the action buffer
**/
PositionActionData::PositionActionData(std::string _player_guid, std::string _match_guid,
	long _action_time_epoch, PositionF _position, RotationF _rotation) :
	BaseActionData({ Actions::Position, _action_time_epoch, _player_guid, _match_guid }) //fill the base data
{
	position = _position;
	rotation = _rotation;

	// calculate size of the action string so it can be used later for total size assesment
	calculate_size();
};

/**
* PositionActionData:
* @copy: Position action to copy
*
* Constructor of  Position Action, this action is later
* passed to the action buffer
**/
PositionActionData::PositionActionData(PositionActionData& copy)
	: BaseActionData(copy)
{
	position = copy.position;
	rotation = copy.rotation;
}

PositionActionData::~PositionActionData()
{

};

/**
* get_data:
*
* Fills action metadata struct from config and from fields set in action constructor
**/
std::map<std::string, std::string> PositionActionData::get_data()
{
	std::map<std::string, std::string> data;
	const ActionStreamFileConfig& config = sdk_config.rest_api_keys; // TODO: why rest api keys??

	// fill part of the action metadata from config
	data[config.player_guid] = player_guid;
	data[config.match_guid] = match_guid;
	data[config.action_type] = std::to_string((int)action_type);
	data[config.action_time_epoch] = std::to_string(action_time_epoch);
	data[config.position] = position_to_string(position, rotation);

	return data;
};

/**
* get_action_stream:
*
* Transforms action metadata from map to string, so this data can be used when sending packets 
* to the server
**/
std::string PositionActionData::get_action_stream()
{
	std::map<std::string, std::string> data = get_data();
	const ActionStreamFileConfig& config = sdk_config.rest_api_keys; // TODO: why rest api keys?
	std::string output_string;

	// Transform action into string in format
	//<Timestamp>, POSITION, <playerID>, <x~y~z~pitch~yaw>

	output_string += data[config.action_time_epoch] + ",";
	output_string += data[config.action_type] + ",";
	output_string += data[config.player_guid] + ",";
	output_string += data[config.position];

	return output_string;
};

/**
* clone:
*
**/
PositionActionData* PositionActionData::clone()
{
	return new PositionActionData(*this);
}

/**
* calculate_size:
*
* Calculate size of the action string
**/
void PositionActionData::calculate_size()
{
	// We have part of the action variables in base data class
	BaseActionData::calculate_size(); 
	
	size += sizeof(position);
	size += sizeof(rotation);
}