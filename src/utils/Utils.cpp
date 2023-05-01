#include "Utils.h"

#include <chrono>
#include <ctime> 
#include <sstream>
#include <iomanip>
#include <string> 

// this variables are used for random guid generation
std::random_device rd; //a system based device that generates data randomly
std::uniform_int_distribution<int> dist(0, 35); //limit for the random device, 0-9 + a-z TODO: i don't understand this

namespace GetGudSdk
{
	/**
	* generate_guid:
	*
	* generate game or match guid, we will use this when we create new match or new game
	**/
	std::string generate_guid()
	{
		std::string output;
		int guid_string_length = 36; // this function only supports 36 guid lengths at the moment!

		// generate random guid char by char
		for (int i = 0; i < guid_string_length; i++)
		{	
			// this is relevant only for guid of length 36
			// we need to put dashes at hardcoded places
			if (i == 8 || i == 13 || i == 18 || i == 23){
				output += "-";
				guid_string_length = guid_string_length + 1; 
			}	
			// if we are not putting dashes, then we are generating random char for the guid
			else
			{
				int value = 0;
				value = dist(rd); //generate symbol randomly, using random generator and limit
				if (value <= 9) // if the symbol is number
					output += (value + 48); // int to char operation for number, + 48 comes from ASCII table
				else // if the symbol is letter
					output += (value + 97 - 10); // int to char operation for letter, 97-10 comes from ASCII table
			}
		}

		return output;
	}
	

	/**
	* get_current_time_string:
	*
	* Get current time stamp as string in format %Y-%m-%d %X 
	**/
	std::string get_current_time_string()
	{
		auto now = std::chrono::system_clock::now();
		auto in_time_t = std::chrono::system_clock::to_time_t(now);

		std::stringstream ss;
		ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
		return ss.str();
	}

	/**
	* position_to_string:
	*
	* Generate positon string in format x~y~z~pitch~roll from data structures containing
	* position and rotation of the player. This will be used when we send actions to getgud to
	* put action strings in the packet json
	**/
	std::string position_to_string(PositionF _position, RotationF _rotation)
	{
		std::string output_position;

		output_position += std::to_string(_position.x) + '~';
		output_position += std::to_string(_position.y) + '~';
		output_position += std::to_string(_position.z) + '~';

		output_position += std::to_string(_rotation.Pitch) + '~';
		output_position += std::to_string(_rotation.Roll);

		return output_position;
	}
}