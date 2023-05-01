#include "../../include/actions/HealActionData.h"

#include "../../include/config/Config.h"

using namespace GetGudSdk;

extern Config sdk_config;

/**
* HealActionData:
* @_data_info: Instance of the base data class which contains all the
* required fields
* @_health_gained: amount of health gained during healing action
*
* Constructor of  Heal Action, this action is later
* passed to the action buffer
**/
HealActionData::HealActionData(BaseData _data_info, float _health_gained)
	: BaseActionData(_data_info) 
{
	health_gained = _health_gained;

	// calculate size of the action string so it can be used later for total size assesment
	calculate_size();
};

/**
* HealActionData:
* @_player_guid: guid of the player who performed heal action
* @_match_guid: guid of the match where the action took place
* @_action_time_epoch: timestamp of the action in the Epoch time in
* milliseconds. Example: 1643717696000
* @_health_gained: amount of health gained during healing action
*
* Constructor of  Heal Action, this action is later
* passed to the action buffer
**/
HealActionData::HealActionData(std::string _player_guid, std::string _match_guid,
	long _action_time_epoch, float _health_gained) :
	BaseActionData({ Actions::Heal, _action_time_epoch, _player_guid, _match_guid })
{
	health_gained = _health_gained;

	// calculate size of the action string so it can be used later for total size assesment
	calculate_size();
};

/**
* HealActionData:
* @copy: Heal action to copy
*
* Constructor of  Heal Action, this action is later
* passed to the action buffer
**/
HealActionData::HealActionData(HealActionData& copy)
	: BaseActionData(copy)
{
	health_gained = copy.health_gained;
}

HealActionData::~HealActionData()
{

};

/**
* get_data:
*
* Fills action metadata struct from config and from fields set in action constructor
**/
std::map<std::string, std::string> HealActionData::get_data()
{
	std::map<std::string, std::string> data;
	const ActionStreamFileConfig& config = sdk_config.rest_api_keys; // TODO: why rest api keys??

	// fill part of the action metadata from config
	data[config.player_guid] = player_guid;
	data[config.match_guid] = match_guid;
	data[config.action_type] = std::to_string((int)action_type);
	data[config.action_time_epoch] = std::to_string(action_time_epoch);
	data[config.health_gained] = std::to_string(health_gained);

	return data;
};

/**
* get_action_stream:
*
* Transforms action metadata from map to string, so this data can be used when sending packets 
* to the server
**/
std::string HealActionData::get_action_stream()
{
	std::map<std::string, std::string> data = get_data();
	const ActionStreamFileConfig& config = sdk_config.rest_api_keys; // TODO: why rest api keys?
	std::string output_string;

	// Transform action into string in format
	//<Timestamp>, HEAL, <playerID>, <Health gained>
	output_string += data[config.action_time_epoch] + ",";
	output_string += data[config.action_type] + ",";
	output_string += data[config.player_guid] + ",";
	output_string += data[config.health_gained];

	return output_string;
};

/**
* clone:
*
**/
HealActionData* HealActionData::clone()
{
	return new HealActionData(*this);
}

/**
* calculate_size:
*
* Calculate size of the action string
**/
void HealActionData::calculate_size()
{
	// We have part of the action variables in base data class
	BaseActionData::calculate_size();

	size += sizeof(health_gained);
}