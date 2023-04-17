#include <iostream>

#include "../Headers/GetGudSdk.h"

int main()
{
    //make different actions
    GetGudSdk::DeathActionData data(GetGudSdk::BaseData{});
    GetGudSdk::AttackActionData data2(GetGudSdk::BaseData{}, std::string());
    GetGudSdk::DamageActionData data3(GetGudSdk::BaseData{}, 10, std::string(), std::string());
    GetGudSdk::HealActionData data4(GetGudSdk::BaseData{}, 10);

    //add pitch, roll to spawn and position action
    GetGudSdk::PositionActionData data5(GetGudSdk::BaseData{}, std::vector<float>());

    //push actions one by one
    GetGudSdk::send_action((GetGudSdk::BaseActionData*)&data);
    GetGudSdk::send_action((GetGudSdk::BaseActionData*)&data2);
    GetGudSdk::send_action((GetGudSdk::BaseActionData*)&data3);
    GetGudSdk::send_action((GetGudSdk::BaseActionData*)&data4);
    GetGudSdk::send_action((GetGudSdk::BaseActionData*)&data5);

    //*Debug only* get buffer of the actions
    auto buffer = GetGudSdk::Debug::get_buffer();

    return 0;
}