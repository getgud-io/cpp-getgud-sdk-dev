#include "../../include/actions/BaseActionData.h"

using namespace GetGudSdk;

// used to count total actions in the system, TODO: rename total_action_amount
int total_amount = 0;
// used to assign incremental ids to every new action TODO: rename incremental_action_id
int id_counter = 0;

/**
* BaseActionData:
* @data: BaseData, structure contains all core metadata for each action
*
* Constructor of Base Action, all other actions inherit from BaseAction
**/
BaseActionData::BaseActionData(BaseData data)
{
	// increase the total amount of actions we have in the system
	total_amount = total_amount + 1;
	id_counter = id_counter + 1;
	id = id_counter;

	action_type = data.action_type;
	action_time_epoch = data.action_time_epoch;
	player_guid = data.player_guid;
	match_guid = data.match_guid;
};

/**
* BaseActionData:
* @copy: BaseData instance to copy from
*
* Constructor of Base Action, all other actions inherit from BaseAction
**/
BaseActionData::BaseActionData(BaseActionData& copy)
{
	// increase the total amount of actions we have in the system
	total_amount = total_amount + 1;
	id = copy.id; 
	size = copy.size;
	
	action_type = copy.action_type;
	action_time_epoch = copy.action_time_epoch;
	player_guid = copy.player_guid;
	match_guid = copy.match_guid;
}

/**
* ~BaseActionData:
*
* Destrctor, called when we delete actions from the system
**/
BaseActionData::~BaseActionData()
{
	// reduce amount of actions in the sdk 
	total_amount = total_amount - 1; 
};

/**
* operator:
* @a: left action
* @b: right action
*
* Compares actions to each other wrt timestamp, 
* uses by std::sort, where sorting based on action_time_epoch
**/
bool BaseActionData::operator()(BaseActionData& a, BaseActionData& b)
{
	return a.action_time_epoch < b.action_time_epoch;
}

/**
* get_data:
*
* Mainly from inheritance from other action classes, BaseActionData doesn't need this
**/
std::map<std::string, std::string> BaseActionData::get_data()
{
	return std::map<std::string, std::string>(); 
};

/**
* get_action_stream:
*
* Mainly from inheritance from other action classes, BaseActionData doesn't need this
**/
std::string BaseActionData::get_action_stream()
{
	return std::string();
};

/**
* clone:
*
* Clone action
**/
BaseActionData* BaseActionData::clone()
{
	return this;
}

/**
* calculate_size:
*
* Virtual function that will be replaced by an action,
* Update size of an action, calls from the cunstructors
**/
void BaseActionData::calculate_size()
{
	size = 0;
	size += sizeof(id);
	size += sizeof(size);
	size += sizeof(action_type);
	size += sizeof(action_time_epoch);
	size += player_guid.size();
	size += match_guid.size();
}

/**
* get_data_size:
*
* returns size in bytes of an action
**/
unsigned int BaseActionData::get_data_size()
{
	return size;
}