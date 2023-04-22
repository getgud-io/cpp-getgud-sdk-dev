#include "ActionsBuffer.h"

namespace GetGudSdk
{
	//create a new shared actions buffer
	ActionsBuffer actions_buffer;

	//return to out_action the first action in the buffer
	bool ActionsBuffer::get_first_action(BaseActionData* out_action)
	{
		out_action = *unified_actions.begin(); //copy value of the first element
		unified_actions.erase(unified_actions.begin());

		return true;
	}

	//return all actions from the buffer, clean the buffer
	bool ActionsBuffer::get_actions(std::deque<BaseActionData*>* out_buffer)
	{
		if (unified_actions.size() == 0) //check if buffer doesnt have any action
			return false; //return error

		//make or clean a new buffer in order to swap it with the buffer
		if (out_buffer == nullptr)
			out_buffer = new std::deque<BaseActionData*>();
		else if (out_buffer->size())
			out_buffer->clear();

		out_buffer->swap(unified_actions); //fast processing function to move elements from the buffer

		return true;
	}

	//append a new action at the end of the buffer
	bool ActionsBuffer::add_action(BaseActionData* in_action)
	{
		unified_actions.push_back(in_action);

		return true;
	}

	//append an array of actions to the bufer
	bool ActionsBuffer::add_actions(std::deque<BaseActionData*>& in_buffer)
	{
		unified_actions.insert(unified_actions.end(), in_buffer.begin(), in_buffer.end());

		return true;
	}

	void ActionsBuffer::dispose()
	{
		dispose_required = true; //set dispose to background thread

		//clear all buffers via lockers
		unified_actions.clear();
	}
	
}