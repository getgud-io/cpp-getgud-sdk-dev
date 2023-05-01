#pragma once
#include <string>
#include <map>

namespace GetGudSdk
{
	class ChatMessageData
	{
	private:
		std::string player_guid;
		std::string message;

	public:
		/**
		* ChatMessageData:
		* @_player_guid: guid of the player who sent the message
		* @_message: message that the player sent
		*
		* Creates a new chat message that will be used in GameData Container
		**/
		ChatMessageData(std::string _player_guid, std::string _message);

		/**
		* get_data:
		*
		* Assigns the fields from ChatMessageData class to the map data struct
		**/
		std::map<std::string, std::string> get_data();
	};

}