#include "MatchData.h"

#include <algorithm>

namespace GetGudSdk
{
	MatchData::MatchData(std::string _match_guid, std::string _match_mode, std::string _map_name)
		: match_guid(_match_guid), match_mode(_match_mode), map_name(_map_name)
	{

	}

	MatchData::~MatchData()
	{
		//delete every action's pointer in the buffer
		for (auto it = match_actions.begin(); it != match_actions.end(); ++it)
		{
			delete* it;
		}

		match_actions.clear();
	}

	std::string& MatchData::get_match_guid()
	{
		return match_guid;
	}

	std::string& MatchData::get_map_name()
	{
		return map_name;
	}

	std::string& MatchData::get_match_mode()
	{
		return match_mode;
	}

	bool MatchData::push_actions(std::deque<BaseActionData*>& _actions)
	{
		match_actions.insert(match_actions.end(), _actions.begin(), _actions.end());
		return true;
	}

	bool MatchData::push_sorted_actions(std::deque<BaseActionData*>& _actions)
	{
		auto actions_it = match_actions.end();

		bool actions_pushed = true;

		do
		{
			actions_it--;//move from last element to begin
			long time_epoch_action = (*actions_it)->action_time_epoch;
			long lowest_time_epoch_actions = (*_actions.begin())->action_time_epoch;
			if (time_epoch_action <= lowest_time_epoch_actions) //if found action that 
				//early than new action
			{
				actions_pushed = true;
				actions_it++; //move at next and insert actions after earlier action
				match_actions.insert(actions_it, _actions.begin(), _actions.end());
				break; //end loop
			}


		} while (actions_it != match_actions.begin()); //loop from end to top of array

		if (!actions_pushed) //check if actions didnt push
		{
			match_actions.insert(match_actions.begin(), _actions.begin(), _actions.end()); //push actions to front
		}

		return true;
	}

	std::deque<BaseActionData*>& MatchData::get_actions()
	{
		return match_actions;
	}

	void MatchData::sort_actions()
	{
		std::sort(match_actions.begin(), match_actions.end());
	}

	void MatchData::get_sorted_actions(std::deque<BaseActionData*>& actions_out)
	{
		for (auto action : match_actions) //parse every action
		{
			auto match_action_it = actions_out.end(); // get end of actions

			bool action_pushed = false;
			do
			{
				match_action_it--;//move from last element to begin
				long time_data = (*match_action_it)->action_time_epoch;
				if (time_data <= action->action_time_epoch) //if found action that 
					//early than new action
				{
					action_pushed = true;
					match_action_it++; //move at next and insert action after earlier action
					actions_out.insert(match_action_it, action);
					break; //end loop
				}

			} while (match_action_it != actions_out.begin()); //loop from end to top of array

			if (!action_pushed) //check if action didnt push
			{
				actions_out.push_front(action); //push action to front
			}
		}
	}
}