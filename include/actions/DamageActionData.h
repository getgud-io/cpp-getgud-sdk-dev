#pragma once
#include "BaseActionData.h"

namespace GetGudSdk {
  /**
   * Define Damage Action which is the part of Getgud action stream
   **/
  class DamageActionData: public BaseActionData {
  private:
    float damage_done = 0;
    std::string victim_player_guid;
    std::string weapon_guid;

    /**
    * calculate_size:
    *
    * Calculate size of the action string
    **/
    void calculate_size() override;

  public:
    /**
     * DamageActionData:
     * @_player_guid: guid of the player who damaged the _victim_player_guid
     * player if the attacker is an environment _attacker_player_guid can be null.
     * TODO: rename to _attacker_player_guid
     * @_match_guid: guid of the match where the action took place
     * @_action_time_epoch: timestamp of the action in the Epoch time in
     * milliseconds. Example: 1643717696000
     * @_damage_done: the amount of damage done from attacker to victim
     * @_victim_player_guid: guid of the player who was damaged by
     * _attacker_player_guid
     * @_weapon_guid: guid of weapon that performed damage from attacker to victim
     *
     * Constructor of  Damage Action, this action is later
     * passed to the action buffer
     **/
    DamageActionData(std::string _player_guid,
                     std::string _match_guid,
                     long _action_time_epoch,
                     float _damage_done,
                     std::string _victim_player_guid,
                     std::string _weapon_guid);

    /**
     * DamageActionData:
     * @_data_info: Instance of the base data class which contains all the
     * required fields
     * @_damage_done: the amount of damage done from attacker to victim
     * @_victim_player_guid: guid of the player who was damaged by
     * _attacker_player_guid
     * @_weapon_guid: guid of weapon that performed damage from attacker to victim
     *
     * Constructor of  Damage Action, this action is later
     * passed to the action buffer
     **/
    DamageActionData(BaseData _data_info,
                     float _damage_done,
                     std::string _victim_player_guid,
                     std::string _weapon_guid);
    DamageActionData(DamageActionData& copy);
    DamageActionData() = delete;
    ~DamageActionData() override;

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
    DamageActionData* clone() override;
  };
}  // namespace GetGudSdk