#pragma once
#include <map>
#include <string>

#include "../../include/actions/Helpers.h"

namespace GetGudSdk {
unsigned int GetReportDataSize();

class ReportData {
 public:
  int titleId = 0;
  std::string privateKey;
  ReportInfo reportInfo;
  ReportData(int titleId, std::string privateKey, ReportInfo reportInfo);
  std::string ToString();
};
}  // namespace GetGudSdk