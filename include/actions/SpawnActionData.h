#pragma once
#include "BaseActionData.h"

#include <vector>

namespace GetGudSdk {
  /**
   * Define Spawn Action which is the part of Getgud action stream
   **/
  class SpawnActionData: public BaseActionData {
  private:
    PositionF position;
    RotationF rotation;
    float initial_health = 0;
    int team_id = 0;
    std::string character_guid;

    /**
     * calculate_size:
     *
     * Update size of Death Action, calls from the cunstructors
     **/
    void calculate_size() override;

  public:
    /**
     * SpawnActionData:
     * @_player_guid: guid of the player who performed attack action
     * @_match_guid: guid of the match where the action took place
     * @_action_time_epoch: timestamp of the action in the Epoch time in
     * milliseconds. Example: 1643717696000
     * @_position: <x,y,z> position of player at _action_time_epoch time
     * @_rotation: <pitch, roll> of where player looks on the screen at
     * _action_time_epoch time
     * @_initial_health: initial health of the spawned player
     * @_tead_id: team id of the spawned player
     * @_character_guid: character guid of the spawned player
     *
     * Constructor of Spawn Action, this action is later
     * passed to the action buffer
     **/
    SpawnActionData(std::string _player_guid,
                    std::string _match_guid,
                    long _action_time_epoch,
                    PositionF _position,
                    RotationF _rotation,
                    float _initial_health,
                    int _team_id,
                    std::string _character_guid);

    /**
     * SpawnActionData:
     ** @_data_info: Instance of the base data class which contains all the
     * required fields
     * @_position: <x,y,z> position of player at _action_time_epoch time
     * @_rotation: <pitch, roll> of where player looks on the screen at
     * _action_time_epoch time
     * @_initial_health: initial health of the spawned player
     * @_tead_id: team id of the spawned player
     * @_character_guid: character guid of the spawned player
     *
     * Constructor of Spawn Action, this action is later
     * passed to the action buffer
     **/
    SpawnActionData(BaseData _data_info,
                    PositionF _position,
                    RotationF _rotation,
                    float _initial_health,
                    int _team_id,
                    std::string _character_guid);

    /**
    * SpawnActionData:
    * @copy: Spawn action to copy
    *
    * Constructor of  Spawn Action, this action is later
    * passed to the action buffer
    **/
    SpawnActionData(SpawnActionData& copy);
    SpawnActionData() = delete;
    ~SpawnActionData() override;

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
    SpawnActionData* clone() override;
  };
}  // namespace GetGudSdk