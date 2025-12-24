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
	UFUNCTION(BlueprintCallable, Category = "GetGud|Init")
	static bool Init();

	/**
	 * Initialize GetGud SDK with config file path
	 */
	UFUNCTION(BlueprintCallable, Category = "GetGud|Init")
	static bool InitWithPath(const FString& ConfigFileFullPath);

	/**
	 * Initialize GetGud SDK with config content or path
	 * @param ConfigFile - Config file path or content string
	 * @param PassAsContent - true: treat ConfigFile as JSON content, false: treat as file path
	 */
	UFUNCTION(BlueprintCallable, Category = "GetGud|Init")
	static bool InitWithContent(const FString& ConfigFile, bool PassAsContent);

	/**
	 * Dispose/shutdown GetGud SDK
	 */
	UFUNCTION(BlueprintCallable, Category = "GetGud|Init")
	static void Dispose();

	// ============================================
	// Game/Match Management
	// ============================================

	/**
	 * Start a new game session with credentials
	 * @return GameGuid on success, empty string on failure
	 */
	UFUNCTION(BlueprintCallable, Category = "GetGud|Game")
	static FString StartGame(int32 TitleId, const FString& PrivateKey, const FString& ServerGuid, const FString& GameMode, const FString& ServerLocation);

	/**
	 * Start a new match within a game
	 * @return MatchGuid on success, empty string on failure
	 */
	UFUNCTION(BlueprintCallable, Category = "GetGud|Game")
	static FString StartMatch(const FString& GameGuid, const FString& MatchMode, const FString& MapName, const FString& CustomField);

	/**
	 * Mark a game as ended
	 */
	UFUNCTION(BlueprintCallable, Category = "GetGud|Game")
	static bool MarkEndGame(const FString& GameGuid);

	/**
	 * Set the winning team for a match
	 */
	UFUNCTION(BlueprintCallable, Category = "GetGud|Game")
	static bool SetMatchWinTeam(const FString& MatchGuid, const FString& TeamGuid);

	// ============================================
	// Actions
	// ============================================

	/**
	 * Send a spawn action
	 */
	UFUNCTION(BlueprintCallable, Category = "GetGud|Actions")
	static bool SendSpawnAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, const FString& CharacterGuid, const FString& TeamGuid, float InitialHealth, const FGetgudPosition& Position, const FGetgudRotation& Rotation);

	/**
	 * Send a position update action
	 */
	UFUNCTION(BlueprintCallable, Category = "GetGud|Actions")
	static bool SendPositionAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, const FGetgudPosition& Position, const FGetgudRotation& Rotation);

	/**
	 * Send an attack action
	 */
	UFUNCTION(BlueprintCallable, Category = "GetGud|Actions")
	static bool SendAttackAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, const FString& WeaponGuid);

	/**
	 * Send a damage action
	 */
	UFUNCTION(BlueprintCallable, Category = "GetGud|Actions")
	static bool SendDamageAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, const FString& VictimPlayerGuid, float DamageDone, const FString& WeaponGuid);

	/**
	 * Send a death action
	 */
	UFUNCTION(BlueprintCallable, Category = "GetGud|Actions")
	static bool SendDeathAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, const FString& AttackerGuid);

	/**
	 * Send a heal action
	 */
	UFUNCTION(BlueprintCallable, Category = "GetGud|Actions")
	static bool SendHealAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, float HealthGained);

	/**
	 * Send an affect action (buff/debuff state changes)
	 */
	UFUNCTION(BlueprintCallable, Category = "GetGud|Actions")
	static bool SendAffectAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& AffectGuid, const FString& PlayerGuid, EGetgudAffectState AffectState);

	// ============================================
	// Reports/Chat/Players
	// ============================================

	/**
	 * Send an in-match report
	 */
	UFUNCTION(BlueprintCallable, Category = "GetGud|Reports")
	static bool SendInMatchReport(const FGetgudReportInfo& ReportInfo);

	/**
	 * Send a chat message
	 */
	UFUNCTION(BlueprintCallable, Category = "GetGud|Chat")
	static bool SendChatMessage(const FGetgudChatMessage& MessageInfo);

	/**
	 * Send out-of-match reports with credentials
	 */
	UFUNCTION(BlueprintCallable, Category = "GetGud|Reports")
	static bool SendReports(int32 TitleId, const FString& PrivateKey, const TArray<FGetgudReportInfo>& Reports);

	/**
	 * Update player information with credentials
	 */
	UFUNCTION(BlueprintCallable, Category = "GetGud|Players")
	static bool UpdatePlayers(int32 TitleId, const FString& PrivateKey, const TArray<FGetgudPlayerInfo>& Players);
};
