#pragma once
#include "BaseActionData.h"

namespace GetGudSdk {
  /**
   * Define Position Action which is the part of Getgud action stream
   **/
  class PositionActionData: public BaseActionData {
  private:
    PositionF position;
    RotationF rotation;

    /**
     * calculate_size:
     *
     * Update size of Death Action, calls from the cunstructors
     **/
    void calculate_size() override;

  public:
    /**
     * PositionActionData:
     * @_player_guid: guid of the player who performed attack action
     * @_match_guid: guid of the match where the action took place
     * @_action_time_epoch: timestamp of the action in the Epoch time in
     * milliseconds. Example: 1643717696000
     * @_position: <x,y,z> position of player at _action_time_epoch time
     * @_rotation: <pitch, roll> of where player looks on the screen at
     * _action_time_epoch time
     *
     * Constructor of  Position Action, this action is later
     * passed to the action buffer
     **/
    PositionActionData(std::string _player_guid,
                       std::string _match_guid,
                       long _action_time_epoch,
                       PositionF _position,
                       RotationF _rotation);

    /**
     * PositionActionData:
     * @_data_info: Instance of the base data class which contains all the
     * required fields
     * @_position: <x,y,z> position of player at _action_time_epoch time
     * @_rotation: <pitch, roll> of where player looks on the screen at
     * _action_time_epoch time
     *
     * Constructor of  Position Action, this action is later
     * passed to the action buffer
     **/
    PositionActionData(BaseData _data_info,
                       PositionF _position,
                       RotationF _rotation);
    
    /**
    * PositionActionData:
    * @copy: Position action to copy
    *
    * Constructor of  Position Action, this action is later
    * passed to the action buffer
    **/
    PositionActionData(PositionActionData& copy);
    PositionActionData() = delete;
    ~PositionActionData() override;

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
    PositionActionData* clone() override;
  };
}  // namespace GetGudSdk