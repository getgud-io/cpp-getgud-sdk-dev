#include "GameSender.h"

namespace GetGudSdk
{
	//create a new GameSender thread by default
	GameSender game_updater;

	extern ActionsBuffer actions_buffer;
	extern GameContainer game_container;

	GameSender::GameSender()
	{
		updater_thread = std::thread([&]() { update_loop(); });//implementation of loop required
	}

	GameSender::~GameSender()
	{
		stop_required = true;
		updater_thread.join();
	}

	void GameSender::check_actions()
	{
		std::deque<BaseActionData*> load_buffer;

		if (actions_buffer.get_actions(&load_buffer) == false) //if actions buffer is empty
		{
			return; //end function
		}

		std::unordered_map<std::string, std::deque<BaseActionData*>>
			splitted_actions;

		split_actions_by_match(load_buffer, splitted_actions); //sort by match and timestamp
		append_splitted_actions(splitted_actions); //push actions to game's matches

		//TODO:
		//verify limits
			//get and check limit
				//if over limit copy game matches and split actions buffer into new copy
			//take old GameData
			//push new GameData with splitted actions
			//send old GameData
	}

	void GameSender::update_loop()
	{
		while (!stop_required)
		{
			check_actions();
			//check end game
			//check size and move upper
			//check packets timer
			//check messages
			std::this_thread::sleep_for(std::chrono::milliseconds(100)); //sleep delay by config
		}
	}

	void GameSender::append_splitted_actions(std::unordered_map<std::string, std::deque<BaseActionData*>>& actions_in)
	{
		for (auto& sorted_actions : actions_in)
		{
			game_container.add_actions(sorted_actions.first, sorted_actions.second); //add by match_guid and data buffer pair
		}
	}

	void GameSender::split_actions_by_match(std::deque<BaseActionData*>& actions_in,
		std::unordered_map<std::string, std::deque<BaseActionData*>>& actions_out) //match_id and actions buffer pair
	{

		for (auto& action : actions_in)
		{
			if (action == nullptr) // check if action's pointer exist
				continue; //skip if doesnt exist

			auto find_it = actions_out.find(action->match_guid); //find the match guid
			if (find_it == actions_out.end()) //if match not found
			{
				//make new sorted buffer element with match_guid and deque buffer pair
				actions_out.emplace(std::make_pair(action->match_guid, std::deque<GetGudSdk::BaseActionData*>()));
			}

			auto& match_actions_buffer = actions_out[action->match_guid]; //get match actions buffer

			actions_out[action->match_guid].push_back(action); //add the action to sorted buffer
		}
	}
}