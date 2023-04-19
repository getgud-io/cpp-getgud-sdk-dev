#include "MatchData.h"

namespace GetGudSdk
{
	MatchData::MatchData(std::string _match_guid, std::string _match_mode, std::string _map_name)
		: match_guid(_match_guid), match_mode(_match_mode), map_name(_map_name)
	{

	}

	std::string& MatchData::get_match_guid()
	{
		return match_guid;
	}
}