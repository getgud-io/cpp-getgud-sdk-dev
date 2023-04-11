#pragma once

#include <string>
#include <map>

class BaseActionData
{
public:
	int id = 0;
	int size = 0;
	int action_type = 0; //replace to enum
	long action_time_epoch = 0;
	std::string player_guid;
	std::string game_guid;
	std::string match_guid;

public:
	BaseActionData(std::string _player_guid, std::string _game_guid, std::string _match_guid,
		int _action_type, long _action_time_epoch);
	BaseActionData() = delete;
	~BaseActionData();
	virtual std::map<std::string, std::string> get_data();
};