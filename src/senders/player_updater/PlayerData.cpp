#include "PlayerData.h"
#include <algorithm>
#include "../../logger/Logger.h"
#include "../../utils/Utils.h"
#include "../../utils/Validator.h"
#include "../../utils/Sanitizer.h"
#include "../../config/Config.h"

#if defined(__linux__) || defined(__APPLE__)
#include <limits.h>
#include <cfloat>
#include <stdio.h>
#endif

namespace GetgudSDK {
	extern Config sdkConfig;

	/**
	 * PlayerData:
	 *
	 **/
	PlayerData::PlayerData(int titleId,
		std::string privateKey,
		PlayerInfo playerInfo
		)
		: m_playerInfo(playerInfo), m_titleId(titleId), m_privateKey(privateKey) {}

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
		// Core validations
		bool isCoreValid = Validator::ValidateStringLength(m_playerInfo.PlayerGuid, 1, 36);
		isCoreValid &= Validator::ValidateStringChars(m_playerInfo.PlayerGuid);
		isCoreValid &= Validator::ValidateStringLength(m_privateKey, 1, 100);
		isCoreValid &= Validator::ValidateStringChars(m_privateKey);
		isCoreValid &= Validator::ValidateItemValue(m_titleId, 1, INT_MAX);

		if (m_playerInfo.PlayerRank != -1)
		{
			isCoreValid &= Validator::ValidateItemValue(m_playerInfo.PlayerRank, 0, INT_MAX);
		}
		if (m_playerInfo.PlayerJoinDateEpoch != -1)
		{
			isCoreValid &= Validator::ValidateItemValue(
				m_playerInfo.PlayerJoinDateEpoch,
				sdkConfig.sdkValidatorConfig.minActionTimeEpochTime,
				sdkConfig.sdkValidatorConfig.maxActionTimeEpochTime);
		}
		// Sanitize non-core fields
		if (!m_playerInfo.PlayerNickname.empty())
		{
			Sanitizer::SanitizeStringCharsSpecial(m_playerInfo.PlayerNickname);
			Sanitizer::SanitizeStringLength(m_playerInfo.PlayerNickname, 36);
		}
		if (!m_playerInfo.PlayerEmail.empty())
		{
			Sanitizer::SanitizeStringCharsSpecial(m_playerInfo.PlayerEmail);
			Sanitizer::SanitizeStringLength(m_playerInfo.PlayerEmail, 64);
		}
		if (!m_playerInfo.PlayerSuspectScore.empty())
		{
			Sanitizer::SanitizeStringCharsSpecial(m_playerInfo.PlayerSuspectScore);
			Sanitizer::SanitizeStringLength(m_playerInfo.PlayerSuspectScore, 100);
		}
		if (!m_playerInfo.PlayerReputation.empty())
		{
			Sanitizer::SanitizeStringCharsSpecial(m_playerInfo.PlayerReputation);
			Sanitizer::SanitizeStringLength(m_playerInfo.PlayerReputation, 36);
		}
		if (!m_playerInfo.PlayerStatus.empty())
		{
			Sanitizer::SanitizeStringCharsSpecial(m_playerInfo.PlayerStatus);
			Sanitizer::SanitizeStringLength(m_playerInfo.PlayerStatus, 36);
		}
		if (!m_playerInfo.PlayerCampaign.empty())
		{
			Sanitizer::SanitizeStringCharsSpecial(m_playerInfo.PlayerCampaign);
			Sanitizer::SanitizeStringLength(m_playerInfo.PlayerCampaign, 128);
		}
		if (!m_playerInfo.PlayerNotes.empty())
		{
			Sanitizer::SanitizeStringCharsSpecial(m_playerInfo.PlayerNotes);
			Sanitizer::SanitizeStringLength(m_playerInfo.PlayerNotes, 128);
		}
		if (!m_playerInfo.PlayerDevice.empty())
		{
			Sanitizer::SanitizeStringCharsSpecial(m_playerInfo.PlayerDevice);
			Sanitizer::SanitizeStringLength(m_playerInfo.PlayerDevice, 8);
		}
		if (!m_playerInfo.PlayerOS.empty())
		{
			Sanitizer::SanitizeStringCharsSpecial(m_playerInfo.PlayerOS);
			Sanitizer::SanitizeStringLength(m_playerInfo.PlayerOS, 8);
		}
		if (!m_playerInfo.PlayerGender.empty())
		{
			Sanitizer::SanitizeStringCharsSpecial(m_playerInfo.PlayerGender);
			Sanitizer::SanitizeStringLength(m_playerInfo.PlayerGender, 8);
		}
		if (!m_playerInfo.PlayerLocation.empty())
		{
			Sanitizer::SanitizeStringCharsSpecial(m_playerInfo.PlayerLocation);
			Sanitizer::SanitizeStringLength(m_playerInfo.PlayerLocation, 36);
		}
		if (!m_playerInfo.Transactions.empty())
		{
			for (auto& playerTransaction : m_playerInfo.Transactions)
			{
				// Core validation for transaction numerics/timestamps
				isCoreValid &= Validator::ValidateItemValue(
					playerTransaction.TransactionDateEpoch,
					sdkConfig.sdkValidatorConfig.minActionTimeEpochTime,
					sdkConfig.sdkValidatorConfig.maxActionTimeEpochTime);
				isCoreValid &= Validator::ValidateItemValue(playerTransaction.TransactionValueUSD, 0.f, FLT_MAX);

				// Sanitize transaction strings
				Sanitizer::SanitizeStringCharsSpecial(playerTransaction.TransactionGuid);
				Sanitizer::SanitizeStringLength(playerTransaction.TransactionGuid, 36);
				Sanitizer::SanitizeStringCharsSpecial(playerTransaction.TransactionName);
				Sanitizer::SanitizeStringLength(playerTransaction.TransactionName, 36);
			}
		}

		return isCoreValid;
	}

}  // namespace GetgudSDK

