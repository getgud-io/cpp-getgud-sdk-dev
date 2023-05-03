#pragma once
#include <string>
#include <map>

#include "../../include/actions/Helpers.h"

namespace GetGudSdk
{
    class PlayerData
    {
    private:
        PlayerInfo player_info;

    public:

        PlayerData(PlayerInfo player_info);
        std::map<std::string, std::string> player_data_to_json_string();
    };
}