#include "..\..\include\actions\AttackActionData.h"

using namespace GetGudSdk;

AttackActionData::AttackActionData(BaseData _data_info, std::string _weapon_guid) 
	: BaseActionData(_data_info) //fill the base data
{
	//fill the action specified data
	weapon_guid = _weapon_guid;
};

AttackActionData::AttackActionData(std::string _player_guid, std::string _match_guid,
	long _action_time_epoch, std::string _weapon_guid) :
	BaseActionData({ Actions::Attack, _action_time_epoch, _player_guid, _match_guid }) //fill the base data
{
	//fill the action specified data
	weapon_guid = _weapon_guid;
};

AttackActionData::~AttackActionData()
{

};

std::map<std::string, std::string> AttackActionData::get_data()
{
	//make a new buffer with <key, value> pairs
	std::map<std::string, std::string> data;

	//fill the buffer by key, value
	data["player_guid"] = player_guid;
	//data["game_guid"] = game_guid; //not required
	data["match_guid"] = match_guid;
	data["action_type"] = "A";
	data["action_time_epoch"] = std::to_string(action_time_epoch);
	data["weapon_guid"] = weapon_guid;

	return data;
};

std::string AttackActionData::get_action_stream()
{
	std::map<std::string, std::string> data = get_data();
	std::string output_string;

	//Attack: 15,A,Storm
	//timestamp, action name, player id
	
	//actions have no player id, and requests have no player_guid, weapon_guid
	//example here:
	//Attack: 3124rwwa-4253-rwe3-w354twt4tey5,8415236435,A,88834153-rew5-435w-352543556436
	//player_guid, action_time_epoch, action_type, weapon_guid

	output_string += data["player_guid"] + ","; //get data by key "player_guid",
												//guid will be writed to output_string
												//append ',' symbol after guid
	output_string += data["action_time_epoch"] + ",";
	output_string += data["action_type"] + ",";
	output_string += data["weapon_guid"];

	return output_string;
};