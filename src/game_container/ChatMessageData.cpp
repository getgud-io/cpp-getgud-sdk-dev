#include "ChatMessageData.h"

namespace GetGudSdk {

/**
 * ChatMessageData:
 * 
 **/
ChatMessageData::ChatMessageData(std::string playerGuid,
                                 std::string message,
                                 long long messageTimeEpoch)
    : playerGuid(playerGuid),
      message(message),
      messageTimeEpoch(messageTimeEpoch) {}

/**
 * ToString:
 *
 * Used when we send chat messages to get good along with match actions
 **/
std::string ChatMessageData::ToString() {
  std::string messageString;
  messageString += "{";
  messageString += "	\"playerGuid\": \"" + playerGuid + "\",";
  messageString += "	\"message\": \"" + message + "\",";
  messageString +=
      "	\"messageTimeEpoch\": " + std::to_string(messageTimeEpoch);
  messageString += "}";

  return messageString;
}
}  // namespace GetGudSdk