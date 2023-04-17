#pragma once

#include <string>
#include <random>
#include <vector>

namespace GetGudSdk
{
	std::string generate_guid();

	std::string position_to_string(std::vector<float> in_position);
}