#include <iostream>

#include <GetGudSdk.h>

#include <unordered_map>

void sort_actions(std::deque<GetGudSdk::BaseActionData*>& actions_in,
	std::unordered_map<std::string, std::deque<GetGudSdk::BaseActionData*>>& actions_out) //match_id and actions buffer pair
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

		if (match_actions_buffer.size() == 0)
		{
			actions_out[action->match_guid].push_back(action); //add the action to sorted buffer
		}
		else
		{
			auto match_action_it = match_actions_buffer.end(); // get end of actions

			bool action_pushed = false;
			do 
			{
				match_action_it--;//move from last element to begin
				long time_epoch_match_action = (*match_action_it)->action_time_epoch;
				if (time_epoch_match_action <= action->action_time_epoch) //if found action that 
					//early than new action
				{
					action_pushed = true;
					match_action_it++; //move at next and insert action after
					match_actions_buffer.insert(match_action_it, action);
					break; //end loop
				}

			} while (match_action_it != match_actions_buffer.begin()); //loop from end to top of array

			if (!action_pushed) //check if action didnt push
			{
				match_actions_buffer.push_front(action); //push action to front
			}
		}
	}
}

int main()
{
    //actions test
    //make different actions
    GetGudSdk::DeathActionData data(GetGudSdk::BaseData{ GetGudSdk::Attack, 12, "player0", "match1" });
    GetGudSdk::AttackActionData data2(GetGudSdk::BaseData{ GetGudSdk::Heal, 11, "player5", "match1" }, std::string());
    GetGudSdk::DamageActionData data3(GetGudSdk::BaseData{ GetGudSdk::Damage, 15, "player4", "match1" }, 10, std::string(), std::string());
    GetGudSdk::HealActionData data4(GetGudSdk::BaseData{ GetGudSdk::Damage, 11, "player2", "match1" }, 10);

    GetGudSdk::PositionActionData data5(GetGudSdk::BaseData{ GetGudSdk::Death, 10, "player3", "match1" }, GetGudSdk::PositionF(), GetGudSdk::RotationF());

	GetGudSdk::DeathActionData _data(GetGudSdk::BaseData{ GetGudSdk::Attack, 12, "player4", "match1"});
	GetGudSdk::AttackActionData _data2(GetGudSdk::BaseData{ GetGudSdk::Heal, 11, "player2", "match1" }, std::string());
	GetGudSdk::DamageActionData _data3(GetGudSdk::BaseData{ GetGudSdk::Damage, 15, "player5", "match1" }, 10, std::string(), std::string());
	GetGudSdk::HealActionData _data4(GetGudSdk::BaseData{ GetGudSdk::Damage, 11, "player1", "match1" }, 10);

	GetGudSdk::PositionActionData _data5(GetGudSdk::BaseData{ GetGudSdk::Death, 10, "player1", "match1" }, GetGudSdk::PositionF(), GetGudSdk::RotationF());

	std::deque<GetGudSdk::BaseActionData*> actions_in{ &data, &data2, &data3, &data4, &data5,
														&_data,&_data2,&_data3,&_data4,&_data5 };

	std::unordered_map<std::string, std::deque<GetGudSdk::BaseActionData*>> sorted_list;

	sort_actions(actions_in, sorted_list);

    //push actions one by one
    GetGudSdk::send_action((GetGudSdk::BaseActionData*)&data);
    GetGudSdk::send_action((GetGudSdk::BaseActionData*)&data2);
    GetGudSdk::send_action((GetGudSdk::BaseActionData*)&data3);
    GetGudSdk::send_action((GetGudSdk::BaseActionData*)&data4);
    GetGudSdk::send_action((GetGudSdk::BaseActionData*)&data5);

    //*Debug only* get buffer of the actions
    auto actions_buffer = GetGudSdk::Debug::get_actions_buffer();

    //packets tests

    std::string guid = GetGudSdk::start_game(11, "test_server", "game mode");

    std::string match1 = GetGudSdk::start_match(guid, "match mode", "map name");

    std::string match2 = GetGudSdk::start_match(guid, "match2 mode", "map2 name");

    //auto& buffer_data = GetGudSdk::Debug::get_packets_buffer();

    std::string match3 = GetGudSdk::start_match(guid, "match2 mode", "map2 name");

    auto* matches_buffer_data = GetGudSdk::Debug::get_matches_buffer(guid);

    bool result = GetGudSdk::set_end_game(guid);

    return 0;
}