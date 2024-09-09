#pragma once

#include <chrono>
#include <deque>
#include <map>
#include <mutex>
#include "../../include/actions/BaseActionData.h"
#include "../utils/Utils.h"

namespace GetgudSDK {

	class ActionsBuffer {
	private:
		std::mutex m_actionsBufferLocker;
		std::deque<BaseActionData*> m_actionsBuffer;
		unsigned int m_actionsBufferSize = 0;
		FilledAverage m_averageSize;
		unsigned int m_actionSize;
		unsigned int m_emptyActionSize;

	public:
		ActionsBuffer();
		std::deque<BaseActionData*> PopActions();
		bool AddActions(std::deque<BaseActionData*>& actions);
		unsigned int GetSizeInBytes();
		unsigned int GetAverageSizeInBytes();
		void Dispose();
	};
}  // namespace GetgudSDK