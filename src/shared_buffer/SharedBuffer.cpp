#include "SharedBuffer.h"

namespace GetGudSdk
{
	//create a new shared buffer
	SharedBuffer buffer;

	//return to out_action the first action in the buffer
	bool SharedBuffer::get_first_action(BaseActionData* out_action)
	{
		buffer_locker.lock();
		out_action = *buffer.begin(); //copy value of the first element
		buffer.erase(buffer.begin());
		buffer_locker.unlock();

		return true;
	}

	//return all actions from the buffer, clean the buffer
	bool SharedBuffer::get_actions(std::deque<BaseActionData*>* out_buffer)
	{
		//make or clean a new buffer in order to swap it with the buffer
		if (out_buffer == nullptr)
			out_buffer = new std::deque<BaseActionData*>();
		else if (out_buffer->size())
			out_buffer->clear();

		buffer_locker.lock();
		out_buffer->swap(buffer); //fast processing function to move elements from the buffer
		buffer_locker.unlock();

		return true;
	}

	//append a new action at the end of the buffer
	bool SharedBuffer::push_action(BaseActionData* in_action)
	{
		buffer_locker.lock();
		buffer.push_back(in_action);
		buffer_locker.unlock();

		return true;
	}

	//append an array of actions to the bufer
	bool SharedBuffer::push_actions(std::deque<BaseActionData*>& in_buffer)
	{
		buffer_locker.lock();
		buffer.insert(buffer.end(), in_buffer.begin(), in_buffer.end());
		buffer_locker.unlock();

		return true;
	}

	bool SharedBuffer::push_game(GameData game_data)
	{
		game_buffer_locker.lock();
		start_game_buffer.push_back(game_data);
		game_buffer_locker.unlock();

		return true;
	}

	bool SharedBuffer::push_end_game(std::string game_guid)
	{
		std::chrono::system_clock::time_point delay_time = std::chrono::system_clock::now(); //get current system time
		delay_time += std::chrono::seconds(5); //append 5 seconds

		end_game_locker.lock();
		end_game_buffer.push_back(std::make_pair(delay_time, game_guid));
		end_game_locker.unlock();

		return true;
	}

	bool SharedBuffer::push_match(MatchData match_data)
	{
		match_buffer_locker.lock();
		start_match_buffer.push_back(match_data);
		match_buffer_locker.unlock();

		return true;
	}

	void SharedBuffer::dispose()
	{
		dispose_required = true; //set dispose to background thread

		//clear all buffers via lockers
		match_buffer_locker.lock();
		start_match_buffer.clear();
		match_buffer_locker.unlock();

		game_buffer_locker.lock();
		start_game_buffer.clear();
		game_buffer_locker.unlock();

		buffer_locker.lock();
		buffer.clear();
		buffer_locker.unlock();

		end_game_locker.lock();
		end_game_buffer.clear();
		end_game_locker.unlock();
	}
	
}