#pragma once

#include <string>
#include <random>
#include <vector>

#include "../../include/actions/Helpers.h"

namespace GetGudSdk
{	
	/**
	* generate_guid:
	*
	* generate game or match guid, we will use this when we create new match or new game
	**/
	std::string generate_guid();

	/**
	* position_to_string:
	*
	* Generate positon string in format x~y~z~pitch~roll from data structures containing
	* position and rotation of the player. This will be used when we send actions to getgud to
	* put action strings in the packet json
	**/
	std::string position_to_string(PositionF _position, RotationF _rotation);

	/**
	* get_current_time_string:
	*
	* Get current time stamp as string in format %Y-%m-%d %X 
	**/
	std::string get_current_time_string();
}