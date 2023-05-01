#pragma once

#include "../../include/actions/BaseActionData.h"
#include <deque>
#include <mutex>
#include <chrono>
#include <map>

namespace GetGudSdk
{

	class ActionsBuffer
	{
	private:
		std::mutex actions_locker; // TODO: rename
		std::deque<BaseActionData*> unified_actions; // TODO rename:
		unsigned int actions_buffer_size = 0;
		bool dispose_required = false; // Flag that Action Buffer has to be disposed

	public:
		/**
		* get_first_action:
		* @out_action: Object to write first buffer action in
		*
		* Takes first action from the buffer and writes it into out_action data
		**/
		bool get_first_action(BaseActionData* out_action);

		/**
		* get_actions:
		* @out_buffer: Object to write all buffer actions in
		*
		* Copy all actions from action buffer to the out_buffer structure
		**/
		unsigned int get_actions(std::deque<BaseActionData*>* out_buffer);

		/**
		* add_action:
		* @in_action: A new action to append
		*
		* Appends the new action to the end of the buffer
		**/
		bool add_action(BaseActionData* in_action);

		/**
		* add_actions:
		* @in_buffer: A deque of actions to append
		*
		* Appends the new actions to the end of the buffer
		**/
		bool add_actions(std::deque<BaseActionData*>& in_buffer);

		/**
		* check_buffer_limit:
		*
		* Check if buffer has memory in it.
		**/
		bool check_buffer_limit();

		/**
		* check_buffer_load:
		*
		* Check if buffer has memory in it.
		**/
		unsigned int check_buffer_load();

		/**
		* get_buffer_size:
		*
		**/
		unsigned int get_buffer_size();

		/**
		* dispose:
		*
		* Delete action buffer
		**/
		void dispose();
	};
}