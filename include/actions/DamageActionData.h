#pragma once
#include "BaseActionData.h"

namespace GetGudSdk {
class DamageActionData : public BaseActionData {
  std::string weaponGuid;
  std::string victimPlayerGuid;
  float damageDone = 0;

 public:
  DamageActionData(std::string matchGuid,
                   long long actionTimeEpoch,
                   std::string playerGuid,
                   std::string victimPlayerGuid,
                   float damageDone,
                   std::string weaponGuid);
  DamageActionData(const DamageActionData& data);
  DamageActionData() = delete;
  ~DamageActionData() override;
  bool IsValid() override;
  std::string ToString() override;
  std::string ToStringMeta() override;
  DamageActionData* Clone() override;
};
}  // namespace GetGudSdk