#pragma once

#include "..\..\include\actions\BaseActionData.h"
#include <deque>
#include <mutex>
#include <chrono>
#include <map>

namespace GetGudSdk
{

	class ActionsBuffer
	{
	private:
		std::deque<BaseActionData*> unified_actions;

		bool dispose_required = false;

	public:
		bool get_first_action(BaseActionData* out_action);
		bool get_actions(std::deque<BaseActionData*>* out_buffer);
		bool add_action(BaseActionData* in_action);
		bool add_actions(std::deque<BaseActionData*>& in_buffer);
		void dispose();
	};
}