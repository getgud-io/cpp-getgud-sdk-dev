#include "../../include/actions/AttackActionData.h"

#include "../../include/config/Config.h"

using namespace GetGudSdk;

extern Config sdk_config;

/**
* AttackActionData:
* @_data_info: Instance of the base data class which contains all the
* required fields
* @_weapon_guid: weapon_guid of the weapon used during the action
*
* Constructor of  Attack Action, this action is later
* passed to the action buffer
**/
AttackActionData::AttackActionData(BaseData _data_info, std::string _weapon_guid)
	: BaseActionData(_data_info) 
{
	weapon_guid = _weapon_guid;

	// calculate size of the action string so it can be used later for total size assesment
	calculate_size();
};

/**
* AttackActionData:
* @_player_guid: guid of the player who performed attack action
* @_match_guid: guid of the match where the action took place
* @_action_time_epoch: timestamp of the action in the Epoch time in
* milliseconds. Example: 1643717696000
* @_weapon_guid: guid of the weapon used during the action
*
* Constructor of  Attack Action, this action is later
* passed to the action buffer
**/
AttackActionData::AttackActionData(std::string _player_guid, std::string _match_guid,
	long _action_time_epoch, std::string _weapon_guid):
	BaseActionData({ Actions::Attack, _action_time_epoch, _player_guid, _match_guid }) 
{
	weapon_guid = _weapon_guid;

	// calculate size of the action string so it can be used later for total size assesment
	calculate_size();
};

/**
* AttackActionData:
* @copy: Attack action to copy
*
* Constructor of  Attack Action, this action is later
* passed to the action buffer
**/
AttackActionData::AttackActionData(AttackActionData& copy)
	: BaseActionData(copy)
{
	weapon_guid = copy.weapon_guid;
}

AttackActionData::~AttackActionData()
{

};

/**
* get_data:
*
* Fills action metadata struct from config and from fields set in action constructor
**/
std::map<std::string, std::string> AttackActionData::get_data()
{
	std::map<std::string, std::string> data;
	const ActionStreamFileConfig& config = sdk_config.rest_api_keys; // TODO: why rest api keys??

	// fill part of the action metadata from config
	data[config.player_guid] = player_guid;
	data[config.match_guid] = match_guid;
	data[config.action_type] = "A"; // TODO: Action types should be in the config!!
	data[config.action_time_epoch] = std::to_string(action_time_epoch);
	data[config.weapon_guid] = weapon_guid;

	return data;
};

/**
* get_action_stream:
*
* Transforms action metadata from map to string, so this data can be used when sending packets 
* to the server
**/
std::string AttackActionData::get_action_stream()
{	
	std::map<std::string, std::string> data = get_data();
	const ActionStreamFileConfig& config = sdk_config.rest_api_keys; // TODO: why rest api keys?
	std::string output_string;

	// Transform action into string in format 
	// <Timestamp>, ATTACK, <playerID>, <WeaponID>
	output_string += data[config.action_time_epoch] + ",";
	output_string += data[config.action_type] + ",";
	output_string += data[config.player_guid] + ",";
	output_string += data[config.weapon_guid];

	return output_string;
};

/**
* clone:
*
**/
AttackActionData* AttackActionData::clone()
{
	return new AttackActionData(*this);
}

/**
* calculate_size:
*
* Calculate size of the action string
**/
void AttackActionData::calculate_size()
{
	// We have part of the action variables in base data class
	BaseActionData::calculate_size();

	size += weapon_guid.size();
}