#include "../../include/actions/BaseActionData.h"

#include <deque>
#include <mutex>

namespace GetGudSdk
{

	class MatchData
	{
	private:
		std::string match_guid;
		std::string match_mode;
		std::string map_name;
		unsigned int actions_size = 0;
		unsigned int size_in_bytes = 0;
		//need reports list
		//need message list
		std::deque<BaseActionData*> actions;

	public:
		MatchData(std::string _match_guid, std::string _match_mode, std::string _map_name);
		std::string& get_match_guid();
	};
}