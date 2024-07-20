#include "ActionsBuffer.h"
#include <deque>
#include <mutex>
#include "BaseActionData.h"
#include "PositionActionData.h"
#include "../config/Config.h"
#include "../logger/Logger.h"

namespace GetgudSDK {
	extern Config sdkConfig;
	extern Logger logger;

	ActionsBuffer actionsBuffer;

	/**
	 * PopActions:
	 *
	 * Pop all actions we have added to action buffer, this is done through
	 * one of the GameSender threads
	 **/
	std::deque<BaseActionData*> ActionsBuffer::PopActions() {

		std::deque<BaseActionData*> outputActionsBuffer; // used for faster copy with swap
		int actionsBufferSize = 0;

		m_actionsBufferLocker.lock();

		m_actionsBuffer.shrink_to_fit();
		actionsBufferSize = m_actionsBuffer.size();

		if (actionsBufferSize != 0) {
			// fast processing function to move elements from the buffer
			m_actionsBuffer.swap(outputActionsBuffer);
		}

		// We recalculate this on every pop to make hypermode more efficient
		m_averageSize.UpdateSize(m_actionsBufferSize);
		m_actionsBufferSize = 0;

		m_actionsBufferLocker.unlock();

		if (actionsBufferSize != 0) logger.Log(LogType::DEBUG, "Popped " + std::to_string(outputActionsBuffer.size()) + " action(s) from ActionBuffer");

		return outputActionsBuffer;
	}

	/**
	 * AddActions:
	 * @actions: A deque of actions to append
	 *
	 * Appends the new actions to the end of the action buffer
	 **/
	bool ActionsBuffer::AddActions(std::deque<BaseActionData*>& actions) {
		std::deque<BaseActionData*> actionToSend;
		unsigned int actionSize = GetPositionActionSize();
		if (m_actionsBufferSize >= sdkConfig.actionsBufferMaxSizeInBytes) {
			// game sender will grab those actions and will delete the game because
			// those actions are empty and marked 
			for (auto& action : actions) {
				auto* emptyAction = new BaseActionData(BaseData(), true);
				emptyAction->m_matchGuid = action->m_matchGuid;
				actionToSend.push_back(emptyAction);
			}
			actionSize = GetEmptyActionSize();
		}
		else {
			// if the action buffer is not full yet we just push regular actions
			for (auto& action : actions)
				actionToSend.push_back(action->Clone());
		}
		m_actionsBufferLocker.lock();

		// return size that calculated in the code
		// 99% of actions are position data, so we assume all actions are position
		// data, BUT in case it is empty actions we use empty action size
		m_actionsBufferSize += actionSize * actionToSend.size();

		m_actionsBuffer.insert(m_actionsBuffer.end(), actionToSend.begin(),
			actionToSend.end());
		m_actionsBuffer.shrink_to_fit();
		m_actionsBufferLocker.unlock();

		return true;
	}

	/**
	 * GetSizeInBytes:
	 *
	 **/
	unsigned int ActionsBuffer::GetSizeInBytes() {
		return m_actionsBufferSize;
	}

	/**
	 * GetAverageSizeInBytes:
	 *
	 * We calculate action buffer avg size to be able to control hyper mode better
	 **/
	unsigned int ActionsBuffer::GetAverageSizeInBytes() {
		return m_averageSize.filledAverageSize;
	}

	/**
	 * Dispose:
	 *
	 **/
	void ActionsBuffer::Dispose() {
		m_actionsBufferLocker.lock();

		// Iterate through all the actions in the buffer and delete them
		for (auto* action : m_actionsBuffer) {
			delete action;
		}

		m_actionsBuffer.clear();
		m_actionsBufferSize = 0;

		m_actionsBufferLocker.unlock();
	}

}  // namespace GetgudSDK