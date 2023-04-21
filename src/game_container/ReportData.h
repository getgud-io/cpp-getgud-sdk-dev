#pragma once
#include <string>
#include <map>

#include "..\..\include\actions\Helpers.h"

namespace GetGudSdk
{
    class ReportData
    {
    private:
        ReportInfo info;

    public:
        ReportData(ReportInfo report_info);
        std::map<std::string, std::string> get_data();
    };
}