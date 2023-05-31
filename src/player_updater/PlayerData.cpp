#include "pch.h"
#include "PlayerData.h"
#include <algorithm>
#include "../logger/Logger.h"
#include "../utils/Utils.h"
#include "../utils/Validator.h"
#include "../config/Config.h"

#ifdef __linux__
#include <limits.h>
#include <stdio.h>
#endif

namespace GetGudSdk {
extern Config sdkConfig;

/**
 * PlayerData:
 *
 **/
PlayerData::PlayerData(int titleId,
                       std::string privateKey,
                       PlayerInfo playerInfo)
    : m_titleId(titleId), m_privateKey(privateKey), m_playerInfo(playerInfo) {}

/**
 * ToString:
 *
 * Is used when sending JSON data to Getgud
 **/
std::string PlayerData::ToString() {
  std::string playerString;
  playerString += "{";
  playerString += "\"playerGuid\": \"" + m_playerInfo.PlayerGuid + "\",";
  if (m_playerInfo.PlayerNickname != "")
    playerString += "\"playerNickname\": \"" + m_playerInfo.PlayerNickname + "\",";
  if (m_playerInfo.PlayerEmail != "")
    playerString += "\"playerEmail\": \"" + m_playerInfo.PlayerEmail + "\",";
  if (m_playerInfo.PlayerRank != -1)
    playerString +=
        "\"playerRank\": " + std::to_string(m_playerInfo.PlayerRank) + ",";
  if (m_playerInfo.PlayerJoinDateEpoch != -1)
    playerString += "\"playerJoinDateEpoch\": " +
                    std::to_string(m_playerInfo.PlayerJoinDateEpoch) + ",";
  playerString.pop_back(); // remove last comma
  playerString += "}";

  return playerString;
}

/**
 * GetPlayerDataSize:
 **/
unsigned int GetPlayerDataSize() {
  int size = 0;
  size += 36 * sizeof(char);  // playerGuid size
  size += 20 * sizeof(char);  // PlayerNickname size
  size += 20 * sizeof(char);  // PlayerEmail size
  size += sizeof(int);        // PlayerRank size
  size += sizeof(long long);  // PlayerJoinDateEpoch size
  return size;
}

/**
 * Dispose:
 **/
void PlayerData::Dispose() {}

bool PlayerData::IsValid() {
  bool isActionValid =
      Validator::ValidateStringLength(m_playerInfo.PlayerGuid, 1, 36);
  isActionValid &= Validator::ValidateStringChars(m_playerInfo.PlayerGuid);
  isActionValid &=
      Validator::ValidateStringLength(m_playerInfo.PlayerNickname, 0, 10000);
  //isActionValid &= Validator::ValidateStringChars(m_playerInfo.PlayerNickname);
  isActionValid &=
      Validator::ValidateStringLength(m_playerInfo.PlayerEmail, 0, 10000);
  //isActionValid &= Validator::ValidateStringChars(m_playerInfo.PlayerEmail);
  if (m_playerInfo.PlayerRank != -1)
    isActionValid &=
        Validator::ValidateItemValue(m_playerInfo.PlayerRank, 0, INT_MAX);
  if (m_playerInfo.PlayerJoinDateEpoch != -1)
    isActionValid &= Validator::ValidateItemValue(
        m_playerInfo.PlayerJoinDateEpoch,
        sdkConfig.sdkValidatorConfig.minActionTimeEpochTime,
        sdkConfig.sdkValidatorConfig.maxActionTimeEpochTime);
  return isActionValid;
}
}  // namespace GetGudSdk