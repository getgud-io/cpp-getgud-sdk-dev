#pragma once
#include <map>
#include <string>

#include "../../include/actions/Helpers.h"

namespace GetgudSDK {
	unsigned int GetReportDataSize();

	class ReportData {
	public:
		ReportInfo m_reportInfo;
		ReportData(ReportInfo reportInfo);
		std::string ToString(bool isOutsideMatch = false);
		virtual bool IsValid();
	};
}  // namespace GetgudSDK