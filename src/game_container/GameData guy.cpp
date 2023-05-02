#include "GameData.h"
#include "utils/Utils.h"
#include "../../include/config/Config.h"

namespace GetGudSdk
{
extern Config sdk_config;

    GameData::GameData(int titleId, std::string privateKey, std::string serverGuid, std::string gameMode)
        : aTitle_id(titleId), aPrivate_key(privateKey), aServer_guid(serverGuid), aGame_mode(gameMode)
    {
        // the only thing left to do aside initializing all the above meta data fields, is to create a game guid
        aGame_guid = generate_guid();
    }   
}
