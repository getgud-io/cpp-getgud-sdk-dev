#include "ActionsBuffer.h"

#include "../../include/config/Config.h"

namespace GetGudSdk {
extern Config sdk_config;

// create a new shared actions buffer
ActionsBuffer actions_buffer;

/**
* get_first_action:
* @out_action: Object to write first buffer action in
*
* Takes first action from the buffer and writes it into out_action data
**/
bool ActionsBuffer::get_first_action(BaseActionData* out_action) {
  actions_locker.lock();
  out_action = *unified_actions.begin(); 
  unified_actions.erase(unified_actions.begin());
  actions_buffer_size = actions_buffer_size - out_action->get_data_size(); 
  actions_locker.unlock();

  return true;
}

/**
* get_actions:
* @out_buffer: Object to write all buffer actions in
*
* Copy all actions from action buffer to the out_buffer structure
**/
unsigned int ActionsBuffer::get_actions(std::deque<BaseActionData*>* out_buffer) {
  int actions_size = 0;
  actions_locker.lock();
  if (unified_actions.size() != 0)  
  {
    // make or clean a new buffer in order to swap it with the buffer
    if (out_buffer == nullptr)
      out_buffer = new std::deque<BaseActionData*>();
    else if (out_buffer->size())
      out_buffer->clear();

    // fast processing function to move elements from the buffer
    out_buffer->swap(unified_actions); 
    actions_size = actions_buffer_size;
    actions_buffer_size = 0;
  }
  actions_locker.unlock();

  return actions_size;
}

/**
* add_action:
* @in_action: A new action to append
*
* Appends the new action to the end of the buffer
**/
bool ActionsBuffer::add_action(BaseActionData* in_action) {
  actions_locker.lock();
  actions_buffer_size = actions_buffer_size + in_action->get_data_size();
  unified_actions.push_back(in_action);
  actions_locker.unlock();

  return true;
}

/**
* add_actions:
* @in_buffer: A deque of actions to append
*
* Appends the new actions to the end of the buffer
**/
bool ActionsBuffer::add_actions(std::deque<BaseActionData*>& in_buffer) {
  actions_locker.lock();

  for (auto& action : in_buffer) {
    actions_buffer_size = actions_buffer_size + action->get_data_size();
  }

  unified_actions.insert(unified_actions.end(), in_buffer.begin(),
                         in_buffer.end());
  actions_locker.unlock();

  return true;
}

/**
* check_buffer_limit:
*
* Check if buffer has memory in it.
**/
bool ActionsBuffer::check_buffer_limit() {
  if (actions_buffer_size > sdk_config.actions_buffer_max_size)
    return true; 
  else
    return false;
}

/**
* check_buffer_load:
*
* Check if buffer has memory in it.
**/
unsigned int ActionsBuffer::check_buffer_load() {
  // TODO: bug actions_buffer_size / sdk_config.actions_buffer_max_size
  int percentage_load =
      sdk_config.actions_buffer_max_size / actions_buffer_size;

  // We should not have any threads because the buffer is full
  if (percentage_load > 100 && percentage_load < 0)
    return 0;
  
  // Determine the total amount of threads we should have at the given point of time
  unsigned int threads_amount =
      percentage_load / sdk_config.hyper_speed_at_buffer_percentage;

  return threads_amount;
}

/**
* get_buffer_size:
*
**/
unsigned int ActionsBuffer::get_buffer_size() {
  return actions_buffer_size;
}

/**
* dispose:
*
* Delete action buffer
**/
void ActionsBuffer::dispose() {
  actions_locker.lock();
  dispose_required = true; 
  unified_actions.clear();
  actions_buffer_size = 0;
  actions_locker.unlock();
}

}  // namespace GetGudSdk