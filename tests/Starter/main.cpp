#include <iostream>
#include <GetGudSdk.h>

#include <chrono>
#include <thread>

int main()
{
    {
        GetGudSdk::init();

        //actions test

        //*Debug only* get buffer of the actions
        auto actions_buffer = GetGudSdk::Debug::get_actions_buffer();

        //packets tests

        std::string guid = GetGudSdk::start_game(11, "test_server", "game mode");

        std::string match1 = GetGudSdk::start_match(guid, "match mode", "map name");

        //make different actions
        GetGudSdk::DeathActionData data(GetGudSdk::BaseData{ GetGudSdk::Actions::Attack, 12, "player0", match1 });
        GetGudSdk::AttackActionData data2(GetGudSdk::BaseData{ GetGudSdk::Actions::Heal, 11, "player5", match1 }, "weapon1");
        GetGudSdk::DamageActionData data3(GetGudSdk::BaseData{ GetGudSdk::Actions::Damage, 15, "player4", match1 }, 10, std::string(), std::string());

        //push actions one by one
        GetGudSdk::send_action((GetGudSdk::BaseActionData*)&data);
        GetGudSdk::send_action((GetGudSdk::BaseActionData*)&data2);
        GetGudSdk::send_action((GetGudSdk::BaseActionData*)&data3);
        
        //push array
        GetGudSdk::send_actions(std::vector<GetGudSdk::BaseActionData*>{
            (GetGudSdk::BaseActionData*)&data, (GetGudSdk::BaseActionData*)&data2, (GetGudSdk::BaseActionData*)&data3});

        GetGudSdk::send_attack_action(match1, "ddasf", "dsafae", 999999999);
        GetGudSdk::send_attack_action(match1, "ddasf", "dsafae", 999999999);
        GetGudSdk::send_attack_action(match1, "ddasf", "dsafae", 999999999);

        //std::string match2 = GetGudSdk::start_match(guid, "match2 mode", "map2 name");

        //auto& buffer_data = GetGudSdk::Debug::get_packets_buffer();

        //std::string match3 = GetGudSdk::start_match(guid, "match2 mode", "map2 name");

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        bool result = GetGudSdk::set_end_game(guid);
    }
    while (true) {};

    //auto* matches_buffer_data = GetGudSdk::Debug::get_matches_buffer(guid);

    return 0;
}