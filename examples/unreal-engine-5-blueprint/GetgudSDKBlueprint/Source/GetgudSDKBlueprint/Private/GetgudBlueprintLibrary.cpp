#include "GetgudBlueprintLibrary.h"
#include "GetgudSDK.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include <cstdlib>

// Helper to convert FGetgudPosition to SDK PositionF
static GetgudSDK::PositionF ToSDKPosition(const FGetgudPosition& Pos)
{
	GetgudSDK::PositionF Result;
	Result.X = Pos.X;
	Result.Y = Pos.Y;
	Result.Z = Pos.Z;
	return Result;
}

// Helper to convert FGetgudRotation to SDK RotationF
static GetgudSDK::RotationF ToSDKRotation(const FGetgudRotation& Rot)
{
	GetgudSDK::RotationF Result;
	Result.Yaw = Rot.Yaw;
	Result.Pitch = Rot.Pitch;
	Result.Roll = Rot.Roll;
	return Result;
}

// Helper to convert EGetgudAffectState to SDK AffectState
static GetgudSDK::AffectState ToSDKAffectState(EGetgudAffectState State)
{
	switch (State)
	{
	case EGetgudAffectState::Attach: return GetgudSDK::AffectState::Attach;
	case EGetgudAffectState::Activate: return GetgudSDK::AffectState::Activate;
	case EGetgudAffectState::Deactivate: return GetgudSDK::AffectState::Deactivate;
	case EGetgudAffectState::Detach: return GetgudSDK::AffectState::Detach;
	default: return GetgudSDK::AffectState::Attach;
	}
}

// Helper to convert EGetgudToxicBehavior to SDK TbType
static GetgudSDK::TbType ToSDKTbType(EGetgudToxicBehavior Type)
{
	return static_cast<GetgudSDK::TbType>(static_cast<int>(Type));
}

// Helper to convert EGetgudReporterType to SDK ReporterType
// Note: SDK uses None=-1, but UE uint8 enum can't have -1, so we handle explicitly
static GetgudSDK::ReporterType ToSDKReporterType(EGetgudReporterType Type)
{
	if (Type == EGetgudReporterType::None)
	{
		return GetgudSDK::ReporterType::None; // SDK -1
	}
	// Other values start at 1 in both
	return static_cast<GetgudSDK::ReporterType>(static_cast<int>(Type));
}

// Helper to convert EGetgudReporterSubtype to SDK ReporterSubtype
// Note: SDK uses None=-1, but UE uint8 enum can't have -1, so we handle explicitly
static GetgudSDK::ReporterSubtype ToSDKReporterSubtype(EGetgudReporterSubtype Type)
{
	if (Type == EGetgudReporterSubtype::None)
	{
		return GetgudSDK::ReporterSubtype::None; // SDK -1
	}
	// Other values start at 1 in both
	return static_cast<GetgudSDK::ReporterSubtype>(static_cast<int>(Type));
}

// Helper to convert FGetgudReportInfo to SDK ReportInfo
static GetgudSDK::ReportInfo ToSDKReportInfo(const FGetgudReportInfo& Info)
{
	GetgudSDK::ReportInfo Result;
	Result.MatchGuid = TCHAR_TO_UTF8(*Info.MatchGuid);
	Result.ReporterName = TCHAR_TO_UTF8(*Info.ReporterName);
	Result.ReporterType = ToSDKReporterType(Info.ReporterType);
	Result.ReporterSubType = ToSDKReporterSubtype(Info.ReporterSubType);
	Result.SuspectedPlayerGuid = TCHAR_TO_UTF8(*Info.SuspectedPlayerGuid);
	Result.TbType = ToSDKTbType(Info.TbType);
	Result.TbTimeEpoch = Info.TbTimeEpoch;
	Result.SuggestedToxicityScore = Info.SuggestedToxicityScore;
	Result.ReportedTimeEpoch = Info.ReportedTimeEpoch;
	return Result;
}

// Helper to convert FGetgudPlayerTransaction to SDK PlayerTransactions
static GetgudSDK::PlayerTransactions ToSDKTransaction(const FGetgudPlayerTransaction& Trans)
{
	GetgudSDK::PlayerTransactions Result;
	Result.TransactionGuid = TCHAR_TO_UTF8(*Trans.TransactionGuid);
	Result.TransactionName = TCHAR_TO_UTF8(*Trans.TransactionName);
	Result.TransactionDateEpoch = Trans.TransactionDateEpoch;
	Result.TransactionValueUSD = Trans.TransactionValueUSD;
	return Result;
}

// Helper to convert FGetgudPlayerInfo to SDK PlayerInfo
static GetgudSDK::PlayerInfo ToSDKPlayerInfo(const FGetgudPlayerInfo& Info)
{
	GetgudSDK::PlayerInfo Result;
	Result.PlayerGuid = TCHAR_TO_UTF8(*Info.PlayerGuid);
	Result.PlayerNickname = TCHAR_TO_UTF8(*Info.PlayerNickname);
	Result.PlayerEmail = TCHAR_TO_UTF8(*Info.PlayerEmail);
	Result.PlayerRank = Info.PlayerRank;
	Result.PlayerJoinDateEpoch = Info.PlayerJoinDateEpoch;
	Result.PlayerSuspectScore = TCHAR_TO_UTF8(*Info.PlayerSuspectScore);
	Result.PlayerReputation = TCHAR_TO_UTF8(*Info.PlayerReputation);
	Result.PlayerStatus = TCHAR_TO_UTF8(*Info.PlayerStatus);
	Result.PlayerCampaign = TCHAR_TO_UTF8(*Info.PlayerCampaign);
	Result.PlayerNotes = TCHAR_TO_UTF8(*Info.PlayerNotes);
	Result.PlayerDevice = TCHAR_TO_UTF8(*Info.PlayerDevice);
	Result.PlayerOS = TCHAR_TO_UTF8(*Info.PlayerOS);
	Result.PlayerAge = Info.PlayerAge;
	Result.PlayerGender = TCHAR_TO_UTF8(*Info.PlayerGender);
	Result.PlayerLocation = TCHAR_TO_UTF8(*Info.PlayerLocation);

	for (const FGetgudPlayerTransaction& Trans : Info.Transactions)
	{
		Result.Transactions.push_back(ToSDKTransaction(Trans));
	}

	return Result;
}

// Helper to convert FGetgudChatMessage to SDK ChatMessageInfo
static GetgudSDK::ChatMessageInfo ToSDKChatMessage(const FGetgudChatMessage& Msg)
{
	GetgudSDK::ChatMessageInfo Result;
	Result.messageTimeEpoch = Msg.MessageTimeEpoch;
	Result.playerGuid = TCHAR_TO_UTF8(*Msg.PlayerGuid);
	Result.message = TCHAR_TO_UTF8(*Msg.Message);
	return Result;
}

// ============================================
// Initialization
// ============================================

bool UGetgudBlueprintLibrary::Init()
{
	// Check if GETGUD_CONFIG_PATH env var is set
	char* EnvPath = std::getenv("GETGUD_CONFIG_PATH");
	if (EnvPath && strlen(EnvPath) > 0)
	{
		// SDK will use env var
		return GetgudSDK::Init();
	}

	// Find config.json next to executable
	FString ExeDir = FPaths::GetPath(FPlatformProcess::ExecutablePath());
	FString ConfigPath = FPaths::Combine(ExeDir, TEXT("config.json"));

	return GetgudSDK::Init(TCHAR_TO_UTF8(*ConfigPath));
}

bool UGetgudBlueprintLibrary::InitWithPath(const FString& ConfigFileFullPath)
{
	return GetgudSDK::Init(TCHAR_TO_UTF8(*ConfigFileFullPath));
}

bool UGetgudBlueprintLibrary::InitWithContent(const FString& ConfigFile, bool PassAsContent)
{
	return GetgudSDK::Init(TCHAR_TO_UTF8(*ConfigFile), PassAsContent);
}

void UGetgudBlueprintLibrary::Dispose()
{
	GetgudSDK::Dispose();
}

// ============================================
// Game/Match Management
// ============================================

FString UGetgudBlueprintLibrary::StartGame(int32 TitleId, const FString& PrivateKey, const FString& ServerGuid, const FString& GameMode, const FString& ServerLocation)
{
	std::string Result = GetgudSDK::StartGame(
		TitleId,
		TCHAR_TO_UTF8(*PrivateKey),
		TCHAR_TO_UTF8(*ServerGuid),
		TCHAR_TO_UTF8(*GameMode),
		TCHAR_TO_UTF8(*ServerLocation)
	);
	return UTF8_TO_TCHAR(Result.c_str());
}

FString UGetgudBlueprintLibrary::StartGameWithConfig(const FString& ServerGuid, const FString& GameMode, const FString& ServerLocation)
{
	std::string Result = GetgudSDK::StartGame(
		TCHAR_TO_UTF8(*ServerGuid),
		TCHAR_TO_UTF8(*GameMode),
		TCHAR_TO_UTF8(*ServerLocation)
	);
	return UTF8_TO_TCHAR(Result.c_str());
}

FString UGetgudBlueprintLibrary::StartMatch(const FString& GameGuid, const FString& MatchMode, const FString& MapName)
{
	std::string Result = GetgudSDK::StartMatch(
		TCHAR_TO_UTF8(*GameGuid),
		TCHAR_TO_UTF8(*MatchMode),
		TCHAR_TO_UTF8(*MapName)
	);
	return UTF8_TO_TCHAR(Result.c_str());
}

FString UGetgudBlueprintLibrary::StartMatchWithCustomField(const FString& GameGuid, const FString& MatchMode, const FString& MapName, const FString& CustomField)
{
	std::string Result = GetgudSDK::StartMatch(
		TCHAR_TO_UTF8(*GameGuid),
		TCHAR_TO_UTF8(*MatchMode),
		TCHAR_TO_UTF8(*MapName),
		TCHAR_TO_UTF8(*CustomField)
	);
	return UTF8_TO_TCHAR(Result.c_str());
}

bool UGetgudBlueprintLibrary::MarkEndGame(const FString& GameGuid)
{
	return GetgudSDK::MarkEndGame(TCHAR_TO_UTF8(*GameGuid));
}

bool UGetgudBlueprintLibrary::SetMatchWinTeam(const FString& MatchGuid, const FString& TeamGuid)
{
	return GetgudSDK::SetMatchWinTeam(TCHAR_TO_UTF8(*MatchGuid), TCHAR_TO_UTF8(*TeamGuid));
}

// ============================================
// Actions
// ============================================

bool UGetgudBlueprintLibrary::SendSpawnAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, const FString& CharacterGuid, const FString& TeamGuid, float InitialHealth, const FGetgudPosition& Position, const FGetgudRotation& Rotation)
{
	return GetgudSDK::SendSpawnAction(
		TCHAR_TO_UTF8(*MatchGuid),
		ActionTimeEpoch,
		TCHAR_TO_UTF8(*PlayerGuid),
		TCHAR_TO_UTF8(*CharacterGuid),
		TCHAR_TO_UTF8(*TeamGuid),
		InitialHealth,
		ToSDKPosition(Position),
		ToSDKRotation(Rotation)
	);
}

bool UGetgudBlueprintLibrary::SendPositionAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, const FGetgudPosition& Position, const FGetgudRotation& Rotation)
{
	return GetgudSDK::SendPositionAction(
		TCHAR_TO_UTF8(*MatchGuid),
		ActionTimeEpoch,
		TCHAR_TO_UTF8(*PlayerGuid),
		ToSDKPosition(Position),
		ToSDKRotation(Rotation)
	);
}

bool UGetgudBlueprintLibrary::SendAttackAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, const FString& WeaponGuid)
{
	return GetgudSDK::SendAttackAction(
		TCHAR_TO_UTF8(*MatchGuid),
		ActionTimeEpoch,
		TCHAR_TO_UTF8(*PlayerGuid),
		TCHAR_TO_UTF8(*WeaponGuid)
	);
}

bool UGetgudBlueprintLibrary::SendDamageAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, const FString& VictimPlayerGuid, float DamageDone, const FString& WeaponGuid)
{
	return GetgudSDK::SendDamageAction(
		TCHAR_TO_UTF8(*MatchGuid),
		ActionTimeEpoch,
		TCHAR_TO_UTF8(*PlayerGuid),
		TCHAR_TO_UTF8(*VictimPlayerGuid),
		DamageDone,
		TCHAR_TO_UTF8(*WeaponGuid)
	);
}

bool UGetgudBlueprintLibrary::SendDeathAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, const FString& AttackerGuid)
{
	return GetgudSDK::SendDeathAction(
		TCHAR_TO_UTF8(*MatchGuid),
		ActionTimeEpoch,
		TCHAR_TO_UTF8(*PlayerGuid),
		TCHAR_TO_UTF8(*AttackerGuid)
	);
}

bool UGetgudBlueprintLibrary::SendHealAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, float HealthGained)
{
	return GetgudSDK::SendHealAction(
		TCHAR_TO_UTF8(*MatchGuid),
		ActionTimeEpoch,
		TCHAR_TO_UTF8(*PlayerGuid),
		HealthGained
	);
}

bool UGetgudBlueprintLibrary::SendAffectAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& AffectGuid, const FString& PlayerGuid, EGetgudAffectState AffectState)
{
	return GetgudSDK::SendAffectAction(
		TCHAR_TO_UTF8(*MatchGuid),
		ActionTimeEpoch,
		TCHAR_TO_UTF8(*AffectGuid),
		TCHAR_TO_UTF8(*PlayerGuid),
		ToSDKAffectState(AffectState)
	);
}

// ============================================
// Reports/Chat/Players
// ============================================

bool UGetgudBlueprintLibrary::SendInMatchReport(const FGetgudReportInfo& ReportInfo)
{
	GetgudSDK::ReportInfo SDKReport = ToSDKReportInfo(ReportInfo);
	return GetgudSDK::SendInMatchReport(SDKReport);
}

bool UGetgudBlueprintLibrary::SendChatMessage(const FString& MatchGuid, const FGetgudChatMessage& MessageInfo)
{
	GetgudSDK::ChatMessageInfo SDKMessage = ToSDKChatMessage(MessageInfo);
	return GetgudSDK::SendChatMessage(TCHAR_TO_UTF8(*MatchGuid), SDKMessage);
}

bool UGetgudBlueprintLibrary::SendReports(int32 TitleId, const FString& PrivateKey, const TArray<FGetgudReportInfo>& Reports)
{
	std::deque<GetgudSDK::ReportInfo> SDKReports;
	for (const FGetgudReportInfo& Report : Reports)
	{
		SDKReports.push_back(ToSDKReportInfo(Report));
	}
	return GetgudSDK::SendReports(TitleId, TCHAR_TO_UTF8(*PrivateKey), SDKReports);
}

bool UGetgudBlueprintLibrary::SendReportsWithConfig(const TArray<FGetgudReportInfo>& Reports)
{
	std::deque<GetgudSDK::ReportInfo> SDKReports;
	for (const FGetgudReportInfo& Report : Reports)
	{
		SDKReports.push_back(ToSDKReportInfo(Report));
	}
	return GetgudSDK::SendReports(SDKReports);
}

bool UGetgudBlueprintLibrary::UpdatePlayers(int32 TitleId, const FString& PrivateKey, const TArray<FGetgudPlayerInfo>& Players)
{
	std::deque<GetgudSDK::PlayerInfo> SDKPlayers;
	for (const FGetgudPlayerInfo& Player : Players)
	{
		SDKPlayers.push_back(ToSDKPlayerInfo(Player));
	}
	return GetgudSDK::UpdatePlayers(TitleId, TCHAR_TO_UTF8(*PrivateKey), SDKPlayers);
}

bool UGetgudBlueprintLibrary::UpdatePlayersWithConfig(const TArray<FGetgudPlayerInfo>& Players)
{
	std::deque<GetgudSDK::PlayerInfo> SDKPlayers;
	for (const FGetgudPlayerInfo& Player : Players)
	{
		SDKPlayers.push_back(ToSDKPlayerInfo(Player));
	}
	return GetgudSDK::UpdatePlayers(SDKPlayers);
}
