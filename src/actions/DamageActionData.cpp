#include "../../include/actions/DamageActionData.h"

#include "../../include/config/Config.h"

using namespace GetGudSdk;

extern Config sdk_config;

/**
* DamageActionData:
* @_data_info: Instance of the base data class which contains all the
* required fields
* @_damage_done: the amount of damage done from attacker to victim
* @_victim_player_guid: guid of the player who was damaged by
* _attacker_player_guid
* @_weapon_guid: guid of weapon that performed damage from attacker to victim
*
* Constructor of  Damage Action, this action is later
* passed to the action buffer
**/
DamageActionData::DamageActionData(BaseData _data_info, float _damage_done, 
	std::string _victim_player_guid, std::string _weapon_guid) 
	: BaseActionData(_data_info) //fill the base data
{
	damage_done = _damage_done;
	weapon_guid = _weapon_guid;
	victim_player_guid = _victim_player_guid;

	// calculate size of the action string so it can be used later for total size assesment
	calculate_size();
};

/**
* DamageActionData:
* @_player_guid: guid of the player who damaged the _victim_player_guid
* player if the attacker is an environment _attacker_player_guid can be null.
* TODO: rename to _attacker_player_guid
* @_match_guid: guid of the match where the action took place
* @_action_time_epoch: timestamp of the action in the Epoch time in
* milliseconds. Example: 1643717696000
* @_damage_done: the amount of damage done from attacker to victim
* @_victim_player_guid: guid of the player who was damaged by
* _attacker_player_guid
* @_weapon_guid: guid of weapon that performed damage from attacker to victim
*
* Constructor of  Damage Action, this action is later
* passed to the action buffer
**/
DamageActionData::DamageActionData(std::string _player_guid, std::string _match_guid,
	long _action_time_epoch, float _damage_done,
	std::string _victim_player_guid, std::string _weapon_guid) :
	BaseActionData({ Actions::Damage, _action_time_epoch, _player_guid, _match_guid }) //fill the base data
{
	damage_done = _damage_done;
	weapon_guid = _weapon_guid;
	victim_player_guid = _victim_player_guid;

	// calculate size of the action string so it can be used later for total size assesment
	calculate_size();
};

/**
* DamageActionData:
* @copy: Damage action to copy
*
* Constructor of  Damage Action, this action is later
* passed to the action buffer
**/
DamageActionData::DamageActionData(DamageActionData& copy)
	: BaseActionData(copy)
{
	damage_done = copy.damage_done;
	weapon_guid = copy.weapon_guid;
	victim_player_guid = copy.victim_player_guid;
}

DamageActionData::~DamageActionData()
{

};

/**
* get_data:
*
* Fills action metadata struct from config and from fields set in action constructor
**/
std::map<std::string, std::string> DamageActionData::get_data()
{
	std::map<std::string, std::string> data;
	const ActionStreamFileConfig& config = sdk_config.rest_api_keys; // TODO: Rename: sdk_config.rest_api_fields

	// fill part of the action metadata from config
	data[config.player_guid] = player_guid;
	data[config.match_guid] = match_guid;
	data[config.action_type] = std::to_string((int)action_type);
	data[config.action_time_epoch] = std::to_string(action_time_epoch);
	data[config.damage_done] = std::to_string(damage_done);
	data[config.weapon_guid] = weapon_guid;
	data[config.victim_player_guid] = victim_player_guid;

	return data;
};

/**
* get_action_stream:
*
* Transforms action metadata from map to string, so this data can be used when sending packets 
* to the server
**/
std::string DamageActionData::get_action_stream()
{
	std::map<std::string, std::string> data = get_data();
	const ActionStreamFileConfig& config = sdk_config.rest_api_keys;
	std::string output_string;

	// Transform action into string in format
	// <Timestamp>, DAMAGE, <Attacker playerID could be null>, <Victim playerID>, <Damage>, <WeaponID>  
	output_string += data[config.action_time_epoch] + ",";
	output_string += data[config.action_type] + ",";
	output_string += data[config.player_guid] + ",";
	output_string += data[config.victim_player_guid] + ",";
	output_string += data[config.damage_done] + ",";
	output_string += data[config.weapon_guid];

	return output_string;
};

/**
* clone:
*
**/
DamageActionData* DamageActionData::clone()
{
	return new DamageActionData(*this);
}

/**
* calculate_size:
*
* Calculate size of the action string
**/
void DamageActionData::calculate_size()
{
	BaseActionData::calculate_size(); //calculate base data first

	size += sizeof(damage_done);
	size += weapon_guid.size();
	size += victim_player_guid.size();
}