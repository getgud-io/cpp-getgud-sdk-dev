#include "..\..\Headers\DeathActionData.h"

using namespace GetGudSdk;

//empty initializations, inheritance only
DeathActionData::DeathActionData(BaseData _data_info) : BaseActionData(_data_info) 
{
	
};

DeathActionData::DeathActionData(std::string _player_guid, std::string _game_guid, std::string _match_guid,
	Actions _action_type, long _action_time_epoch) : 
	BaseActionData({ _action_type, _action_time_epoch, _player_guid, _game_guid, _match_guid}) 
{

};

DeathActionData::~DeathActionData() 
{
	
};


std::map<std::string, std::string> DeathActionData::get_data()
{
	std::map<std::string, std::string> data;

	data["player_guid"] = player_guid;
	data["game_guid"] = game_guid;
	data["match_guid"] = match_guid;
	data["action_type"] = std::to_string((int)action_type);
	data["action_time_epoch"] = std::to_string(action_time_epoch);

	return data;
};
