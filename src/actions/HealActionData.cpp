#include "..\..\include\actions\HealActionData.h"

using namespace GetGudSdk;

HealActionData::HealActionData(BaseData _data_info, float _health_gained)
	: BaseActionData(_data_info) //fill the base data
{
	//fill the action specified data
	health_gained = _health_gained;
};

HealActionData::HealActionData(std::string _player_guid, std::string _match_guid,
	long _action_time_epoch, float _health_gained) :
	BaseActionData({ Actions::Heal, _action_time_epoch, _player_guid, _match_guid }) //fill the base data
{
	//fill the acton secified data
	health_gained = _health_gained;
};

HealActionData::~HealActionData()
{

};

std::map<std::string, std::string> HealActionData::get_data()
{
	//make a new buffer with <key, value> pairs
	std::map<std::string, std::string> data;

	//fill the buffer by key, value
	data["player_guid"] = player_guid;
	//data["game_guid"] = game_guid; //not required
	data["match_guid"] = match_guid;
	data["action_type"] = std::to_string((int)action_type);
	data["action_time_epoch"] = std::to_string(action_time_epoch);
	data["health_gained"] = std::to_string(health_gained);

	return data;
};

std::string HealActionData::get_action_stream()
{
	std::map<std::string, std::string> data = get_data();
	std::string output_string;

	//TODO: create string here

	return output_string;
};