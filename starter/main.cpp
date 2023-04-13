#include <iostream>

#include "../Headers/GetGudSdk.h"

int main()
{
    GetGudSdk::DeathActionData data(GetGudSdk::BaseData{});
    GetGudSdk::AttackActionData data2(GetGudSdk::BaseData{}, std::string());
    GetGudSdk::DamageActionData data3(GetGudSdk::BaseData{}, 10, std::string(), std::string());
    GetGudSdk::HealActionData data4(GetGudSdk::BaseData{}, 10);
    GetGudSdk::PositionActionData data5(GetGudSdk::BaseData{}, std::vector<float>());

    return 0;
}