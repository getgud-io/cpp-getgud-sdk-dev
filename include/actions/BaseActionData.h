#pragma once

#include <map>
#include <string>

#include "Helpers.h"

namespace GetGudSdk {
unsigned int GetEmptyActionSize();

class BaseActionData {
 public:
  Actions actionType = Actions::None;
  long long actionTimeEpoch = 0;
  std::string playerGuid;
  std::string matchGuid;
  bool isEmpty = false;

 public:
  BaseActionData(BaseData data, bool _isEmpty = false);
  BaseActionData(const BaseActionData& data);
  BaseActionData() = delete;
  virtual ~BaseActionData();
  virtual bool IsValid();
  virtual std::string ToString();
  virtual std::string ToStringMeta();
  virtual BaseActionData* Clone();

  // TODO implement Clone function in order to prevent deletion control of
  // external data
};
}  // namespace GetGudSdk