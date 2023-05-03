#include "ReportData.h"

namespace GetGudSdk
{
	/**
	* ReportData:
	* @report_info: All the report metadata in the ReportInfo struct
	*
	* Creates a new report that will be used in GameData Container
	**/
	ReportData::ReportData(ReportInfo report_info)
		: info(report_info)
	{

	}

	/**
	* get_data:
	*
	* Assigns the fields from ReportData class to the map data struct
	**/
	std::map<std::string, std::string> ReportData::get_data()
	{
		std::map<std::string, std::string> data;

		data["match_guid"] = info.match_guid;
		data["reporter_name"] = info.reporter_name;
		data["reporter_type"] = std::to_string(info.reporter_type);
		data["reporter_sub_type"] = std::to_string(info.reporter_sub_type);
		data["suspected_player_guid"] = info.suspected_player_guid;
		data["tb_type"] = std::to_string(info.tb_type);
		data["tb_sub_type"] = std::to_string(info.tb_sub_type);
		data["tb_time_epoch"] = std::to_string(info.tb_time_epoch);
		data["suggested_toxicity_score"] = std::to_string(info.suggested_toxicity_score);
		data["reported_time_epoch"] = std::to_string(info.reported_time_epoch);

		return data;
	}
}