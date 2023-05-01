#include "ChatMessageData.h"

namespace GetGudSdk
{	
	/**
	* ChatMessageData:
	* @_player_guid: guid of the player who sent the message
	* @_message: message that the player sent
	*
	* Creates a new chat message that will be used in GameData Container
	**/
	ChatMessageData::ChatMessageData(std::string _player_guid, std::string _message)
		: player_guid(_player_guid), message(_message)
	{

	}

	/**
	* get_data:
	*
	* Assigns the fields from ChatMessageData class to the map data struct
	**/
	std::map<std::string, std::string> ChatMessageData::get_data()
	{
		std::map<std::string, std::string> data;

		data["player_guid"] = player_guid;
		data["message"] = message;

		return data;
	}
}