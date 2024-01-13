#include "pch.h"
#include "PlayerData.h"
#include <algorithm>
#include "../../logger/Logger.h"
#include "../../utils/Utils.h"
#include "../../utils/Validator.h"
#include "../../config/Config.h"

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
  playerString += "\"playerGuid\":\"" + m_playerInfo.PlayerGuid + "\",";
  if (!m_playerInfo.PlayerNickname.empty())
    playerString += "\"playerNickname\":\"" + m_playerInfo.PlayerNickname + "\",";
  if (!m_playerInfo.PlayerEmail.empty())
    playerString += "\"playerEmail\":\"" + m_playerInfo.PlayerEmail + "\",";
  if (m_playerInfo.PlayerRank != -1)
    playerString +=
        "\"playerRank\":" + std::to_string(m_playerInfo.PlayerRank) + ",";
  if (m_playerInfo.PlayerJoinDateEpoch != -1)
    playerString += "\"playerJoinDateEpoch\":" +
                    std::to_string(m_playerInfo.PlayerJoinDateEpoch) + ",";
  if (!m_playerInfo.PlayerSuspectScore.empty())
      playerString += "\"playerSuspectScore\":\"" + m_playerInfo.PlayerSuspectScore + "\",";
  if (!m_playerInfo.PlayerReputation.empty())
      playerString += "\"playerReputation\":\"" + m_playerInfo.PlayerReputation + "\",";
  if (!m_playerInfo.PlayerStatus.empty())
      playerString += "\"playerStatus\":\"" + m_playerInfo.PlayerStatus + "\",";
  if (!m_playerInfo.PlayerCampaign.empty())
      playerString += "\"playerCampaign\":\"" + m_playerInfo.PlayerCampaign + "\",";
  if (!m_playerInfo.PlayerNotes.empty())
      playerString += "\"playerNotes\":\"" + m_playerInfo.PlayerNotes + "\",";
  if (!m_playerInfo.PlayerDevice.empty())
      playerString += "\"playerDevice\":\"" + m_playerInfo.PlayerDevice + "\",";
  if (!m_playerInfo.PlayerOS.empty())
      playerString += "\"playerOS\":\"" + m_playerInfo.PlayerOS + "\",";
  if (m_playerInfo.PlayerAge != -1)
      playerString += "\"playerAge\":" + std::to_string(m_playerInfo.PlayerAge) + ",";
  if (!m_playerInfo.PlayerGender.empty())
      playerString += "\"playerGender\":\"" + m_playerInfo.PlayerGender + "\",";
  if (!m_playerInfo.PlayerLocation.empty())
      playerString += "\"playerLocation\":\"" + m_playerInfo.PlayerLocation + "\",";

  if (m_playerInfo.Transactions.size() != 0)
  {
      playerString += "\"playerTransactions\":[";
      for (auto& playerTransaction : m_playerInfo.Transactions)
      {
          playerString += "{";
          playerString += "\"transactionGuid\":\"" + playerTransaction.TransactionGuid + "\",";
          playerString += "\"transactionName\":\"" + playerTransaction.TransactionName + "\",";
          playerString += "\"transactionDateEpoch\":\"" + std::to_string(playerTransaction.TransactionDateEpoch) + "\",";
          playerString += "\"transactionValueUSD\":\"" + ShortenDecimalNumber(std::to_string(playerTransaction.TransactionValueUSD)) + "\"";
          playerString += "},";
      }
      playerString.pop_back(); // remove last comma
      playerString += "]";
  }

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
  if (!m_playerInfo.PlayerNickname.empty())
  {
    isActionValid &=
      Validator::ValidateStringLength(m_playerInfo.PlayerNickname, 0, 36);
    isActionValid &= Validator::ValidateStringCharsSpecial(m_playerInfo.PlayerNickname);
  }
  if (!m_playerInfo.PlayerEmail.empty())
  {
    isActionValid &=
      Validator::ValidateStringLength(m_playerInfo.PlayerEmail, 0, 64);
    isActionValid &= Validator::ValidateStringCharsSpecial(m_playerInfo.PlayerEmail);
  }
  if (m_playerInfo.PlayerRank != -1)
  {
    isActionValid &=
      Validator::ValidateItemValue(m_playerInfo.PlayerRank, 0, INT_MAX);
  }
  if (m_playerInfo.PlayerJoinDateEpoch != -1)
  {
    isActionValid &= Validator::ValidateItemValue(
      m_playerInfo.PlayerJoinDateEpoch,
      sdkConfig.sdkValidatorConfig.minActionTimeEpochTime,
      sdkConfig.sdkValidatorConfig.maxActionTimeEpochTime);
  }
  if (!m_playerInfo.PlayerSuspectScore.empty())
  {
      isActionValid &=
          Validator::ValidateStringLength(m_playerInfo.PlayerSuspectScore, 0, 100);
      isActionValid &= Validator::ValidateStringCharsSpecial(m_playerInfo.PlayerSuspectScore);
  }
  if (!m_playerInfo.PlayerReputation.empty())
  {
      isActionValid &=
          Validator::ValidateStringLength(m_playerInfo.PlayerReputation, 0, 36);
      isActionValid &= Validator::ValidateStringCharsSpecial(m_playerInfo.PlayerReputation);
  }
  if (!m_playerInfo.PlayerStatus.empty())
  {
      isActionValid &=
          Validator::ValidateStringLength(m_playerInfo.PlayerStatus, 0, 36);
      isActionValid &= Validator::ValidateStringCharsSpecial(m_playerInfo.PlayerStatus);
  } 
  if (!m_playerInfo.PlayerCampaign.empty())
  {
      isActionValid &=
          Validator::ValidateStringLength(m_playerInfo.PlayerCampaign, 0, 128);
      isActionValid &= Validator::ValidateStringCharsSpecial(m_playerInfo.PlayerCampaign);
  }
  if (!m_playerInfo.PlayerNotes.empty())
  {
      isActionValid &=
          Validator::ValidateStringLength(m_playerInfo.PlayerNotes, 0, 128);
      isActionValid &= Validator::ValidateStringCharsSpecial(m_playerInfo.PlayerNotes);
  }
  if (!m_playerInfo.PlayerDevice.empty())
  {
      isActionValid &=
          Validator::ValidateStringLength(m_playerInfo.PlayerDevice, 0, 8);
      isActionValid &= Validator::ValidateStringCharsSpecial(m_playerInfo.PlayerDevice);
  }
  if (!m_playerInfo.PlayerOS.empty())
  {
      isActionValid &=
          Validator::ValidateStringLength(m_playerInfo.PlayerOS, 0, 8);
      isActionValid &= Validator::ValidateStringCharsSpecial(m_playerInfo.PlayerOS);
  }
  if (m_playerInfo.PlayerAge != -1)
  {
      isActionValid &=
          Validator::ValidateItemValue(m_playerInfo.PlayerAge, 0, 128);
  }
  if (!m_playerInfo.PlayerGender.empty())
  {
      isActionValid &=
          Validator::ValidateStringLength(m_playerInfo.PlayerGender, 0, 8);
      isActionValid &= Validator::ValidateStringCharsSpecial(m_playerInfo.PlayerGender);
  }
  if (!m_playerInfo.PlayerLocation.empty())
  {
      isActionValid &=
          Validator::ValidateStringLength(m_playerInfo.PlayerLocation, 0, 36);
      isActionValid &= Validator::ValidateStringCharsSpecial(m_playerInfo.PlayerLocation);
  }
  if (!m_playerInfo.Transactions.empty())
  {
      for (auto& playerTransaction : m_playerInfo.Transactions)
      {
          isActionValid &=
              Validator::ValidateStringLength(playerTransaction.TransactionGuid, 0, 36);
          isActionValid &= Validator::ValidateStringCharsSpecial(playerTransaction.TransactionGuid);

          isActionValid &=
              Validator::ValidateStringLength(playerTransaction.TransactionName, 0, 36);
          isActionValid &= Validator::ValidateStringCharsSpecial(playerTransaction.TransactionName);

          isActionValid &= Validator::ValidateItemValue(
              playerTransaction.TransactionDateEpoch,
              sdkConfig.sdkValidatorConfig.minActionTimeEpochTime,
              sdkConfig.sdkValidatorConfig.maxActionTimeEpochTime);

          isActionValid &=
              Validator::ValidateItemValue(playerTransaction.TransactionValueUSD, 0.f, FLT_MAX);
      }
  }

  return isActionValid;
}
}  // namespace GetGudSdk