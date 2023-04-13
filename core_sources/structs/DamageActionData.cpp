#include "..\..\Headers\DamageActionData.h"

using namespace GetGudSdk;

DamageActionData::DamageActionData(BaseData _data_info, float _damage_done, 
	std::string _victim_player_guid, std::string _weapon_guid) : BaseActionData(_data_info)
{
	damage_done = _damage_done;
	weapon_guid = _weapon_guid;
	victim_player_guid = _victim_player_guid;
};

DamageActionData::DamageActionData(std::string _player_guid, std::string _game_guid, std::string _match_guid,
	Actions _action_type, long _action_time_epoch, float _damage_done,
	std::string _victim_player_guid, std::string _weapon_guid) :
	BaseActionData({ _action_type, _action_time_epoch, _player_guid, _game_guid, _match_guid })
{
	damage_done = _damage_done;
	weapon_guid = _weapon_guid;
	victim_player_guid = _victim_player_guid;
};

DamageActionData::~DamageActionData()
{

};

std::map<std::string, std::string> DamageActionData::get_data()
{
	std::map<std::string, std::string> data;

	data["player_guid"] = player_guid;
	data["game_guid"] = game_guid;
	data["match_guid"] = match_guid;
	data["action_type"] = std::to_string((int)action_type);
	data["action_time_epoch"] = std::to_string(action_time_epoch);

	data["damage_done"] = std::to_string(damage_done);
	data["weapon_guid"] = weapon_guid;
	data["victim_player_guid"] = victim_player_guid;

	return data;
};
