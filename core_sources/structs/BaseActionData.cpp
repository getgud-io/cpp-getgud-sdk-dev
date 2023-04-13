#include "..\..\Headers\BaseActionData.h"

using namespace GetGudSdk;

//hidden from other files
int total_amount = 0;
int id_counter = 0;

BaseActionData::BaseActionData(BaseData data) {
	total_amount++;
	//filling the data
	id = ++id_counter; //id starts from one
	//size is ignored currently
	action_type = data.action_type;
	action_time_epoch = data.action_time_epoch;
	player_guid = data.player_guid;
	game_guid = data.game_guid;
	match_guid = data.match_guid;
};
BaseActionData::~BaseActionData() {
	total_amount--;
};


std::map<std::string, std::string> BaseActionData::get_data() {
	return std::map<std::string, std::string>(); //empty filled buffer, inheritance syntax
};
