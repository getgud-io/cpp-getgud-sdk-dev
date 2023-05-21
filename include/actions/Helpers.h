#pragma once

#include <string>

namespace GetGudSdk {

/**
 * LogLevel:
 *
 * What information will logger log?
 **/
enum class LogLevel : unsigned int { FATAL = 0, _ERROR, WARN_AND_ERROR, FULL };

/**
 * LogType:
 *
 * Type of each log message is one of the following
 **/
enum class LogType : unsigned int { DEBUG = 0, WARN, _ERROR, FATAL };

/**
 * Actions:
 *
 * Our primal 6, action types you can send to Getgud
 **/
enum class Actions : unsigned int {
  None = 0,
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
 * Defines the <x, y, z> position vector you send to us with Position and Spawn
 * actions
 **/
struct PositionF {
  float X = 0;
  float Y = 0;
  float Z = 0;
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
 *
 * Report data that you send to Getgud in match our outside match
 **/
struct ReportInfo {
  std::string MatchGuid;
  std::string ReporterName;
  int ReporterType = 0;
  int ReporterSubType = 0;
  std::string SuspectedPlayerGuid;
  int TbType = 0;
  int TbSubType = 0;
  long long TbTimeEpoch = 0;
  int SuggestedToxicityScore = 0;
  long long ReportedTimeEpoch = 0;
};

/**
 * PlayerInfo:
 *
 * Player data you send to Getgud on UpdatePlayer
 **/
struct PlayerInfo {
  std::string PlayerGuid;
  std::string PlayerNickname;
  std::string PlayerEmail;
  int PlayerRank = 0;
  long long PlayerJoinDateEpoch = 0;
};

/**
 * ChatMessageInfo:
 *
 * Chat message data you sent to Getgud from match
 **/
struct ChatMessageInfo {
  long long messageTimeEpoch = 0;
  std::string playerGuid;
  std::string message;
};

/**
 * BaseData:
 *
 **/
struct BaseData {
  Actions ActionType = Actions::None;
  long long ActionTimeEpoch = 0;
  std::string PlayerGuid;
  std::string MatchGuid;
};
}  // namespace GetGudSdk