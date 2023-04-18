#include "Utils.h"

std::random_device rd; //a system based device that generates data randomly
std::uniform_int_distribution<int> dist(0, 35); //limit for the random device, 0-9 + a-z

namespace GetGudSdk
{
	std::string generate_guid()
	{
		std::string output;
		int guid_string_length = 36;
		for (int i = 0; i < guid_string_length; i++)
		{
			if (i == 8 || i == 13 || i == 18 || i == 23) // put dashes by guid syntax.
														 // every 9 sympol from first and end
														 // every 5 symbol at the middle
			{
				output += "-";
				guid_string_length = guid_string_length + 1; // increase string total length
			}	
			else //put symbol
			{
				int value = 0;
				value = dist(rd); //generate symbol randomly, using random generator and limit
				if (value <= 10) //verify if the symbol is number
					output += (value + 48); // int to char number, append 48 by ASCII table
				else //the symbol is letter
					output += (value + 97 - 10); // int to char letter, append 97 by ASCII table
												 // and decrease by integers amount

			}
		}

		return output;
	}

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