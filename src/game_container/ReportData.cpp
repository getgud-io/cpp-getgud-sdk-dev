#include "ReportData.h"

namespace GetGudSdk {

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
ReportData::ReportData(int titleId, std::string privateKey, ReportInfo reportInfo)
    : reportInfo(reportInfo), titleId(titleId), privateKey(privateKey) {}

/**
 * ToString:
 *
 * Used for sending reports to Getgud
 **/
std::string ReportData::ToString() {
  std::string reportString;
  reportString += "{";
  reportString += "	\"reporterName\": \"" + reportInfo.ReporterName + "\",";
  reportString +=
      "	\"reporterType\": " + std::to_string(reportInfo.ReporterType) + ",";
  reportString +=
      "	\"reporterSubType\": " + std::to_string(reportInfo.ReporterSubType) +
      ",";
  reportString +=
      "	\"suspectedPlayerGuid\": \"" + reportInfo.SuspectedPlayerGuid + "\",";
  reportString += "	\"TBType\": " + std::to_string(reportInfo.TbType) + ",";
  reportString +=
      "	\"TBSubType\": " + std::to_string(reportInfo.TbSubType) + ",";
  reportString +=
      "	\"TBTimeEpoch\": " + std::to_string(reportInfo.TbTimeEpoch) + ",";
  reportString += "	\"suggestedToxicityScore\": " +
                  std::to_string(reportInfo.SuggestedToxicityScore) + ",";
  reportString +=
      "	\"reportedTimeEpoch\": " + std::to_string(reportInfo.ReportedTimeEpoch);
  reportString += "}";

  return reportString;
}
}  // namespace GetGudSdk