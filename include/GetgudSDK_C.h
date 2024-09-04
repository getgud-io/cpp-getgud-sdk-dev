#if defined(__cplusplus) && defined(_WIN32)
#ifndef GETGUDSDK_EXPORTS
#define GETGUDSDK_API __declspec(dllexport)
#else
#define GETGUDSDK_API __declspec(dllimport)
#endif
#else // not win32
#define GETGUDSDK_API
#endif

enum Actions { None = -1, Affect, Attack, Damage, Death, Heal, Position, Spawn };
enum AffectState { Attach, Activate, Deactivate, Detach };

struct PositionF {
	float X;
	float Y;
	float Z;
};

struct RotationF {
	float Yaw;
	float Pitch;
	float Roll;
};

struct PlayerTransactions
{
	const char* TransactionGuid;
	int TransactionGuidSize;
	const char* TransactionName;
	int TransactionNameSize;
	long long TransactionDateEpoch;
	float TransactionValueUSD;
};

struct BaseActionData {
	long long actionTimeEpoch;
	const char* matchGuid;
	int matchGuidSize;
	const char* playerGuid;
	int playerGuidSize;
};
struct StartGameInfo {
	int titleId;
	const char* privateKey;
	int privateKeySize;
	const char* serverGuid;
	int serverGuidSize;
	const char* gameMode;
	int gameModeSize;
	const char* serverLocation;
	int serverLocationSize;
};

struct StartMatchInfo {
	const char* gameGuid;
	int gameGuidSize;
	const char* matchMode;
	int matchModeSize;
	const char* mapName;
	int mapNameSize;
	const char* customField;
	int customFieldSize;
};

struct ChatMessageInfo {
	long long messageTimeEpoch;
	const char* matchGuid;
	int matchGuidSize;
	const char* playerGuid;
	int playerGuidSize;
	const char* message;
	int messageSize;
};

struct PlayerInfo {
	const char* playerGuid;
	int playerGuidSize;
	const char* playerNickname;
	int playerNicknameSize;
	const char* playerEmail;
	int playerEmailSize;
	int playerRank;
	long long playerJoinDateEpoch;
	const char* playerSuspectScore;
	int playerSuspectScoreSize;
	const char* playerReputation;
	int playerReputationSize;
	const char* playerStatus;
	int playerStatusSize;
	const char* PlayerCampaign;
	int PlayerCampaignSize;
	const char* playerNotes;
	int playerNotesSize;
	const char* playerDevice;
	int playerDeviceSize;
	const char* playerOS;
	int playerOSSize;
	int playerAge;
	const char* playerGender;
	int playerGenderSize;
	const char* playerLocation;
	int playerLocationSize;
	struct PlayerTransactions* transactions;
	int transactionsSize;
};

struct ReportInfo {
	const char* matchGuid;
	int matchGuidSize;
	const char* reporterName;
	int reporterNameSize;
	int reporterType;
	int reporterSubType;
	const char* suspectedPlayerGuid;
	int suspectedPlayerGuidSize;
	int tbType;
	long long tbTimeEpoch;
	int suggestedToxicityScore;
	long long reportedTimeEpoch;
};

#ifdef __cplusplus
extern "C" {
#endif

	/**
	* Init:
	*
	* Init Getgud SDK
	**/
	GETGUDSDK_API int init();

	/**
	* Init:
	*
	* Init Getgud SDK
	**/
	GETGUDSDK_API int init_conf_path(const char* configFilePath);

	/**
	* Init:
	*
	* Init Getgud SDK
	* *passAsContent: true - read the configFile as a string
	* *passAsContent: false - read the configFile as a file path
	**/
	GETGUDSDK_API int init_conf(const char* configFile, int passAsContent);

	/**
	 * StartGame:
	 *
	 * Start new game
	 * Returns size
	 **/
	GETGUDSDK_API int StartGame(struct StartGameInfo gameInfo, char* gameGuidOut);

	/**
	 * Start match:
	 *
	 * Start a new match for an existing game
	 **/
	GETGUDSDK_API int StartMatch(struct StartMatchInfo matchInfo, char* matchGuidOut);

	/**
	 * MarkEndGame:
	 *
	 * Mark started game as finished
	 **/
	GETGUDSDK_API int MarkEndGame(const char* gameGuid, int guidSize);

	/**
     * SetMatchWinTeam:
     *
     * Set the winning team for a match
     **/
    GETGUDSDK_API int SetMatchWinTeam(const char* matchGuid, int matchGuidSize, const char* teamGuid, int teamGuidSize);

	/**
	 * SendAffectAction:
	 *
	 **/
	GETGUDSDK_API int SendAffectAction(struct BaseActionData baseData,
		const char* affectGuid,
		int affectGuidSize,
		enum AffectState affectState);

	/**
	 * SendAttackAction:
	 *
	 **/
	GETGUDSDK_API int SendAttackAction(struct BaseActionData baseData,
		const char* weaponGuid,
		int weaponGuidSize);

	/**
	 * SendDamageAction:
	 *
	 **/
	GETGUDSDK_API int SendDamageAction(struct BaseActionData baseData,
		const char* victimPlayerGuid,
		int victimPlayerGuidSize,
		float damageDone,
		const char* weaponGuid,
		int weaponGuidSize);

	/**
	 * SendHealAction:
	 *
	 **/
	GETGUDSDK_API int SendHealAction(struct BaseActionData baseData, float healthGained);

	/**
	 * SendSpawnAction:
	 *
	 **/
	GETGUDSDK_API int SendSpawnAction(struct BaseActionData baseData,
		const char* characterGuid,
		int characterGuidSize,
		const char* teamGuid,
		int teamGuidGuidSize,
		float initialHealth,
		struct PositionF position,
		struct RotationF rotation);

	/**
	 * SendDeathAction:
	 *
	 **/
	GETGUDSDK_API int SendDeathAction(struct BaseActionData baseData,
		const char* attackerGuid,
		int attackerGuidSize);

	/**
	 * SendPositionAction:
	 *
	 **/
	GETGUDSDK_API int SendPositionAction(struct BaseActionData baseData,
		struct PositionF position,
		struct RotationF rotation);
	/**
	 * SendInMatchReport:
	 *
	 * Send a report which belongs to specifc match which is now live
	 **/
	GETGUDSDK_API int SendInMatchReport(struct ReportInfo reportInfo);

	/**
	 * SendChatMessage:
	 *
	 *  Send a message which belongs to specifc match which is now live
	 **/
	GETGUDSDK_API int SendChatMessage(struct ChatMessageInfo messageInfo);

	/**
	 * SendReport:
	 *
	 * Send report which are outside of the live match
	 **/
	GETGUDSDK_API int SendReport(int titleId,
		const char* privateKey, int privateKeySize, struct ReportInfo report);

	/**
	 * UpdatePlayer:
	 *
	 * Update player info outside of the live match
	 **/
	GETGUDSDK_API int UpdatePlayer(int titleId,
		const char* privateKey, int privateKeySize, struct PlayerInfo player);

	/**
	 * Dispose:
	 *
	 **/
	GETGUDSDK_API void dispose();

#ifdef __cplusplus
}
#endif