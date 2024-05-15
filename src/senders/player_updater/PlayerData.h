#pragma once
#include <map>
#include <string>

#include "../../../include/actions/Helpers.h"

namespace GetgudSDK {
unsigned int GetPlayerDataSize();
class PlayerData {
 public:
  PlayerInfo m_playerInfo;
  int m_titleId;
  std::string m_privateKey;
  PlayerData(int titleId, std::string privateKey, PlayerInfo playerInfo);
  std::string ToString();
  void Dispose();
  bool IsValid();
};
}  // namespace GetgudSDK