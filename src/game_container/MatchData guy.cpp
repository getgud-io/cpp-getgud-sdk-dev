#include "MatchData.guy.h"
#include "utils/Utils.h"
#include <algorithm>

namespace GetGudSdk
{	

	MatchData::MatchData(std::string matchMode, std::string mapName)
		: aMatch_mode(matchMode), aMap_name(mapName)
	{
        // the only thing left to do aside initializing all the above meta data fields, is to create a match guid
        aMatch_guid = generate_guid();
	}

	void MatchData::AddActions(std::vector<BaseActionData*>& actionVector)
	{
		// add the newly arrived action vector to the current action vector, where all the actions are waiting to be processed
		aAction_vector.add(&actionVector); // TODO: figure out the syntax here - NO NEED FOR COPY!!! THIS IS OUR OBJECTS!!! just add
	}
}