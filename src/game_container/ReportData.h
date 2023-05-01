#pragma once
#include <string>
#include <map>

#include "../../include/actions/Helpers.h"

namespace GetGudSdk
{
    class ReportData
    {
    private:
        ReportInfo info;

    public:

        /**
        * ReportData:
        * @report_info: All the report metadata in the ReportInfo struct
        *
        * Creates a new report that will be used in GameData Container
        **/
        ReportData(ReportInfo report_info);

        /**
        * get_data:
        *
        * Assigns the fields from ReportData class to the map data struct
        **/
        std::map<std::string, std::string> get_data();
    };
}