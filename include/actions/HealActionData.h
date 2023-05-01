#pragma once
#include "BaseActionData.h"

namespace GetGudSdk {
  /**
   * Define Heal Action which is the part of Getgud action stream
   **/
  class HealActionData: public BaseActionData {
  private:
    float health_gained = 0;

    /**
     * calculate_size:
     *
     * Update size of Death Action, calls from the cunstructors
     **/
    void calculate_size() override;

  public:
    /**
     * HealActionData:
     * @_player_guid: guid of the player who performed heal action
     * @_match_guid: guid of the match where the action took place
     * @_action_time_epoch: timestamp of the action in the Epoch time in
     * milliseconds. Example: 1643717696000
     * @_health_gained: amount of health gained during healing action
     *
     * Constructor of  Heal Action, this action is later
     * passed to the action buffer
     **/
    HealActionData(std::string _player_guid,
                   std::string _match_guid,
                   long _action_time_epoch,
                   float _health_gained);

    /**
     * HealActionData:
     * @_data_info: Instance of the base data class which contains all the
     * required fields
     * @_health_gained: amount of health gained during healing action
     *
     * Constructor of  Heal Action, this action is later
     * passed to the action buffer
     **/
    HealActionData(BaseData _data_info, float _health_gained);
    HealActionData(HealActionData& copy);
    HealActionData() = delete;
    ~HealActionData() override;

    /**
    * get_data:
    *
    * Fills action metadata struct from config and from fields set in action constructor
    **/
    std::map<std::string, std::string> get_data() override;

    /**
    * get_action_stream:
    *
    * Transforms action metadata from map to string, so this data can be used when sending packets 
    * to the server
    **/
    std::string get_action_stream() override;

    /**
    * clone:
    *
    **/
    HealActionData* clone() override;
  };
}  // namespace GetGudSdk