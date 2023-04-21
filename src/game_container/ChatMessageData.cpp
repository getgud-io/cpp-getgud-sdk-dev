#include "ChatMessageData.h"

namespace GetGudSdk
{
	ChatMessageData::ChatMessageData(std::string _player_guid, std::string _message)
		: player_guid(_player_guid), message(_message)
	{

	}

	std::map<std::string, std::string> ChatMessageData::get_data()
	{
		std::map<std::string, std::string> data;

		//fill the buffer by key, value
		data["player_guid"] = player_guid;
		data["message"] = message;

		return data;
	}
}