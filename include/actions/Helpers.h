#pragma once

#include <string>

/**
 * Helper structures for GetGud sdk
 * TODO: Describe Where is this used? What can I do with this as a developer?
 **/
namespace GetGudSdk {

  /**
   * LogLevel:
   *
   * @NoLogs: Do not log anything
   * @Actions: Log all actions sending
   * @Responses: Log responses from the rest api and packets sending
   * @Processes: Log Responses and packets sending and comparing packets
   * @Full: Log everything
   *
   * Choose what to log in the logger with this enum
   **/
  enum class LogLevel: int {
    NoLogs = 0,                 // logging nothing
    Actions = 1,                // actions sending
    Responses = 2,              // responses from the rest api, packets sending
    Processes = Responses + 1,  // packets comparing and sendind
    Full = Processes +
    Actions  // packets sending, action processing and ret api responses
  };

  /**
   * LogType:
   *
   * @MESSAGE:
   * @WARNING:
   * @_ERROR:
   * TODO: write description for this
   *
   **/
  enum class LogType: unsigned int {
    MESSAGE = 0,
    WARNING,
    _ERROR  // using _ because of standart ERROR define
  };

  /**
   * Actions:
   *
   * Our primal 6 actions you can send to us
   **/
  enum class Actions: unsigned int {
    none = 0,
    Attack,
    Damage,
    Death,
    Heal,
    Position,
    Spawn
  };

  /**
   * PositionF:
   *
   * Defines the <x,y,z> position vector you send to us with Position and Spawn
   *actions
   **/
  struct PositionF {
    float x = 0;
    float y = 0;
    float z = 0;
  };

  /**
   * RotationF:
   *
   * Defines the <pitch, roll> view vector you send to us with Position and Spawn
   * actions
   **/
  struct RotationF {
    float Pitch = 0;
    float Roll = 0;
  };

  /**
   * ReportInfo:
   * @match_guid:
   * @reporter_name:
   * @reporter_type:
   * @reporter_sub_type:
   * @suspected_player_guid:
   * @tb_type:
   * @tb_sub_type:
   * @tb_time_epoch:
   * @suggested_toxicity_score:
   * @reported_time_epoch:
   *
   * TODO: describe what is this and where is this used?
   **/
  struct ReportInfo {
    std::string match_guid;
    std::string reporter_name;
    int reporter_type = 0;
    int reporter_sub_type = 0;
    std::string suspected_player_guid;
    int tb_type = 0;
    int tb_sub_type = 0;
    long tb_time_epoch = 0;
    int suggested_toxicity_score = 0;
    long reported_time_epoch = 0;
  };

    struct PlayerInfo {
    std::string player_guid;
    std::string player_nickname;
    std::string player_email;
    int player_rank;
    long player_join_date_epoch;
  };

  /**
   * BaseData:
   * @action_type:
   * @action_time_epoch:
   * @player_guid:
   * @match_guid:
   *
   * TODO: describe what is this and where is this used?
   **/
  struct BaseData {
    Actions action_type = Actions::none;
    long action_time_epoch = 0;
    std::string player_guid;
    std::string match_guid;
  };
}  // namespace GetGudSdk