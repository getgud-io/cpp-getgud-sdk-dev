#include "ActionsBuffer.h"
#include <deque>
#include <mutex>
#include "../../include/actions/BaseActionData.h"
#include "../../include/actions/PositionActionData.h"
#include "../config/Config.h"
#include "../logger/Logger.h"

namespace GetGudSdk {
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
  // TODO why do we need outputActionsBuffer? update PopActions (no need
  // outputActionsBuffer)
  std::deque<BaseActionData*> outputActionsBuffer; // used for faster copy with swap
  actionsBufferLocker.lock();

  if (actionsBuffer.size() != 0) {
    // fast processing function to move elements from the buffer
    outputActionsBuffer.swap(actionsBuffer);
    logger.Log(LogType::DEBUG, "Popped " +
                                   std::to_string(outputActionsBuffer.size()) +
                                   " action(s) from ActionBuffer");
  }
  // We recalculate this on every pop to make hypermode more efficient
  averageSize.UpdateSize(actionsBufferSize);

  actionsBufferSize = 0;
  actionsBufferLocker.unlock();

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
  //TODO: size can cross the limit because of unlocked mutex
  if (actionsBufferSize >= sdkConfig.actionsBufferMaxSizeInBytes) {
    // TODO: this shluldn't be called every time but just once.
    logger.Log(LogType::WARN,
               std::string("ActionsBuffer::AddActions->Actions buffer memory "
                           "limit reached, cannot add more actions."));
    // game sender will grab those actions and will delete the game because
    // those actions are empty and marked 
    for (auto& action : actions) {
      auto* emptyAction = new BaseActionData(BaseData(), true);
      emptyAction->matchGuid = action->matchGuid;
      actionToSend.push_back(emptyAction);
    }
    actionSize = GetEmptyActionSize();
  } else {
    // if the action buffer is not full yet we just push regular actions
    for (auto& action : actions)
      actionToSend.push_back(action->Clone());
  }

  actionsBufferLocker.lock();

  // return size that calculated in the code
  // 99% of actions are position data, so we assume all actions are position
  // data, BUT in case it is empty actions we use empty action size
  actionsBufferSize += actionSize * actionToSend.size(); 

  actionsBuffer.insert(actionsBuffer.end(), actionToSend.begin(),
                       actionToSend.end());
  actionsBufferLocker.unlock();

  return true;
}

/**
 * GetSizeInBytes:
 *
 **/
unsigned int ActionsBuffer::GetSizeInBytes() {
  return actionsBufferSize;
}

/**
 * GetAverageSizeInBytes:
 *
 * We calculate action buffer avg size to be able to control hyper mode better
 **/
unsigned int ActionsBuffer::GetAverageSizeInBytes() {
  return averageSize.filledAverageSize;
}

/**
 * Dispose:
 *
 **/
void ActionsBuffer::Dispose() {
  actionsBufferLocker.lock();

  // Iterate through all the actions in the buffer and delete them
  for (auto* action : actionsBuffer) {
    delete action;
  }

  actionsBuffer.clear();
  actionsBufferSize = 0;

  actionsBufferLocker.unlock();
}

}  // namespace GetGudSdk