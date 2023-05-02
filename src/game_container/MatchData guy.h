#pragma once

#include "../../include/actions/BaseActionData.h"
#include "ChatMessageData.h"
#include "ReportData.h"
#include <deque>
#include <mutex>

namespace GetGudSdk
{
	class MatchData
	{
	private:

		// holds all the actions (in order) that arrived from the client and are waiting to be processed
		std::vector<GameData*> aAction_vector;

		// holds all the in match reports that are waiting to be processed
		std::vector<GameData*> aInMatchReport_vector;

		// holds all the chat massages that are waiting to be processed
		std::vector<GameData*> aChat_massage_vector;

		// helpers and data holders
		std::string aMatch_guid;
		std::string aMatch_mode;
		std::string aMap_name;
		unsigned int aActions_count = 0;
		unsigned int aSize_in_bytes = 0;

	public:
		
		MatchData(std::string matchMode, std::string mapName);
		
		std::string GetMatchGuid();

		unsigned int GetMatchSize();

		void AddActions(std::vector<BaseActionData*>& actionVector);

		void Dispose();
	};
}