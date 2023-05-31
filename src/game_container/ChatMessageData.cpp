#include "pch.h"
#include "ChatMessageData.h"
#include "../utils/Validator.h"
#include "../config/Config.h"

namespace GetGudSdk {
extern Config sdkConfig;

/**
 * ChatMessageData:
 * 
 **/
ChatMessageData::ChatMessageData(std::string playerGuid,
                                 std::string message,
                                 long long messageTimeEpoch)
    : m_playerGuid(playerGuid),
      m_message(message),
      m_messageTimeEpoch(messageTimeEpoch) {}

/**
 * ToString:
 *
 * Used when we send chat messages to get good along with match actions
 **/
std::string ChatMessageData::ToString() {
  std::string messageString;
  messageString += "{";
  messageString += "	\"playerGuid\": \"" + m_playerGuid + "\",";
  messageString += "	\"message\": \"" + m_message + "\",";
  messageString +=
      "	\"messageTimeEpoch\": " + std::to_string(m_messageTimeEpoch);
  messageString += "}";

  return messageString;
}

bool ChatMessageData::IsValid() {
  bool isActionValid =
      Validator::ValidateStringLength(m_playerGuid, 1, 36);
  isActionValid &= Validator::ValidateStringChars(m_playerGuid);
  isActionValid &=
      Validator::ValidateStringLength(m_message, 1, 10000);
  isActionValid &= Validator::ValidateStringChars(m_message);
  isActionValid &= Validator::ValidateItemValue(
      m_messageTimeEpoch,
      sdkConfig.sdkValidatorConfig.minActionTimeEpochTime,
      sdkConfig.sdkValidatorConfig.maxActionTimeEpochTime);
  return isActionValid;
}
}  // namespace GetGudSdk