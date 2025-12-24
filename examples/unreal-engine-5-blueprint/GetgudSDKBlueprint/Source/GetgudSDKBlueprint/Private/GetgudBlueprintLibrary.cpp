#include "GetgudBlueprintLibrary.h"
#include "GetgudSDK_C.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"
#include "Interfaces/IPluginManager.h"
#include <cstdlib>
#include <cstring>

// Helper to convert FGetgudPosition to C PositionF
static PositionF ToCPosition(const FGetgudPosition& Pos)
{
	PositionF Result;
	Result.X = Pos.X;
	Result.Y = Pos.Y;
	Result.Z = Pos.Z;
	return Result;
}

// Helper to convert FGetgudRotation to C RotationF
static RotationF ToCRotation(const FGetgudRotation& Rot)
{
	RotationF Result;
	Result.Yaw = Rot.Yaw;
	Result.Pitch = Rot.Pitch;
	Result.Roll = Rot.Roll;
	return Result;
}

// Helper to convert EGetgudAffectState to C AffectState enum
static enum AffectState ToCAffectState(EGetgudAffectState State)
{
	switch (State)
	{
	case EGetgudAffectState::Attach: return Attach;
	case EGetgudAffectState::Activate: return Activate;
	case EGetgudAffectState::Deactivate: return Deactivate;
	case EGetgudAffectState::Detach: return Detach;
	default: return Attach;
	}
}

// Helper to convert EGetgudReporterType to C int
static int ToCReporterType(EGetgudReporterType Type)
{
	if (Type == EGetgudReporterType::None)
	{
		return -1; // C SDK uses -1 for None
	}
	return static_cast<int>(Type);
}

// Helper to convert EGetgudReporterSubtype to C int
static int ToCReporterSubtype(EGetgudReporterSubtype Type)
{
	if (Type == EGetgudReporterSubtype::None)
	{
		return -1; // C SDK uses -1 for None
	}
	return static_cast<int>(Type);
}

// Helper to convert EGetgudToxicBehavior to C int
static int ToCTbType(EGetgudToxicBehavior Type)
{
	return static_cast<int>(Type);
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
		UE_LOG(LogTemp, Log, TEXT("GetgudSDK: Using config from GETGUD_CONFIG_PATH env var"));
		return init() != 0;
	}

	// Search for config.json in multiple locations
	TArray<FString> SearchPaths;

	// Path 1: Use IPluginManager to get plugin directory (works in Editor)
	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("GetgudSDKBlueprint"));
	if (Plugin.IsValid())
	{
		SearchPaths.Add(FPaths::Combine(
			Plugin->GetBaseDir(),
			TEXT("ThirdParty"),
			TEXT("GetgudSDK"),
			TEXT("bin"),
			TEXT("Win64"),
			TEXT("config.json")
		));
	}

	// Path 2: Next to executable (for packaged games)
	SearchPaths.Add(FPaths::Combine(
		FPaths::GetPath(FPlatformProcess::ExecutablePath()),
		TEXT("config.json")
	));

	// Path 3: Project Binaries folder
	SearchPaths.Add(FPaths::Combine(
		FPaths::ProjectDir(),
		TEXT("Binaries"),
		TEXT("Win64"),
		TEXT("config.json")
	));

	// Try each path
	for (const FString& Path : SearchPaths)
	{
		FString FullPath = FPaths::ConvertRelativePathToFull(Path);
		if (FPaths::FileExists(FullPath))
		{
			UE_LOG(LogTemp, Log, TEXT("GetgudSDK: Found config.json at: %s"), *FullPath);
			return init_conf_path(TCHAR_TO_UTF8(*FullPath)) != 0;
		}
	}

	// Log error if not found
	UE_LOG(LogTemp, Error, TEXT("GetgudSDK: config.json not found! Searched paths:"));
	for (const FString& Path : SearchPaths)
	{
		UE_LOG(LogTemp, Error, TEXT("  - %s"), *FPaths::ConvertRelativePathToFull(Path));
	}

	return false;
}

bool UGetgudBlueprintLibrary::InitWithPath(const FString& ConfigFileFullPath)
{
	return init_conf_path(TCHAR_TO_UTF8(*ConfigFileFullPath)) != 0;
}

bool UGetgudBlueprintLibrary::InitWithContent(const FString& ConfigFile, bool PassAsContent)
{
	return init_conf(TCHAR_TO_UTF8(*ConfigFile), PassAsContent ? 1 : 0) != 0;
}

void UGetgudBlueprintLibrary::Dispose()
{
	dispose();
}

// ============================================
// Game/Match Management
// ============================================

FString UGetgudBlueprintLibrary::StartGame(int32 TitleId, const FString& PrivateKey, const FString& ServerGuid, const FString& GameMode, const FString& ServerLocation)
{
	struct ::StartGameInfo gameInfo;
	gameInfo.titleId = TitleId;

	auto PrivateKeyUtf8 = StringCast<ANSICHAR>(*PrivateKey);
	gameInfo.privateKey = PrivateKeyUtf8.Get();
	gameInfo.privateKeySize = PrivateKey.Len();

	auto ServerGuidUtf8 = StringCast<ANSICHAR>(*ServerGuid);
	gameInfo.serverGuid = ServerGuidUtf8.Get();
	gameInfo.serverGuidSize = ServerGuid.Len();

	auto GameModeUtf8 = StringCast<ANSICHAR>(*GameMode);
	gameInfo.gameMode = GameModeUtf8.Get();
	gameInfo.gameModeSize = GameMode.Len();

	auto ServerLocationUtf8 = StringCast<ANSICHAR>(*ServerLocation);
	gameInfo.serverLocation = ServerLocationUtf8.Get();
	gameInfo.serverLocationSize = ServerLocation.Len();

	char gameGuidOut[128] = {0};
	int size = ::StartGame(gameInfo, gameGuidOut);

	if (size > 0)
	{
		return FString(UTF8_TO_TCHAR(gameGuidOut));
	}
	return FString();
}

FString UGetgudBlueprintLibrary::StartMatch(const FString& GameGuid, const FString& MatchMode, const FString& MapName, const FString& CustomField)
{
	struct ::StartMatchInfo matchInfo;

	auto GameGuidUtf8 = StringCast<ANSICHAR>(*GameGuid);
	matchInfo.gameGuid = GameGuidUtf8.Get();
	matchInfo.gameGuidSize = GameGuid.Len();

	auto MatchModeUtf8 = StringCast<ANSICHAR>(*MatchMode);
	matchInfo.matchMode = MatchModeUtf8.Get();
	matchInfo.matchModeSize = MatchMode.Len();

	auto MapNameUtf8 = StringCast<ANSICHAR>(*MapName);
	matchInfo.mapName = MapNameUtf8.Get();
	matchInfo.mapNameSize = MapName.Len();

	auto CustomFieldUtf8 = StringCast<ANSICHAR>(*CustomField);
	matchInfo.customField = CustomFieldUtf8.Get();
	matchInfo.customFieldSize = CustomField.Len();

	char matchGuidOut[128] = {0};
	int size = ::StartMatch(matchInfo, matchGuidOut);

	if (size > 0)
	{
		return FString(UTF8_TO_TCHAR(matchGuidOut));
	}
	return FString();
}

bool UGetgudBlueprintLibrary::MarkEndGame(const FString& GameGuid)
{
	auto GameGuidUtf8 = StringCast<ANSICHAR>(*GameGuid);
	return ::MarkEndGame(GameGuidUtf8.Get(), GameGuid.Len()) != 0;
}

bool UGetgudBlueprintLibrary::SetMatchWinTeam(const FString& MatchGuid, const FString& TeamGuid)
{
	auto MatchGuidUtf8 = StringCast<ANSICHAR>(*MatchGuid);
	auto TeamGuidUtf8 = StringCast<ANSICHAR>(*TeamGuid);
	return ::SetMatchWinTeam(MatchGuidUtf8.Get(), MatchGuid.Len(), TeamGuidUtf8.Get(), TeamGuid.Len()) != 0;
}

// ============================================
// Actions
// ============================================

bool UGetgudBlueprintLibrary::SendSpawnAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, const FString& CharacterGuid, const FString& TeamGuid, float InitialHealth, const FGetgudPosition& Position, const FGetgudRotation& Rotation)
{
	struct ::BaseActionData baseData;
	baseData.actionTimeEpoch = ActionTimeEpoch;

	auto MatchGuidUtf8 = StringCast<ANSICHAR>(*MatchGuid);
	baseData.matchGuid = MatchGuidUtf8.Get();
	baseData.matchGuidSize = MatchGuid.Len();

	auto PlayerGuidUtf8 = StringCast<ANSICHAR>(*PlayerGuid);
	baseData.playerGuid = PlayerGuidUtf8.Get();
	baseData.playerGuidSize = PlayerGuid.Len();

	auto CharacterGuidUtf8 = StringCast<ANSICHAR>(*CharacterGuid);
	auto TeamGuidUtf8 = StringCast<ANSICHAR>(*TeamGuid);

	PositionF pos = ToCPosition(Position);
	RotationF rot = ToCRotation(Rotation);

	return ::SendSpawnAction(baseData,
		CharacterGuidUtf8.Get(), CharacterGuid.Len(),
		TeamGuidUtf8.Get(), TeamGuid.Len(),
		InitialHealth, pos, rot) != 0;
}

bool UGetgudBlueprintLibrary::SendPositionAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, const FGetgudPosition& Position, const FGetgudRotation& Rotation)
{
	struct ::BaseActionData baseData;
	baseData.actionTimeEpoch = ActionTimeEpoch;

	auto MatchGuidUtf8 = StringCast<ANSICHAR>(*MatchGuid);
	baseData.matchGuid = MatchGuidUtf8.Get();
	baseData.matchGuidSize = MatchGuid.Len();

	auto PlayerGuidUtf8 = StringCast<ANSICHAR>(*PlayerGuid);
	baseData.playerGuid = PlayerGuidUtf8.Get();
	baseData.playerGuidSize = PlayerGuid.Len();

	PositionF pos = ToCPosition(Position);
	RotationF rot = ToCRotation(Rotation);

	return ::SendPositionAction(baseData, pos, rot) != 0;
}

bool UGetgudBlueprintLibrary::SendAttackAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, const FString& WeaponGuid)
{
	struct ::BaseActionData baseData;
	baseData.actionTimeEpoch = ActionTimeEpoch;

	auto MatchGuidUtf8 = StringCast<ANSICHAR>(*MatchGuid);
	baseData.matchGuid = MatchGuidUtf8.Get();
	baseData.matchGuidSize = MatchGuid.Len();

	auto PlayerGuidUtf8 = StringCast<ANSICHAR>(*PlayerGuid);
	baseData.playerGuid = PlayerGuidUtf8.Get();
	baseData.playerGuidSize = PlayerGuid.Len();

	auto WeaponGuidUtf8 = StringCast<ANSICHAR>(*WeaponGuid);

	return ::SendAttackAction(baseData, WeaponGuidUtf8.Get(), WeaponGuid.Len()) != 0;
}

bool UGetgudBlueprintLibrary::SendDamageAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, const FString& VictimPlayerGuid, float DamageDone, const FString& WeaponGuid)
{
	struct ::BaseActionData baseData;
	baseData.actionTimeEpoch = ActionTimeEpoch;

	auto MatchGuidUtf8 = StringCast<ANSICHAR>(*MatchGuid);
	baseData.matchGuid = MatchGuidUtf8.Get();
	baseData.matchGuidSize = MatchGuid.Len();

	auto PlayerGuidUtf8 = StringCast<ANSICHAR>(*PlayerGuid);
	baseData.playerGuid = PlayerGuidUtf8.Get();
	baseData.playerGuidSize = PlayerGuid.Len();

	auto VictimGuidUtf8 = StringCast<ANSICHAR>(*VictimPlayerGuid);
	auto WeaponGuidUtf8 = StringCast<ANSICHAR>(*WeaponGuid);

	return ::SendDamageAction(baseData,
		VictimGuidUtf8.Get(), VictimPlayerGuid.Len(),
		DamageDone,
		WeaponGuidUtf8.Get(), WeaponGuid.Len()) != 0;
}

bool UGetgudBlueprintLibrary::SendDeathAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, const FString& AttackerGuid)
{
	struct ::BaseActionData baseData;
	baseData.actionTimeEpoch = ActionTimeEpoch;

	auto MatchGuidUtf8 = StringCast<ANSICHAR>(*MatchGuid);
	baseData.matchGuid = MatchGuidUtf8.Get();
	baseData.matchGuidSize = MatchGuid.Len();

	auto PlayerGuidUtf8 = StringCast<ANSICHAR>(*PlayerGuid);
	baseData.playerGuid = PlayerGuidUtf8.Get();
	baseData.playerGuidSize = PlayerGuid.Len();

	auto AttackerGuidUtf8 = StringCast<ANSICHAR>(*AttackerGuid);

	return ::SendDeathAction(baseData, AttackerGuidUtf8.Get(), AttackerGuid.Len()) != 0;
}

bool UGetgudBlueprintLibrary::SendHealAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& PlayerGuid, float HealthGained)
{
	struct ::BaseActionData baseData;
	baseData.actionTimeEpoch = ActionTimeEpoch;

	auto MatchGuidUtf8 = StringCast<ANSICHAR>(*MatchGuid);
	baseData.matchGuid = MatchGuidUtf8.Get();
	baseData.matchGuidSize = MatchGuid.Len();

	auto PlayerGuidUtf8 = StringCast<ANSICHAR>(*PlayerGuid);
	baseData.playerGuid = PlayerGuidUtf8.Get();
	baseData.playerGuidSize = PlayerGuid.Len();

	return ::SendHealAction(baseData, HealthGained) != 0;
}

bool UGetgudBlueprintLibrary::SendAffectAction(const FString& MatchGuid, int64 ActionTimeEpoch, const FString& AffectGuid, const FString& PlayerGuid, EGetgudAffectState AffectState)
{
	struct ::BaseActionData baseData;
	baseData.actionTimeEpoch = ActionTimeEpoch;

	auto MatchGuidUtf8 = StringCast<ANSICHAR>(*MatchGuid);
	baseData.matchGuid = MatchGuidUtf8.Get();
	baseData.matchGuidSize = MatchGuid.Len();

	auto PlayerGuidUtf8 = StringCast<ANSICHAR>(*PlayerGuid);
	baseData.playerGuid = PlayerGuidUtf8.Get();
	baseData.playerGuidSize = PlayerGuid.Len();

	auto AffectGuidUtf8 = StringCast<ANSICHAR>(*AffectGuid);

	return ::SendAffectAction(baseData, AffectGuidUtf8.Get(), AffectGuid.Len(), ToCAffectState(AffectState)) != 0;
}

// ============================================
// Reports/Chat/Players
// ============================================

bool UGetgudBlueprintLibrary::SendInMatchReport(const FGetgudReportInfo& InReportInfo)
{
	struct ::ReportInfo cReport;

	auto MatchGuidUtf8 = StringCast<ANSICHAR>(*InReportInfo.MatchGuid);
	cReport.matchGuid = MatchGuidUtf8.Get();
	cReport.matchGuidSize = InReportInfo.MatchGuid.Len();

	auto ReporterNameUtf8 = StringCast<ANSICHAR>(*InReportInfo.ReporterName);
	cReport.reporterName = ReporterNameUtf8.Get();
	cReport.reporterNameSize = InReportInfo.ReporterName.Len();

	cReport.reporterType = ToCReporterType(InReportInfo.ReporterType);
	cReport.reporterSubType = ToCReporterSubtype(InReportInfo.ReporterSubType);

	auto SuspectedPlayerGuidUtf8 = StringCast<ANSICHAR>(*InReportInfo.SuspectedPlayerGuid);
	cReport.suspectedPlayerGuid = SuspectedPlayerGuidUtf8.Get();
	cReport.suspectedPlayerGuidSize = InReportInfo.SuspectedPlayerGuid.Len();

	cReport.tbType = ToCTbType(InReportInfo.TbType);
	cReport.tbTimeEpoch = InReportInfo.TbTimeEpoch;
	cReport.suggestedToxicityScore = InReportInfo.SuggestedToxicityScore;
	cReport.reportedTimeEpoch = InReportInfo.ReportedTimeEpoch;

	return ::SendInMatchReport(cReport) != 0;
}

bool UGetgudBlueprintLibrary::SendChatMessage(const FGetgudChatMessage& MessageInfo)
{
	struct ::ChatMessageInfo cMessage;
	cMessage.messageTimeEpoch = MessageInfo.MessageTimeEpoch;

	auto MatchGuidUtf8 = StringCast<ANSICHAR>(*MessageInfo.MatchGuid);
	cMessage.matchGuid = MatchGuidUtf8.Get();
	cMessage.matchGuidSize = MessageInfo.MatchGuid.Len();

	auto PlayerGuidUtf8 = StringCast<ANSICHAR>(*MessageInfo.PlayerGuid);
	cMessage.playerGuid = PlayerGuidUtf8.Get();
	cMessage.playerGuidSize = MessageInfo.PlayerGuid.Len();

	auto MessageUtf8 = StringCast<ANSICHAR>(*MessageInfo.Message);
	cMessage.message = MessageUtf8.Get();
	cMessage.messageSize = MessageInfo.Message.Len();

	return ::SendChatMessage(cMessage) != 0;
}

bool UGetgudBlueprintLibrary::SendReports(int32 TitleId, const FString& PrivateKey, const TArray<FGetgudReportInfo>& Reports)
{
	auto PrivateKeyUtf8 = StringCast<ANSICHAR>(*PrivateKey);

	bool bAllSuccess = true;
	for (const FGetgudReportInfo& Report : Reports)
	{
		struct ::ReportInfo cReport;

		auto MatchGuidUtf8 = StringCast<ANSICHAR>(*Report.MatchGuid);
		cReport.matchGuid = MatchGuidUtf8.Get();
		cReport.matchGuidSize = Report.MatchGuid.Len();

		auto ReporterNameUtf8 = StringCast<ANSICHAR>(*Report.ReporterName);
		cReport.reporterName = ReporterNameUtf8.Get();
		cReport.reporterNameSize = Report.ReporterName.Len();

		cReport.reporterType = ToCReporterType(Report.ReporterType);
		cReport.reporterSubType = ToCReporterSubtype(Report.ReporterSubType);

		auto SuspectedPlayerGuidUtf8 = StringCast<ANSICHAR>(*Report.SuspectedPlayerGuid);
		cReport.suspectedPlayerGuid = SuspectedPlayerGuidUtf8.Get();
		cReport.suspectedPlayerGuidSize = Report.SuspectedPlayerGuid.Len();

		cReport.tbType = ToCTbType(Report.TbType);
		cReport.tbTimeEpoch = Report.TbTimeEpoch;
		cReport.suggestedToxicityScore = Report.SuggestedToxicityScore;
		cReport.reportedTimeEpoch = Report.ReportedTimeEpoch;

		if (::SendReport(TitleId, PrivateKeyUtf8.Get(), PrivateKey.Len(), cReport) == 0)
		{
			bAllSuccess = false;
		}
	}
	return bAllSuccess;
}

bool UGetgudBlueprintLibrary::UpdatePlayers(int32 TitleId, const FString& PrivateKey, const TArray<FGetgudPlayerInfo>& Players)
{
	auto PrivateKeyUtf8 = StringCast<ANSICHAR>(*PrivateKey);

	bool bAllSuccess = true;
	for (const FGetgudPlayerInfo& Player : Players)
	{
		// Prepare all string conversions first (they need to stay in scope)
		auto PlayerGuidUtf8 = StringCast<ANSICHAR>(*Player.PlayerGuid);
		auto PlayerNicknameUtf8 = StringCast<ANSICHAR>(*Player.PlayerNickname);
		auto PlayerEmailUtf8 = StringCast<ANSICHAR>(*Player.PlayerEmail);
		auto PlayerSuspectScoreUtf8 = StringCast<ANSICHAR>(*Player.PlayerSuspectScore);
		auto PlayerReputationUtf8 = StringCast<ANSICHAR>(*Player.PlayerReputation);
		auto PlayerStatusUtf8 = StringCast<ANSICHAR>(*Player.PlayerStatus);
		auto PlayerCampaignUtf8 = StringCast<ANSICHAR>(*Player.PlayerCampaign);
		auto PlayerNotesUtf8 = StringCast<ANSICHAR>(*Player.PlayerNotes);
		auto PlayerDeviceUtf8 = StringCast<ANSICHAR>(*Player.PlayerDevice);
		auto PlayerOSUtf8 = StringCast<ANSICHAR>(*Player.PlayerOS);
		auto PlayerGenderUtf8 = StringCast<ANSICHAR>(*Player.PlayerGender);
		auto PlayerLocationUtf8 = StringCast<ANSICHAR>(*Player.PlayerLocation);

		// Prepare transactions
		TArray<struct ::PlayerTransactions> cTransactions;
		TArray<TArray<ANSICHAR>> transGuidBuffers;
		TArray<TArray<ANSICHAR>> transNameBuffers;

		for (const FGetgudPlayerTransaction& Trans : Player.Transactions)
		{
			// Store UTF8 strings in buffers that persist
			auto TransGuidUtf8 = StringCast<ANSICHAR>(*Trans.TransactionGuid);
			auto TransNameUtf8 = StringCast<ANSICHAR>(*Trans.TransactionName);

			TArray<ANSICHAR> guidBuf;
			guidBuf.Append(TransGuidUtf8.Get(), Trans.TransactionGuid.Len() + 1);
			transGuidBuffers.Add(MoveTemp(guidBuf));

			TArray<ANSICHAR> nameBuf;
			nameBuf.Append(TransNameUtf8.Get(), Trans.TransactionName.Len() + 1);
			transNameBuffers.Add(MoveTemp(nameBuf));
		}

		// Now build the transactions array with pointers to our buffers
		for (int32 i = 0; i < Player.Transactions.Num(); i++)
		{
			struct ::PlayerTransactions cTrans;
			cTrans.TransactionGuid = transGuidBuffers[i].GetData();
			cTrans.TransactionGuidSize = Player.Transactions[i].TransactionGuid.Len();
			cTrans.TransactionName = transNameBuffers[i].GetData();
			cTrans.TransactionNameSize = Player.Transactions[i].TransactionName.Len();
			cTrans.TransactionDateEpoch = Player.Transactions[i].TransactionDateEpoch;
			cTrans.TransactionValueUSD = Player.Transactions[i].TransactionValueUSD;
			cTransactions.Add(cTrans);
		}

		struct ::PlayerInfo cPlayer;
		cPlayer.playerGuid = PlayerGuidUtf8.Get();
		cPlayer.playerGuidSize = Player.PlayerGuid.Len();
		cPlayer.playerNickname = PlayerNicknameUtf8.Get();
		cPlayer.playerNicknameSize = Player.PlayerNickname.Len();
		cPlayer.playerEmail = PlayerEmailUtf8.Get();
		cPlayer.playerEmailSize = Player.PlayerEmail.Len();
		cPlayer.playerRank = Player.PlayerRank;
		cPlayer.playerJoinDateEpoch = Player.PlayerJoinDateEpoch;
		cPlayer.playerSuspectScore = PlayerSuspectScoreUtf8.Get();
		cPlayer.playerSuspectScoreSize = Player.PlayerSuspectScore.Len();
		cPlayer.playerReputation = PlayerReputationUtf8.Get();
		cPlayer.playerReputationSize = Player.PlayerReputation.Len();
		cPlayer.playerStatus = PlayerStatusUtf8.Get();
		cPlayer.playerStatusSize = Player.PlayerStatus.Len();
		cPlayer.playerCampaign = PlayerCampaignUtf8.Get();
		cPlayer.playerCampaignSize = Player.PlayerCampaign.Len();
		cPlayer.playerNotes = PlayerNotesUtf8.Get();
		cPlayer.playerNotesSize = Player.PlayerNotes.Len();
		cPlayer.playerDevice = PlayerDeviceUtf8.Get();
		cPlayer.playerDeviceSize = Player.PlayerDevice.Len();
		cPlayer.playerOS = PlayerOSUtf8.Get();
		cPlayer.playerOSSize = Player.PlayerOS.Len();
		cPlayer.playerAge = Player.PlayerAge;
		cPlayer.playerGender = PlayerGenderUtf8.Get();
		cPlayer.playerGenderSize = Player.PlayerGender.Len();
		cPlayer.playerLocation = PlayerLocationUtf8.Get();
		cPlayer.playerLocationSize = Player.PlayerLocation.Len();
		cPlayer.transactions = cTransactions.Num() > 0 ? cTransactions.GetData() : nullptr;
		cPlayer.transactionsSize = cTransactions.Num();

		if (::UpdatePlayer(TitleId, PrivateKeyUtf8.Get(), PrivateKey.Len(), cPlayer) == 0)
		{
			bAllSuccess = false;
		}
	}
	return bAllSuccess;
}
