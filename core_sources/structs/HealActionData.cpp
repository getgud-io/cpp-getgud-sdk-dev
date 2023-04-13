#include "..\..\Headers\HealActionData.h"

using namespace GetGudSdk;

HealActionData::HealActionData(BaseData _data_info, float _health_gained) : BaseActionData(_data_info)
{
	health_gained = _health_gained;
};

HealActionData::HealActionData(std::string _player_guid, std::string _game_guid, std::string _match_guid,
	Actions _action_type, long _action_time_epoch, float _health_gained) :
	BaseActionData({ _action_type, _action_time_epoch, _player_guid, _game_guid, _match_guid })
{
	health_gained = _health_gained;
};

HealActionData::~HealActionData()
{

};

std::map<std::string, std::string> HealActionData::get_data()
{
	std::map<std::string, std::string> data;

	data["player_guid"] = player_guid;
	data["game_guid"] = game_guid;
	data["match_guid"] = match_guid;
	data["action_type"] = std::to_string((int)action_type);
	data["action_time_epoch"] = std::to_string(action_time_epoch);
	data["health_gained"] = std::to_string(health_gained);

	return data;
};