#include "..\..\include\actions\BaseActionData.h"

using namespace GetGudSdk;

//the helpers, hidden from other files
int total_amount = 0;
int id_counter = 0; 

BaseActionData::BaseActionData(BaseData data) //filling the base data
{
	total_amount = total_amount + 1; //increase total amount counter of created actions
	id_counter = id_counter + 1;
	id = id_counter; //increase id value by one and then assign it to the action's id
	//size is ignored currently
	action_type = data.action_type;
	action_time_epoch = data.action_time_epoch;
	player_guid = data.player_guid;
	match_guid = data.match_guid;
};

BaseActionData::~BaseActionData() 
{
	total_amount = total_amount - 1; //total amount counter decreasing when action deletes
};

bool BaseActionData::operator()(BaseActionData& a, BaseActionData& b)
{
	return a.action_time_epoch < b.action_time_epoch;
}

std::map<std::string, std::string> BaseActionData::get_data() 
{
	return std::map<std::string, std::string>(); //empty filled buffer, inheritance syntax
};

std::string BaseActionData::get_action_stream()
{
	return std::string();
};