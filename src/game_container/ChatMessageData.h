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
		ChatMessageData(std::string _player_guid, std::string _message);
		std::map<std::string, std::string> get_data();
	};

}