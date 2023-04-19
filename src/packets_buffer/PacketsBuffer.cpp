#include "PacketsBuffer.h"

namespace GetGudSdk
{
	//create a new shared packets buffer
	PacketsBuffer packets_buffer;

	bool PacketsBuffer::push_game(GameData game_data)
	{
		buffer_locker.lock();

		//send end game if game already exists
		for (auto game : buffer) //parse every game in the buffer
		{
			if (game.get_game_guid() == game_data.get_game_guid()) //if game found
			{
				buffer_locker.unlock();
				return false; //game guid already exists
			}
		}

		buffer.push_back(game_data);
		buffer_locker.unlock();
		return true;
	}

	bool PacketsBuffer::push_end_game(std::string game_guid)
	{
		buffer_locker.lock();
		for (auto game : buffer) //parse every game in the buffer
		{
			if (game.get_game_guid() == game_guid) //if game found
			{
				auto current_point = std::chrono::system_clock::now(); // get current time

				game.set_end_game(current_point + std::chrono::seconds(5)); // add 5 sec delay to current time (config)
			}
		}
		buffer_locker.unlock();
		return true;
	}

	bool PacketsBuffer::push_match(std::string game_guid, MatchData match_data)
	{
		bool result = false;
		buffer_locker.lock();
		for (auto game : buffer) //parse every game in the buffer
		{
			if (game.get_game_guid() == game_guid) //if game found
			{
				result = game.push_match(match_data);
			}
		}
		buffer_locker.unlock();
		return result;
	}

	void PacketsBuffer::dispose()
	{
		buffer_locker.lock();
		buffer.clear();
		buffer_locker.unlock();
	}

}