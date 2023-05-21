#include "PlayerData.h"
#include <algorithm>
#include "../logger/Logger.h"
#include "../utils/Utils.h"

namespace GetGudSdk {

PlayerData::PlayerData(int titleId,
                       std::string privateKey,
                       PlayerInfo playerInfo)
    : titleId(titleId), privateKey(privateKey), playerInfo(playerInfo) {}

std::string PlayerData::ToString(){
  std::string playerString;
  playerString += "\"playerName\": \"" + playerInfo.PlayerGuid + "\",";
  playerString += "\"playerNickname\": \"" + playerInfo.PlayerNickname + "\",";
  playerString += "\"playerEmail\": \"" + playerInfo.PlayerEmail + "\",";
  playerString += "\"playerRank\": " + std::to_string(playerInfo.PlayerRank) + ",";
  playerString +=
      "\"playerJoinDateEpoch\": " + std::to_string(playerInfo.PlayerJoinDateEpoch) + "";

  return playerString;
}

unsigned int GetPlayerDataSize() {
  int size = 0;
  size += 36 * sizeof(char);  // playerGuid
  size += 20 * sizeof(char);  // PlayerNickname
  size += 20 * sizeof(char);  // PlayerEmail
  size += sizeof(int);  // PlayerRank
  size += sizeof(long long);  // PlayerJoinDateEpoch
  return size;
}

void PlayerData::Dispose() {}
}  // namespace GetGudSdk