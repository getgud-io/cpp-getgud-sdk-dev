#pragma once
#include "BaseActionData.h"

namespace GetGudSdk {
  /**
   * Define Attack Action which is the part of Getgud action stream
   **/
  class AttackActionData: public BaseActionData {
  private:
    std::string weapon_guid;

    /**
    * calculate_size:
    *
    * Calculate size of the action string
    **/
    void calculate_size() override;

  public:
    /**
     * AttackActionData:
     * @_player_guid: guid of the player who performed attack action
     * @_match_guid: guid of the match where the action took place
     * @_action_time_epoch: timestamp of the action in the Epoch time in
     * milliseconds. Example: 1643717696000
     * @_weapon_guid: guid of the weapon used during the action
     *
     * Constructor of  Attack Action, this action is later
     * passed to the action buffer
     **/
    AttackActionData(std::string _player_guid,
                     std::string _match_guid,
                     long _action_time_epoch,
                     std::string _weapon_guid);

    /**
     * AttackActionData:
     * @_data_info: Instance of the base data class which contains all the
     * required fields
     * @_weapon_guid: weapon_guid of the weapon used during the action
     *
     * Constructor of  Attack Action, this action is later
     * passed to the action buffer
     **/
    AttackActionData(BaseData _data_info, std::string _weapon_guid);
    AttackActionData() = delete;
    AttackActionData(AttackActionData& copy);
    ~AttackActionData() override;

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
    AttackActionData* clone() override;
  };
}  // namespace GetGudSdk