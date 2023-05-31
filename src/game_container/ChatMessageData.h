#pragma once
#include <map>
#include <string>

namespace GetGudSdk {
class ChatMessageData {
 private:
  std::string m_playerGuid;
  std::string m_message;
  long long m_messageTimeEpoch;

 public:
  ChatMessageData(std::string playerGuid,
                  std::string message,
                  long long messageTimeEpoch);
  std::string ToString();
  bool IsValid();
};

}  // namespace GetGudSdk