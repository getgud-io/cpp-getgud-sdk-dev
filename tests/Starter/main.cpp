#include <iostream>

#include <GetGudSdk.h>

int main()
{
    //actions test
    //make different actions
    GetGudSdk::DeathActionData data(GetGudSdk::BaseData{});
    GetGudSdk::AttackActionData data2(GetGudSdk::BaseData{}, std::string());
    GetGudSdk::DamageActionData data3(GetGudSdk::BaseData{}, 10, std::string(), std::string());
    GetGudSdk::HealActionData data4(GetGudSdk::BaseData{}, 10);

    GetGudSdk::PositionActionData data5(GetGudSdk::BaseData{}, GetGudSdk::PositionF(), GetGudSdk::RotationF());

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

    auto& buffer_data = GetGudSdk::Debug::get_packets_buffer();

    std::string match3 = GetGudSdk::start_match(guid, "match2 mode", "map2 name");

    bool result = GetGudSdk::end_game(guid);

    return 0;
}