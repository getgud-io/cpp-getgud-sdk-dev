#include "../../include/actions/DeathActionData.h"

#include "../../include/config/Config.h"

using namespace GetGudSdk;

extern Config sdk_config;

/**
* DeathActionData:
* @_data_info: Instance of the base data class which contains all the
* required fields
*
* Constructor of  Death Action, this action is later
* passed to the action buffer
**/
DeathActionData::DeathActionData(BaseData _data_info) 
	: BaseActionData(_data_info)
{
	// calculate size of the action string so it can be used later for total size assesment
	calculate_size();
};

/**
* DeathActionData:
* @_player_guid: guid of the player who died
* @_match_guid: guid of the match where player died
* @_action_time_epoch: timestamp of the action in the Epoch time in
* milliseconds. Example: 1643717696000
*
* Constructor of  Death Action, this action is later
* passed to the action buffer
**/
DeathActionData::DeathActionData(std::string _player_guid, std::string _match_guid,
	long _action_time_epoch) : 
	BaseActionData({ Actions::Death, _action_time_epoch, _player_guid, _match_guid})
{
	// calculate size of the action string so it can be used later for total size assesment
	calculate_size();
};

/**
* DeathActionData:
* @copy: Death action to copy
*
* Constructor of  Death Action, this action is later
* passed to the action buffer
**/
DeathActionData::DeathActionData(DeathActionData& copy)
	: BaseActionData(copy)
{

}

DeathActionData::~DeathActionData() 
{
	
};


/**
* get_data:
*
* Fills action metadata struct from config and from fields set in action constructor
**/
std::map<std::string, std::string> DeathActionData::get_data()
{
	std::map<std::string, std::string> data;
  	const ActionStreamFileConfig& config = sdk_config.rest_api_keys; // TODO: why rest_api_keys

	// fill part of the action metadata from config
	data[config.player_guid] = player_guid;
	data[config.match_guid] = match_guid;
	data[config.action_type] = std::to_string((int)action_type);
	data[config.action_time_epoch] = std::to_string(action_time_epoch);

	return data;
};

/**
* get_action_stream:
*
* Transforms action metadata from map to string, so this data can be used when sending packets 
* to the server
**/
std::string DeathActionData::get_action_stream()
{
	std::map<std::string, std::string> data = get_data();
	const ActionStreamFileConfig& config = sdk_config.rest_api_keys; // TODO: why rest api keys?
	std::string output_string;

	// Transform action into string in format
	//<Timestamp>, DEATH, <playerID> 

	output_string += data[config.action_time_epoch] + ",";
	output_string += data[config.action_type] + ",";
	output_string += data[config.player_guid];

	return output_string;
};

/**
* clone:
*
**/
DeathActionData* DeathActionData::clone()
{
	return new DeathActionData(*this);
}

/**
* calculate_size:
*
* Calculate size of the action string
**/
void DeathActionData::calculate_size()
{
	// We have part of the action variables in base data class
	BaseActionData::calculate_size(); 
}