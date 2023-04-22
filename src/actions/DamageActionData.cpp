#include "..\..\include\actions\DamageActionData.h"

using namespace GetGudSdk;

DamageActionData::DamageActionData(BaseData _data_info, float _damage_done, 
	std::string _victim_player_guid, std::string _weapon_guid) 
	: BaseActionData(_data_info) //fill the base data
{
	//fill the action specified data
	damage_done = _damage_done;
	weapon_guid = _weapon_guid;
	victim_player_guid = _victim_player_guid;
};

DamageActionData::DamageActionData(std::string _player_guid, std::string _match_guid,
	long _action_time_epoch, float _damage_done,
	std::string _victim_player_guid, std::string _weapon_guid) :
	BaseActionData({ Actions::Damage, _action_time_epoch, _player_guid, _match_guid }) //fill the base data
{
	//fill the action specified data
	damage_done = _damage_done;
	weapon_guid = _weapon_guid;
	victim_player_guid = _victim_player_guid;
};

DamageActionData::~DamageActionData()
{

};

std::map<std::string, std::string> DamageActionData::get_data()
{
	//make a new buffer with <key, value> pairs
	std::map<std::string, std::string> data;

	//fill the buffer by key, value
	data["player_guid"] = player_guid;
	//data["game_guid"] = game_guid; //not required
	data["match_guid"] = match_guid;
	data["action_type"] = std::to_string((int)action_type);
	data["action_time_epoch"] = std::to_string(action_time_epoch);

	data["damage_done"] = std::to_string(damage_done);
	data["weapon_guid"] = weapon_guid;
	data["victim_player_guid"] = victim_player_guid;

	return data;
};

std::string DamageActionData::get_action_stream()
{
	std::map<std::string, std::string> data = get_data();
	std::string output_string;

	//TODO: create string here

	return output_string;
};