#include "ReportData.h"

namespace GetGudSdk
{
	ReportData::ReportData(ReportInfo report_info)
		: info(report_info)
	{

	}

	std::map<std::string, std::string> ReportData::get_data()
	{
		//make a new buffer with <key, value> pairs
		std::map<std::string, std::string> data;

		//fill the buffer by key, value
		data["match_guid"] = info.match_guid;
		data["reporter_name"] = info.reporter_name;
		data["reporter_type"] = std::to_string(info.reporter_type);
		data["reporter_sub_type"] = std::to_string(info.reporter_sub_type);
		data["suspected_player_name"] = info.suspected_player_name;
		data["tb_type"] = std::to_string(info.tb_type);
		data["tb_sub_type"] = std::to_string(info.tb_sub_type);
		data["tb_time_epoch"] = std::to_string(info.tb_time_epoch);
		data["suggested_toxicity_score"] = std::to_string(info.suggested_toxicity_score);
		data["reported_time_epoch"] = std::to_string(info.reported_time_epoch);

		return data;
	}
}