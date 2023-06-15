#pragma once

#include <string>
#include <limits>

namespace GetGudSdk {

  namespace Values {
  const std::string g_Environment = "Environment";
  }

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

enum class TbType : int {
  None = -1,
  Aimbot = 0,
  Wallhack,
  Laghack,
  Godmode,
  Speedhack,
  Spinbot,
  Wallpass,
  Ammohack,
  Healthhack,
  Spawnhack,
  Teamkill,
  Spawnkill,
  Camping,
  Insulting,
  Boosting,
  Feeding,
  Smurfing,
  Ragequit
};

enum class ReporterType : int {
  None = -1,
  Other = 0,
  Player,
  Moderator,
  Client,
  AntiCheat,
  Custom
};

enum class ReporterSubtype : int {
  None = -1,
  Other = 0,
  CommunityManager,
  LiveOps,
  Support,
  QA,
  Custom,
  OtherAntiCheat,
  CustomAntiCheat,
  BattleEye,
  EasyAntiCheat,
  PunkBuster,
  VAC,
  VACNet,
  FairFight
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
  float Yaw = 0;
  float Pitch = 0;
  float Roll = -1000.f; // optional
};

struct Orientation {
  PositionF position = { 0.0f };
  RotationF rotation = { 0.0f };
  Orientation operator-(Orientation& rhs)
  {
    Orientation outOrientation;
    outOrientation.position.X = position.X - rhs.position.X;
    outOrientation.position.Y = position.Y - rhs.position.Y;
    outOrientation.position.Z = position.Z - rhs.position.Z;

    outOrientation.rotation.Yaw = rotation.Yaw - rhs.rotation.Yaw;
    outOrientation.rotation.Pitch = rotation.Pitch - rhs.rotation.Pitch;
    outOrientation.rotation.Roll = rotation.Roll - rhs.rotation.Roll;
    return outOrientation;
  }
};

/**
 * ReportInfo:
 *
 * Report data that you send to Getgud in match our outside match
 **/
struct ReportInfo {
  std::string MatchGuid; //36 + SQL // required
  std::string ReporterName;  // SQL, size <=10.000
  GetGudSdk::ReporterType ReporterType = ReporterType::None; // >= 1, < max
  ReporterSubtype ReporterSubType = ReporterSubtype::None; // >= 1, < max
  std::string SuspectedPlayerGuid;  // 36 + SQL //required
  GetGudSdk::TbType TbType = TbType::None; //>=1, <max
  long long TbTimeEpoch = -1; //>=time.min, <= time.max
  int SuggestedToxicityScore = -1; //>=0, <=100
  long long ReportedTimeEpoch = -1; //>=time.min, <=time.max
};

/**
 * PlayerInfo:
 *
 * Player data you send to Getgud on UpdatePlayer
 **/
struct PlayerInfo {
  std::string PlayerGuid;//36 + SQL //required
  std::string PlayerNickname;  // SQL, size <=10.000
  std::string PlayerEmail;     // SQL, size <=10.000
  int PlayerRank = -1;//>=0, <=max
  long long PlayerJoinDateEpoch = -1;//>=time.min, <=time.max
};

/**
 * ChatMessageInfo:
 *
 * Chat message data you sent to Getgud from match
 **/
struct ChatMessageInfo {
  long long messageTimeEpoch = 0;//>=time.min, <=time.max
  std::string playerGuid;//36 + SQL
  std::string message;//SQL, size <=10.000
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
