#include "MatchData.h"

#include <algorithm>

namespace GetGudSdk
{	
	/**
	* MatchData:
	* @_match_guid: guid of the match
	* @_match_mode: mode of the match
	* @_map_name: map name of the match
	*  
	* Match data is part of Game Packet that will sent to GetGud. 
	* Every match has metadata, actions, chat and reports associated with it
	**/
	MatchData::MatchData(std::string _match_guid, std::string _match_mode, std::string _map_name)
		: match_guid(_match_guid), match_mode(_match_mode), map_name(_map_name)
	{
		// calculate size of the match metadata so it can be used later for total size assesment
		calculate_size();
	}

	MatchData::~MatchData()
	{

	}

	/**
	* calculate_size:
	*
	* Calculate size of match metadata
	**/
	void MatchData::calculate_size()
	{
		size_in_bytes += match_guid.size();
		size_in_bytes += match_mode.size();
		size_in_bytes += map_name.size();
		size_in_bytes += sizeof(actions_size);
		size_in_bytes += sizeof(size_in_bytes);
	}

	/**
	* dispose_actions:
	*
	* Delete all the actions which belong to this match in the game packet
	**/
	void MatchData::dispose_actions()
	{
		for (auto it = match_actions.begin(); it != match_actions.end(); ++it)
		{
			delete* it;
		}
		
		match_actions.clear();
	}

	/**
	* get_match_guid:
	*
	**/
	std::string& MatchData::get_match_guid()
	{
		return match_guid;
	}

	/**
	* get_map_name:
	*
	**/
	std::string& MatchData::get_map_name()
	{
		return map_name;
	}

	/**
	* get_match_mode:
	*
	**/
	std::string& MatchData::get_match_mode()
	{
		return match_mode;
	}

	/**
	* get_match_size:
	*
	**/
	unsigned int MatchData::get_match_size()
	{
		return size_in_bytes;
	}

	/**
	* push_actions:
	* @_actions: deque of actions that need to be added to the match
	*
	* Add actions to the match metadata of game packet
	**/
	unsigned int MatchData::push_actions(std::deque<BaseActionData*>& _actions)
	{
		unsigned int actions_increase_size = 0;

		if (match_actions.empty())
			actions_increase_size = size_in_bytes;

		// calculate total increase size for the actions we are pushing
		for (auto& action : _actions)
		{
			actions_increase_size = actions_increase_size + action->get_data_size();
		}

		size_in_bytes = size_in_bytes + actions_increase_size;

		match_actions.insert(match_actions.end(), _actions.begin(), _actions.end());
		return actions_increase_size;
	}

	/**
	* get_actions:
	*
	**/
	std::deque<BaseActionData*>& MatchData::get_actions()
	{
		return match_actions;
	}


	/**
	* sort_actions:
	*
	* Sort all alctions of this match w.r.t action timestamp
	**/
	void MatchData::sort_actions()
	{
		std::sort(match_actions.begin(), match_actions.end());
	}

	/**
	* get_sorted_actions:
	* @actions_out: 
	*
	* Sort all alctions of this match w.r.t action timestamp
	* TODO: describe difference between sort_actions and get_sorted_actions
	**/
	void MatchData::get_sorted_actions(std::deque<BaseActionData*>& actions_out)
	{
		for (auto action : match_actions) 
		{
			auto match_action_it = actions_out.end(); 

			bool action_pushed = false;
			do
			{
				//move from last element 
				match_action_it--;
				long time_data = (*match_action_it)->action_time_epoch;
				
				// if found action that was earlier than new action
				if (time_data <= action->action_time_epoch)
				{
					action_pushed = true;
					match_action_it++; 
					actions_out.insert(match_action_it, action);
					break; 
				}

			} while (match_action_it != actions_out.begin()); //loop from end to top of array

			if (!action_pushed)
			{
				actions_out.push_front(action); 
			}
		}
	}
}