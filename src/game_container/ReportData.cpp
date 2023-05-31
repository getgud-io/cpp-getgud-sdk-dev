#include "pch.h"
#include "ReportData.h"
#include "../src/utils/Validator.h"
#include "../src/config/Config.h"

#ifdef __linux__
#include <limits.h>
#include <stdio.h>
#endif

namespace GetGudSdk {
extern Config sdkConfig;
/**
 * GetReportDataSize:
 *
 * We use this to calculate report size in ReportSender outside of the 
 * live games
 **/
unsigned int GetReportDataSize() {
  int size = 0;
  size += 40 * sizeof(char);  // matchGuid size
  size += 20 * sizeof(char);  // reporterName size
  size += sizeof(int);        // reporterType size
  size += sizeof(int);        // reporterSubType size
  size += 40 * sizeof(char);  // suspectedPlayerGuid size
  size += sizeof(int);        // TBType size
  size += sizeof(int);        // TBSubType size
  size += sizeof(long long);  // TBTimeEpoch size
  size += sizeof(int);        // SuggestedToxicityScore size
  size += sizeof(long long);  // TBTimeEpoch size
  return size;
}

/**
 * ReportData:
 * 
 **/
ReportData::ReportData(ReportInfo reportInfo)
    : m_reportInfo(reportInfo) {}

/**
 * ToString:
 *
 * Used for sending reports to Getgud
 **/
std::string ReportData::ToString(bool isOutsideMatch) {
  std::string reportString;
  reportString += "{";
  if (isOutsideMatch)
    reportString += "	\"matchGuid\": \"" + m_reportInfo.MatchGuid + "\",";
  reportString += "	\"reporterName\": \"" + m_reportInfo.ReporterName + "\",";
  if (m_reportInfo.ReporterType != -1)
    reportString +=
        "	\"reporterType\": " + std::to_string(m_reportInfo.ReporterType) + ",";
  if (m_reportInfo.ReporterSubType != -1)
    reportString +=
        "	\"reporterSubType\": " + std::to_string(m_reportInfo.ReporterSubType) +
        ",";
  reportString +=
      "	\"suspectedPlayerGuid\": \"" + m_reportInfo.SuspectedPlayerGuid + "\",";
  if (m_reportInfo.TbType != -1)
    reportString += "	\"TBType\": " + std::to_string(m_reportInfo.TbType) + ",";
  if (m_reportInfo.TbSubType != -1)
    reportString +=
        "	\"TBSubType\": " + std::to_string(m_reportInfo.TbSubType) + ",";
  if (m_reportInfo.TbTimeEpoch != -1)
    reportString +=
        "	\"TBTimeEpoch\": " + std::to_string(m_reportInfo.TbTimeEpoch) + ",";
  if (m_reportInfo.SuggestedToxicityScore != -1)
    reportString += "	\"suggestedToxicityScore\": " +
                    std::to_string(m_reportInfo.SuggestedToxicityScore) + ",";
  if (m_reportInfo.ReportedTimeEpoch != -1)
    reportString += "	\"reportedTimeEpoch\": " +
                    std::to_string(m_reportInfo.ReportedTimeEpoch) + ",";
  reportString.pop_back(); // remove last comma
  reportString += "}";

  return reportString;
}

bool ReportData::IsValid() {
  bool isActionValid =
      Validator::ValidateStringLength(m_reportInfo.MatchGuid, 1, 36);
  isActionValid &= Validator::ValidateStringChars(m_reportInfo.MatchGuid);
  isActionValid &=
      Validator::ValidateStringLength(m_reportInfo.ReporterName, 1, 10000);
  isActionValid &= Validator::ValidateStringChars(m_reportInfo.ReporterName);
 
  if (m_reportInfo.ReporterType != -1)
    isActionValid &= 
      Validator::ValidateItemValue(m_reportInfo.ReporterType, 1, INT_MAX);
  if (m_reportInfo.ReporterSubType != -1)
    isActionValid &=
        Validator::ValidateItemValue(m_reportInfo.ReporterSubType, 1, INT_MAX);
  isActionValid &= Validator::ValidateStringLength(
      m_reportInfo.SuspectedPlayerGuid, 1, 36);
  isActionValid &=
      Validator::ValidateStringChars(m_reportInfo.SuspectedPlayerGuid);
  if (m_reportInfo.TbType != -1)
    isActionValid &=
        Validator::ValidateItemValue(m_reportInfo.TbType, 1, INT_MAX);
  if (m_reportInfo.TbSubType != -1)
    isActionValid &=
        Validator::ValidateItemValue(m_reportInfo.TbSubType, 1, INT_MAX);
  if (m_reportInfo.TbTimeEpoch != -1)
    isActionValid &= Validator::ValidateItemValue(
        m_reportInfo.TbTimeEpoch,
        sdkConfig.sdkValidatorConfig.minActionTimeEpochTime,
        sdkConfig.sdkValidatorConfig.maxActionTimeEpochTime);
  if (m_reportInfo.SuggestedToxicityScore != -1)
    isActionValid &=
        Validator::ValidateItemValue(m_reportInfo.SuggestedToxicityScore, 1, 100);
  if (m_reportInfo.ReportedTimeEpoch != -1)
    isActionValid &= Validator::ValidateItemValue(
        m_reportInfo.ReportedTimeEpoch,
        sdkConfig.sdkValidatorConfig.minActionTimeEpochTime,
        sdkConfig.sdkValidatorConfig.maxActionTimeEpochTime);
  return isActionValid;
}
}  // namespace GetGudSdk