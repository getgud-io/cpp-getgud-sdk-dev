#pragma once
#include <string>
#include <map>

#include "../../include/actions/Helpers.h"

namespace GetGudSdk
{
  unsigned int GetPlayerDataSize();
    class PlayerData
    {

    public:

        PlayerInfo playerInfo;
        int titleId;
        std::string privateKey;

        PlayerData(int titleId, std::string privateKey, PlayerInfo playerInfo);
        std::string ToString();
        void Dispose();


    };
}