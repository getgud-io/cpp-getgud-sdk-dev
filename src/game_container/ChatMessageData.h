#pragma once
#include <map>
#include <string>

namespace GetGudSdk {
class ChatMessageData {
 private:
  std::string playerGuid;
  std::string message;
  long long messageTimeEpoch;

 public:
  ChatMessageData(std::string playerGuid,
                  std::string message,
                  long long messageTimeEpoch);
  std::string ToString();
};

}  // namespace GetGudSdk