#pragma once

#include "BaseActionData.h"

namespace GetGudSdk {
  /**
   * Define Death Action which is the part of Getgud action stream
   **/
  class DeathActionData: public BaseActionData {
  private:
  
    /**
    * calculate_size:
    *
    * Calculate size of the action string
    **/
    void calculate_size() override;

  public:
    /**
     * DeathActionData:
     * @_player_guid: guid of the player who died
     * @_match_guid: guid of the match where player died
     * @_action_time_epoch: timestamp of the action in the Epoch time in
     * milliseconds. Example: 1643717696000
     *
     * Constructor of  Death Action, this action is later
     * passed to the action buffer
     **/
    DeathActionData(std::string _player_guid,
                    std::string _match_guid,
                    long _action_time_epoch);

    /**
     * DeathActionData:
     * @_data_info: Instance of the base data class which contains all the
     * required fields
     *
     * Constructor of  Death Action, this action is later
     * passed to the action buffer
     **/
    DeathActionData(BaseData _data_info);

    DeathActionData(DeathActionData& copy);
    DeathActionData() = delete;
    ~DeathActionData() override;

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
    DeathActionData* clone() override;
  };
}  // namespace GetGudSdk