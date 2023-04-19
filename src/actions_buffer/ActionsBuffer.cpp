#include "ActionsBuffer.h"

namespace GetGudSdk
{
	//create a new shared actions buffer
	ActionsBuffer actions_buffer;

	//return to out_action the first action in the buffer
	bool ActionsBuffer::get_first_action(BaseActionData* out_action)
	{
		buffer_locker.lock();
		out_action = *buffer.begin(); //copy value of the first element
		buffer.erase(buffer.begin());
		buffer_locker.unlock();

		return true;
	}

	//return all actions from the buffer, clean the buffer
	bool ActionsBuffer::get_actions(std::deque<BaseActionData*>* out_buffer)
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
	bool ActionsBuffer::push_action(BaseActionData* in_action)
	{
		buffer_locker.lock();
		buffer.push_back(in_action);
		buffer_locker.unlock();

		return true;
	}

	//append an array of actions to the bufer
	bool ActionsBuffer::push_actions(std::deque<BaseActionData*>& in_buffer)
	{
		buffer_locker.lock();
		buffer.insert(buffer.end(), in_buffer.begin(), in_buffer.end());
		buffer_locker.unlock();

		return true;
	}

	void ActionsBuffer::dispose()
	{
		dispose_required = true; //set dispose to background thread

		//clear all buffers via lockers
		buffer_locker.lock();
		buffer.clear();
		buffer_locker.unlock();
	}
	
}