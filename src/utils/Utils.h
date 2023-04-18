#pragma once

#include <string>
#include <random>
#include <vector>

#include "../../include/actions/Helpers.h"

namespace GetGudSdk
{
	std::string generate_guid();

	std::string position_to_string(PositionF _position, RotationF _rotation);
}