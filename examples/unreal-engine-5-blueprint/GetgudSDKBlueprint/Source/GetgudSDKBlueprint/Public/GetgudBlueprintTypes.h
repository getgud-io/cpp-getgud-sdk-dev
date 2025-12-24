#pragma once

#include "CoreMinimal.h"
#include "GetgudBlueprintTypes.generated.h"

/**
 * Toxic behavior types matching SDK TbType enum
 */
UENUM(BlueprintType)
enum class EGetgudToxicBehavior : uint8
{
	None = 0,  // Placeholder required by UE5 (SDK TbType starts at 1)
	Other = 1,
	Aimbot,
	Wallhack,
	Laghack,
	Godmode,
	Speedhack,
	Spinbot,
	Wallpass,
	Ammohack,
	Healthhack,
	Spawnhack,
	FriendlyFire,
	Spawnkill,
	Camping,
	Insulting,
	Boosting,
	Feeding,
	Smurfing,
	Ragequit,
	Afk,
	Teaming,
	RapidFire,
	AffectAbuse,
	AffectHacking
};

/**
 * Reporter types matching SDK ReporterType enum
 */
UENUM(BlueprintType)
enum class EGetgudReporterType : uint8
{
	None = 0,
	Other = 1,
	Player,
	Moderator,
	Client,
	AntiCheat,
	Custom
};

/**
 * Reporter subtypes matching SDK ReporterSubtype enum
 */
UENUM(BlueprintType)
enum class EGetgudReporterSubtype : uint8
{
	None = 0,
	Other = 1,
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
	FairFight,
	AbilityAbuse,
	Griefing,
	Slurring,
	VoipAbuse,
	ChatAbuse,
	AFK
};

/**
 * Affect states matching SDK AffectState enum
 */
UENUM(BlueprintType)
enum class EGetgudAffectState : uint8
{
	Attach,
	Activate,
	Deactivate,
	Detach
};

/**
 * Position struct matching SDK PositionF
 */
USTRUCT(BlueprintType)
struct FGetgudPosition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	float X = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	float Y = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	float Z = 0.f;
};

/**
 * Rotation struct matching SDK RotationF
 */
USTRUCT(BlueprintType)
struct FGetgudRotation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	float Yaw = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	float Pitch = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	float Roll = -1000.f; // Optional, SDK default
};

/**
 * Player transaction struct matching SDK PlayerTransactions
 */
USTRUCT(BlueprintType)
struct FGetgudPlayerTransaction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	FString TransactionGuid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	FString TransactionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	int64 TransactionDateEpoch = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	float TransactionValueUSD = -1.f;
};

/**
 * Report info struct matching SDK ReportInfo
 */
USTRUCT(BlueprintType)
struct FGetgudReportInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	FString MatchGuid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	FString ReporterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	EGetgudReporterType ReporterType = EGetgudReporterType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	EGetgudReporterSubtype ReporterSubType = EGetgudReporterSubtype::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	FString SuspectedPlayerGuid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	EGetgudToxicBehavior TbType = EGetgudToxicBehavior::Other;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	int64 TbTimeEpoch = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	int32 SuggestedToxicityScore = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	int64 ReportedTimeEpoch = -1;
};

/**
 * Player info struct matching SDK PlayerInfo
 */
USTRUCT(BlueprintType)
struct FGetgudPlayerInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	FString PlayerGuid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	FString PlayerNickname;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	FString PlayerEmail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	int32 PlayerRank = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	int64 PlayerJoinDateEpoch = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	FString PlayerSuspectScore;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	FString PlayerReputation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	FString PlayerStatus;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	FString PlayerCampaign;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	FString PlayerNotes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	FString PlayerDevice;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	FString PlayerOS;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	int32 PlayerAge = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	FString PlayerGender;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	FString PlayerLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	TArray<FGetgudPlayerTransaction> Transactions;
};

/**
 * Chat message struct matching SDK ChatMessageInfo
 */
USTRUCT(BlueprintType)
struct FGetgudChatMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	FString MatchGuid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	int64 MessageTimeEpoch = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	FString PlayerGuid;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GetGud")
	FString Message;
};
