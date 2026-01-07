#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GetgudBlueprintTypes.h"
#include "GetgudBlueprintLibrary.generated.h"

/**
 * Blueprint function library exposing GetGud SDK functions
 */
UCLASS()
class GETGUDSDKBLUEPRINT_API UGetgudBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// ============================================
	// Initialization
	// ============================================

	/**
	 * Initialize GetGud SDK with default config (looks for config.json next to executable)
	 */
	UFUNCTION(BlueprintCallable, Category = "Getgud|Init")
	static bool Init();

	/**
	 * Initialize GetGud SDK with config file path
	 */
	UFUNCTION(BlueprintCallable, Category = "Getgud|Init")
	static bool InitConfPath(const FString& ConfigFilePath);

	/**
	 * Initialize GetGud SDK with config content or path
	 * @param ConfigFile - Config file path or content string
	 * @param PassAsContent - true: treat ConfigFile as JSON content, false: treat as file path
	 */
	UFUNCTION(BlueprintCallable, Category = "Getgud|Init")
	static bool InitConf(const FString& ConfigFile, bool PassAsContent);

	/**
	 * Dispose/shutdown GetGud SDK
	 */
	UFUNCTION(BlueprintCallable, Category = "Getgud|Init")
	static void Dispose();

	// ============================================
	// Game/Match Management
	// ============================================

	/**
	 * Start a new game session with credentials
	 * @return GameGuid on success, empty string on failure
	 */
	UFUNCTION(BlueprintCallable, Category = "Getgud|Game")
	static FString StartGame(int32 TitleId, const FString& PrivateKey, const FString& ServerGuid, const FString& GameMode, const FString& ServerLocation);

	/**
	 * Start a new match within a game
	 * @return MatchGuid on success, empty string on failure
	 */
	UFUNCTION(BlueprintCallable, Category = "Getgud|Game")
	static FString StartMatch(const FString& GameGuid, const FString& MatchMode, const FString& MapName, const FString& CustomField);

	/**
	 * Mark a game as ended
	 */
	UFUNCTION(BlueprintCallable, Category = "Getgud|Game")
	static bool MarkEndGame(const FString& GameGuid);

	/**
	 * Flush waits until all queued actions are sent before returning.
	 * Uses timeout from config (markEndGameBlockingTimeoutMilliseconds).
	 * @return true on success, false on timeout
	 */
	UFUNCTION(BlueprintCallable, Category = "Getgud|Game")
	static bool Flush();

	/**
	 * Set the winning team for a match
	 */
	UFUNCTION(BlueprintCallable, Category = "Getgud|Game")
	static bool SetMatchWinTeam(const FString& MatchGuid, const FString& TeamGuid);

	// ============================================
	// Actions
	// ============================================

	/**
	 * Send a spawn action
	 */
	UFUNCTION(BlueprintCallable, Category = "Getgud|Actions")
	static bool SendSpawnAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, const FString& CharacterGuid, const FString& TeamGuid, float InitialHealth, const FGetgudPosition& Position, const FGetgudRotation& Rotation);

	/**
	 * Send a position update action
	 */
	UFUNCTION(BlueprintCallable, Category = "Getgud|Actions")
	static bool SendPositionAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, const FGetgudPosition& Position, const FGetgudRotation& Rotation);

	/**
	 * Send an attack action
	 */
	UFUNCTION(BlueprintCallable, Category = "Getgud|Actions")
	static bool SendAttackAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, const FString& WeaponGuid);

	/**
	 * Send a damage action
	 */
	UFUNCTION(BlueprintCallable, Category = "Getgud|Actions")
	static bool SendDamageAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, const FString& VictimPlayerGuid, float DamageDone, const FString& WeaponGuid);

	/**
	 * Send a death action
	 */
	UFUNCTION(BlueprintCallable, Category = "Getgud|Actions")
	static bool SendDeathAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, const FString& AttackerGuid);

	/**
	 * Send a heal action
	 */
	UFUNCTION(BlueprintCallable, Category = "Getgud|Actions")
	static bool SendHealAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, float HealthGained);

	/**
	 * Send an affect action (buff/debuff state changes)
	 */
	UFUNCTION(BlueprintCallable, Category = "Getgud|Actions")
	static bool SendAffectAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, const FString& AffectGuid, EGetgudAffectState AffectState);

	// ============================================
	// Reports/Chat/Players
	// ============================================

	/**
	 * Send an in-match report
	 */
	UFUNCTION(BlueprintCallable, Category = "Getgud|Reports")
	static bool SendInMatchReport(const FString& MatchGuid, const FString& ReporterName, EGetgudReporterType ReporterType, EGetgudReporterSubtype ReporterSubType, const FString& SuspectedPlayerGuid, EGetgudToxicBehavior TbType, int64 TbTimeEpoch, int32 SuggestedToxicityScore, int64 ReportedTimeEpoch);

	/**
	 * Send a chat message
	 */
	UFUNCTION(BlueprintCallable, Category = "Getgud|Chat")
	static bool SendChatMessage(const FString& MatchGuid, int64 MessageTimeEpoch, const FString& PlayerGuid, const FString& Message);

	/**
	 * Send out-of-match report with credentials
	 */
	UFUNCTION(BlueprintCallable, Category = "Getgud|Reports")
	static bool SendReport(int32 TitleId, const FString& PrivateKey, const FGetgudReportInfo& Report);

	/**
	 * Update player information with credentials
	 */
	UFUNCTION(BlueprintCallable, Category = "Getgud|Players")
	static bool UpdatePlayer(int32 TitleId, const FString& PrivateKey, const FGetgudPlayerInfo& Player);
};
