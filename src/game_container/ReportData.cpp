#include "ReportData.h"
#include "../src/utils/Validator.h"
#include "../src/utils/Sanitizer.h"
#include "../src/config/Config.h"

#ifdef __linux__
#include <limits.h>
#include <stdio.h>
#endif

namespace GetgudSDK {
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
			reportString += "\"matchGuid\":\"" + m_reportInfo.MatchGuid + "\",";
		if (!m_reportInfo.ReporterName.empty())
			reportString += "\"reporterName\":\"" + m_reportInfo.ReporterName + "\",";
		if (m_reportInfo.ReporterType != ReporterType::None)
			reportString +=
			"\"reporterType\":" + std::to_string(static_cast<int>(m_reportInfo.ReporterType)) + ",";
		if (m_reportInfo.ReporterSubType != ReporterSubtype::None)
			reportString +=
			"\"reporterSubType\":" + std::to_string(static_cast<int>(m_reportInfo.ReporterSubType)) +
			",";
		reportString +=
			"\"suspectedPlayerGuid\":\"" + m_reportInfo.SuspectedPlayerGuid + "\",";
		reportString += "\"TBType\":" + std::to_string(static_cast<int>(m_reportInfo.TbType)) + ",";
		if (m_reportInfo.TbTimeEpoch != -1)
			reportString +=
			"\"TBTimeEpoch\":" + std::to_string(m_reportInfo.TbTimeEpoch) + ",";
		if (m_reportInfo.SuggestedToxicityScore != -1)
			reportString += "\"suggestedToxicityScore\":" +
			std::to_string(m_reportInfo.SuggestedToxicityScore) + ",";
		if (m_reportInfo.ReportedTimeEpoch != -1)
			reportString += "\"reportedTimeEpoch\":" +
			std::to_string(m_reportInfo.ReportedTimeEpoch) + ",";
		reportString.pop_back(); // remove last comma
		reportString += "}";

		return reportString;
	}

	/**
	 * IsValid:
	 *
	 **/
	bool ReportData::IsValid() {
		// Core validations
		bool isCoreValid = Validator::ValidateStringLength(m_reportInfo.MatchGuid, 1, 36);
		isCoreValid &= Validator::ValidateStringChars(m_reportInfo.MatchGuid);
		isCoreValid &= Validator::ValidateStringLength(m_reportInfo.SuspectedPlayerGuid, 1, 36);
		isCoreValid &= Validator::ValidateStringChars(m_reportInfo.SuspectedPlayerGuid);
		isCoreValid &= Validator::ValidateItemValue(static_cast<int>(m_reportInfo.TbType), 1, static_cast<int>(TbType::AffectHacking));

		if (m_reportInfo.ReporterType != ReporterType::None)
		{
			isCoreValid &= Validator::ValidateItemValue(static_cast<int>(m_reportInfo.ReporterType), 0, static_cast<int>(ReporterType::Custom));
		}
		if (m_reportInfo.ReporterSubType != ReporterSubtype::None)
		{
			isCoreValid &= Validator::ValidateItemValue(static_cast<int>(m_reportInfo.ReporterSubType), 0, static_cast<int>(ReporterSubtype::AFK));
		}
		if (m_reportInfo.TbTimeEpoch != -1)
		{
			isCoreValid &= Validator::ValidateItemValue(
				m_reportInfo.TbTimeEpoch,
				sdkConfig.sdkValidatorConfig.minActionTimeEpochTime,
				sdkConfig.sdkValidatorConfig.maxActionTimeEpochTime);
		}
		if (m_reportInfo.SuggestedToxicityScore != -1)
		{
			isCoreValid &= Validator::ValidateItemValue(m_reportInfo.SuggestedToxicityScore, 1, 100);
		}
		if (m_reportInfo.ReportedTimeEpoch != -1)
		{
			isCoreValid &= Validator::ValidateItemValue(
				m_reportInfo.ReportedTimeEpoch,
				sdkConfig.sdkValidatorConfig.minActionTimeEpochTime,
				sdkConfig.sdkValidatorConfig.maxActionTimeEpochTime);
		}

		// Sanitize non-core fields
		if (!m_reportInfo.ReporterName.empty()) { // ReporterName is optional
		    Sanitizer::SanitizeStringChars(m_reportInfo.ReporterName);
		    Sanitizer::SanitizeStringLength(m_reportInfo.ReporterName, 36);
		}

		return isCoreValid;
	}
}  // namespace GetgudSDK