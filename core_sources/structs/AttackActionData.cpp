#include "..\..\Headers\AttackActionData.h"

using namespace GetGudSdk;

AttackActionData::AttackActionData(BaseData _data_info, std::string _weapon_guid) : BaseActionData(_data_info)
{
	weapon_guid = _weapon_guid;
};

AttackActionData::AttackActionData(std::string _player_guid, std::string _game_guid, std::string _match_guid,
	Actions _action_type, long _action_time_epoch, std::string _weapon_guid) :
	BaseActionData({ _action_type, _action_time_epoch, _player_guid, _game_guid, _match_guid })
{
	weapon_guid = _weapon_guid;
};

AttackActionData::~AttackActionData()
{

};


std::map<std::string, std::string> AttackActionData::get_data()
{
	std::map<std::string, std::string> data;

	data["player_guid"] = player_guid;
	data["game_guid"] = game_guid;
	data["match_guid"] = match_guid;
	data["action_type"] = std::to_string((int)action_type);
	data["action_time_epoch"] = std::to_string(action_time_epoch);
	data["weapon_guid"] = weapon_guid;

	return data;
};
