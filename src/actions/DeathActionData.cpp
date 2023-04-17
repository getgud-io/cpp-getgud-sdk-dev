#include "..\..\include\actions\DeathActionData.h"

using namespace GetGudSdk;

DeathActionData::DeathActionData(BaseData _data_info) 
	: BaseActionData(_data_info) //fill the base data 
{
	//the action class doesn't have specified data
};

DeathActionData::DeathActionData(std::string _player_guid, std::string _match_guid,
	long _action_time_epoch) : 
	BaseActionData({ Actions::Death, _action_time_epoch, _player_guid, _match_guid}) //fill the base data 
{
	//the action class doesn't have specified data
};

DeathActionData::~DeathActionData() 
{
	
};


std::map<std::string, std::string> DeathActionData::get_data()
{
	//make a new buffer with <key, value> pairs
	std::map<std::string, std::string> data;

	//fill the buffer by key, value
	data["player_guid"] = player_guid;
	data["game_guid"] = game_guid;
	data["match_guid"] = match_guid;
	data["action_type"] = std::to_string((int)action_type);
	data["action_time_epoch"] = std::to_string(action_time_epoch);

	return data;
};
