using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Transactions;

namespace GetgudSDK
{
    public struct PlayerTransactionsData
    {
        public string TransactionGuid;
        public string TransactionName;
        public long TransactionDateEpoch;
        public float TransactionValueUSD;
    };

    public struct BaseActionData
    {
        public long actionTimeEpoch;
        public string matchGuid;
        public string playerGuid;
    };
    public struct SendAffectActionInfo
    {
        public BaseActionData baseData;
        public string affectGuid;
        public AffectState affectState;
    };
    public struct SendAttackActionInfo
    {
        public BaseActionData baseData;
        public string weaponGuid;
    };
    public struct SendDamageActionInfo
    {
        public BaseActionData baseData;
        public string victimPlayerGuid;
        public float damageDone;
        public string weaponGuid;
    };
    public struct SendDeathActionInfo
    {
        public BaseActionData baseData;
        public string attackerGuid;
    };
    public struct SendHealActionInfo
    {
        public BaseActionData baseData;
        public float healthGained;
    };

    public struct SendSpawnActionInfo
    {
        public BaseActionData baseData;
        public string characterGuid;
		public string teamGuid;
        public float initialHealth;
        public PositionF position;
        public RotationF rotation;
    };

    public struct SendPositionActionInfo
    {
        public BaseActionData baseData;
        public PositionF position;
        public RotationF rotation;
    };

    public struct StartGameInfo
    {
        public int TitleId;
        public string PrivateKey;
        public string ServerGuid;
        public string GameMode;
        public string ServerLocation;
    };
    public struct StartMatchInfo
    {
        public string gameGuid;
        public string matchMode;
        public string mapName;
        public string customField;

    };
    public struct ChatMessageInfo
    {
        public long messageTimeEpoch;
        public string matchGuid;
        public string playerGuid;
        public string message;
    };

    public struct PlayerInfo
    {
        public string playerGuid;
        public string playerNickname;
        public string playerEmail;
        public int playerRank;
        public long playerJoinDateEpoch;
        public string playerSuspectScore;
        public string playerReputation;
        public string playerStatus;
        public string playerCampaign;
        public string playerNotes;
        public string playerDevice;
        public string playerOS;
        public int playerAge;
        public string playerGender;
        public string playerLocation;
        public List<PlayerTransactionsData> transactions;
        public PlayerInfo(string playerGuid,
                          string playerNickname = "",
                          string playerEmail = "",
                          int playerRank = -1,
                          long playerJoinDateEpoch = -1,
                          string playerSuspectScore = "",
                          string playerReputation = "",
                          string playerStatus = "",
                          string playerCampaign = "",
                          string playerNotes = "",
                          string playerDevice = "",
                          string playerOS = "",
                          int playerAge = -1,
                          string playerGender = "",
                          string playerLocation = "",
                          List<PlayerTransactionsData>? transactions = null)
        {
            this.playerGuid = playerGuid;
            this.playerNickname = playerNickname;
            this.playerEmail = playerEmail;
            this.playerRank = playerRank;
            this.playerJoinDateEpoch = playerJoinDateEpoch;
            this.playerSuspectScore = playerSuspectScore;
            this.playerReputation = playerReputation;
            this.playerStatus = playerStatus;
            this.playerCampaign = playerCampaign;
            this.playerNotes = playerNotes;
            this.playerDevice = playerDevice;
            this.playerOS = playerOS;
            this.playerAge = playerAge;
            this.playerGender = playerGender;
            this.playerLocation = playerLocation;
            this.transactions = transactions ?? new List<PlayerTransactionsData>();
        }
    };

    public struct ReportInfo
    {
        public string matchGuid;
        public string reporterName;
        public int reporterType;
        public int reporterSubType;
        public string suspectedPlayerGuid;
        public int tbType;
        public long tbTimeEpoch;
        public int suggestedToxicityScore;
        public long reportedTimeEpoch;
        public ReportInfo(
            string matchGuid,
            long reportedTimeEpoch,
            string suspectedPlayerGuid,
            string reporterName = "",
            int reporterType = -1,
            int reporterSubType = -1,
            int tbType = 1,
            long tbTimeEpoch = -1,
            int suggestedToxicityScore = -1
            )
        {
            this.matchGuid = matchGuid;
            this.reporterName = reporterName;
            this.reporterType = reporterType;
            this.reporterSubType = reporterSubType;
            this.suspectedPlayerGuid = suspectedPlayerGuid;
            this.tbType = tbType;
            this.tbTimeEpoch = tbTimeEpoch;
            this.suggestedToxicityScore = suggestedToxicityScore;
            this.reportedTimeEpoch = reportedTimeEpoch;

        }
    };

    static public class Methods
    {
#pragma warning disable CS8601, CS0649
        /**
        * Init:
        *
        * Init Getgud SDK
        **/
        static public int Init()
        {
            return GetgudSDK_calls.GetgudSDK_calls.init();
        }
		
		/**
		* Init:
		*
		* Init Getgud SDK
		**/
        static public int InitConfPath(string configFilePath)
        {
			var unmanagedConfigFile = Marshal.StringToHGlobalAnsi(configFilePath);
            var result = GetgudSDK_calls.GetgudSDK_calls.init_conf_path(unmanagedConfigFile);

            Marshal.FreeHGlobal(unmanagedConfigFile);
			
            return result;
        }
		
		/**
		* Init:
		*
		* Init Getgud SDK
		* *passAsContent: true - read the configFile as a string
		* *passAsContent: false - read the configFile as a file path
		**/
        static public int InitConf(string configFile, int passAsContent)
        {
			var unmanagedConfigFile = Marshal.StringToHGlobalAnsi(configFile);
            var result = GetgudSDK_calls.GetgudSDK_calls.init_conf(unmanagedConfigFile, passAsContent);

            Marshal.FreeHGlobal(unmanagedConfigFile);
			
            return result;
        }

        /**
         * StartGame:
         *
         * Start new game
         * Returns size
         **/
        static public int StartGame(StartGameInfo info, out string gameGuidOut)
        {
            // convert managed struct to unmanaged struct
            var unmanagedInfo = new GetgudSDK_calls.GetgudSDK_calls.StartGameInfoWrapper
            {
                titleId = info.TitleId,
                privateKey = Marshal.StringToHGlobalAnsi(info.PrivateKey),
                privateKeySize = info.PrivateKey.Length,
                serverGuid = Marshal.StringToHGlobalAnsi(info.ServerGuid),
                serverGuidSize = info.ServerGuid.Length,
                gameMode = Marshal.StringToHGlobalAnsi(info.GameMode),
                gameModeSize = info.GameMode.Length,
                serverLocation = Marshal.StringToHGlobalAnsi(info.ServerLocation),
                serverLocationSize = info.ServerLocation.Length
            };

            // call unmanaged function
            IntPtr gameGuidOutPtr = Marshal.AllocHGlobal(36);
            var result = GetgudSDK_calls.GetgudSDK_calls.StartGame(unmanagedInfo, gameGuidOutPtr);

            if (gameGuidOutPtr == IntPtr.Zero || result == 0)
            {
                gameGuidOut = "";
            }
            else
            {
                gameGuidOut = Marshal.PtrToStringAnsi(gameGuidOutPtr, result);
            }

            // free unmanaged memory
            Marshal.FreeHGlobal(unmanagedInfo.privateKey);
            Marshal.FreeHGlobal(unmanagedInfo.serverGuid);
            Marshal.FreeHGlobal(unmanagedInfo.gameMode);
            Marshal.FreeHGlobal(gameGuidOutPtr);

            return result;
        }

        /**
         * Start match:
         *
         * Start a new match for an existing game
         **/
        static public int StartMatch(StartMatchInfo info, out string matchGuidOut)
        {
            var unmanagedInfo = new GetgudSDK_calls.GetgudSDK_calls.StartMatchInfoWrapper
            {
                gameGuid = Marshal.StringToHGlobalAnsi(info.gameGuid),
                gameGuidSize = info.gameGuid.Length,
                matchMode = Marshal.StringToHGlobalAnsi(info.matchMode),
                matchModeSize = info.matchMode.Length,
                mapName = Marshal.StringToHGlobalAnsi(info.mapName),
                mapNameSize = info.mapName.Length,
                customField = Marshal.StringToHGlobalAnsi(info.customField),
                customFieldSize = info.customField?.Length ?? 0
            };

            // call unmanaged function
            IntPtr matchGuidOutPtr = Marshal.AllocHGlobal(36);
            var result = GetgudSDK_calls.GetgudSDK_calls.StartMatch(unmanagedInfo, matchGuidOutPtr);

            if (matchGuidOutPtr == IntPtr.Zero || result == 0)
            {
                matchGuidOut = "";
            }
            else
            {
                matchGuidOut = Marshal.PtrToStringAnsi(matchGuidOutPtr, result);
            }

            // free unmanaged memory
            Marshal.FreeHGlobal(unmanagedInfo.gameGuid);
            Marshal.FreeHGlobal(unmanagedInfo.matchMode);
            Marshal.FreeHGlobal(unmanagedInfo.mapName);
            Marshal.FreeHGlobal(unmanagedInfo.customField);
            Marshal.FreeHGlobal(matchGuidOutPtr);

            return result;
        }

        /**
         * MarkEndGame:
         *
         * Mark started game as finished
         **/
        static public int MarkEndGame(string gameGuid)
        {
            var unmanagedGameGuid = Marshal.StringToHGlobalAnsi(gameGuid);
            var result = GetgudSDK_calls.GetgudSDK_calls.MarkEndGame(unmanagedGameGuid, gameGuid.Length);

            Marshal.FreeHGlobal(unmanagedGameGuid);

            return result;
        }

        /**
         * SetMatchWinTeam:
         *
         * Set the winning team for a match
         **/
        static public int SetMatchWinTeam(string matchGuid, string teamGuid)
        {
            IntPtr matchGuidPtr = Marshal.StringToHGlobalAnsi(matchGuid);
            IntPtr teamGuidPtr = Marshal.StringToHGlobalAnsi(teamGuid);

            var result = GetgudSDK_calls.GetgudSDK_calls.SetMatchWinTeam(matchGuidPtr, matchGuid.Length, teamGuidPtr, teamGuid.Length);

            Marshal.FreeHGlobal(matchGuidPtr);
            Marshal.FreeHGlobal(teamGuidPtr);

            return result;
        }

        /**
         * SendAffectAction:
         *
         **/
        static public int SendAffectkAction(SendAffectActionInfo info)
        {
            var unmanagedWeaponGuid = Marshal.StringToHGlobalAnsi(info.affectGuid);
            var unmanagedBaseData = new GetgudSDK_calls.GetgudSDK_calls.BaseActionDataWrapper
            {
                actionTimeEpoch = info.baseData.actionTimeEpoch,
                matchGuid = Marshal.StringToHGlobalAnsi(info.baseData.matchGuid),
                matchGuidSize = info.baseData.matchGuid.Length,
                playerGuid = Marshal.StringToHGlobalAnsi(info.baseData.playerGuid),
                playerGuidSize = info.baseData.playerGuid.Length
            };

            var result = GetgudSDK_calls.GetgudSDK_calls.SendAffectAction(unmanagedBaseData, unmanagedWeaponGuid, info.affectGuid.Length, info.affectState);

            Marshal.FreeHGlobal(unmanagedBaseData.matchGuid);
            Marshal.FreeHGlobal(unmanagedBaseData.playerGuid);
            Marshal.FreeHGlobal(unmanagedWeaponGuid);

            return result;
        }

        /**
         * SendAttackAction:
         *
         **/
        static public int SendAttackAction(SendAttackActionInfo info)
        {
            var unmanagedWeaponGuid = Marshal.StringToHGlobalAnsi(info.weaponGuid);
            var unmanagedBaseData = new GetgudSDK_calls.GetgudSDK_calls.BaseActionDataWrapper
            {
                actionTimeEpoch = info.baseData.actionTimeEpoch,
                matchGuid = Marshal.StringToHGlobalAnsi(info.baseData.matchGuid),
                matchGuidSize = info.baseData.matchGuid.Length,
                playerGuid = Marshal.StringToHGlobalAnsi(info.baseData.playerGuid),
                playerGuidSize = info.baseData.playerGuid.Length
            };

            var result = GetgudSDK_calls.GetgudSDK_calls.SendAttackAction(unmanagedBaseData, unmanagedWeaponGuid, info.weaponGuid.Length);

            Marshal.FreeHGlobal(unmanagedBaseData.matchGuid);
            Marshal.FreeHGlobal(unmanagedBaseData.playerGuid);
            Marshal.FreeHGlobal(unmanagedWeaponGuid);

            return result;
        }

        /**
         * SendDamageAction:
         *
         **/
        static public int SendDamageAction(SendDamageActionInfo info)
        {

            var unmanagedBaseData = new GetgudSDK_calls.GetgudSDK_calls.BaseActionDataWrapper
            {
                actionTimeEpoch = info.baseData.actionTimeEpoch,
                matchGuid = Marshal.StringToHGlobalAnsi(info.baseData.matchGuid),
                matchGuidSize = info.baseData.matchGuid.Length,
                playerGuid = Marshal.StringToHGlobalAnsi(info.baseData.playerGuid),
                playerGuidSize = info.baseData.playerGuid.Length
            };

            IntPtr victimPlayerGuid = Marshal.StringToHGlobalAnsi(info.victimPlayerGuid);
            int victimPlayerGuidSize = info.victimPlayerGuid.Length;

            IntPtr weaponGuid = Marshal.StringToHGlobalAnsi(info.weaponGuid);
            int weaponGuidSize = info.weaponGuid.Length;

            var result = GetgudSDK_calls.GetgudSDK_calls.SendDamageAction(unmanagedBaseData, victimPlayerGuid, victimPlayerGuidSize, info.damageDone, weaponGuid, weaponGuidSize);

            Marshal.FreeHGlobal(unmanagedBaseData.matchGuid);
            Marshal.FreeHGlobal(unmanagedBaseData.playerGuid);
            Marshal.FreeHGlobal(victimPlayerGuid);
            Marshal.FreeHGlobal(weaponGuid);

            return result;
        }

        /**
         * SendHealAction:
         *
         **/
        static public int SendHealAction(SendHealActionInfo info)
        {
            var unmanagedBaseData = new GetgudSDK_calls.GetgudSDK_calls.BaseActionDataWrapper
            {
                actionTimeEpoch = info.baseData.actionTimeEpoch,
                matchGuid = Marshal.StringToHGlobalAnsi(info.baseData.matchGuid),
                matchGuidSize = info.baseData.matchGuid.Length,
                playerGuid = Marshal.StringToHGlobalAnsi(info.baseData.playerGuid),
                playerGuidSize = info.baseData.playerGuid.Length
            };

            var result = GetgudSDK_calls.GetgudSDK_calls.SendHealAction(unmanagedBaseData, info.healthGained);

            Marshal.FreeHGlobal(unmanagedBaseData.matchGuid);
            Marshal.FreeHGlobal(unmanagedBaseData.playerGuid);

            return result;
        }

        /**
         * SendSpawnAction:
         *
         **/
        static public int SendSpawnAction(SendSpawnActionInfo info)
        {
            var unmanagedBaseData = new GetgudSDK_calls.GetgudSDK_calls.BaseActionDataWrapper
            {
                actionTimeEpoch = info.baseData.actionTimeEpoch,
                matchGuid = Marshal.StringToHGlobalAnsi(info.baseData.matchGuid),
                matchGuidSize = info.baseData.matchGuid.Length,
                playerGuid = Marshal.StringToHGlobalAnsi(info.baseData.playerGuid),
                playerGuidSize = info.baseData.playerGuid.Length
            };

            IntPtr characterGuid = Marshal.StringToHGlobalAnsi(info.characterGuid);
            int characterGuidSize = info.characterGuid.Length;
			
			IntPtr teamGuid = Marshal.StringToHGlobalAnsi(info.teamGuid);
            int teamGuidSize = info.teamGuid.Length;

            var result = GetgudSDK_calls.GetgudSDK_calls.SendSpawnAction(unmanagedBaseData, characterGuid, characterGuidSize, teamGuid, teamGuidSize, info.initialHealth,
                info.position, info.rotation);

            Marshal.FreeHGlobal(unmanagedBaseData.matchGuid);
            Marshal.FreeHGlobal(unmanagedBaseData.playerGuid);
            Marshal.FreeHGlobal(characterGuid);
			Marshal.FreeHGlobal(teamGuid);

            return result;
        }

        /**
         * SendDeathAction:
         *
         **/
        static public int SendDeathAction(SendDeathActionInfo info)
        {
            var unmanagedBaseData = new GetgudSDK_calls.GetgudSDK_calls.BaseActionDataWrapper
            {
                actionTimeEpoch = info.baseData.actionTimeEpoch,
                matchGuid = Marshal.StringToHGlobalAnsi(info.baseData.matchGuid),
                matchGuidSize = info.baseData.matchGuid.Length,
                playerGuid = Marshal.StringToHGlobalAnsi(info.baseData.playerGuid),
                playerGuidSize = info.baseData.playerGuid.Length
            };

            IntPtr attackerGuid = Marshal.StringToHGlobalAnsi(info.attackerGuid);
            int attackerGuidSize = info.attackerGuid.Length;

            var result = GetgudSDK_calls.GetgudSDK_calls.SendDeathAction(unmanagedBaseData, attackerGuid, attackerGuidSize);

            Marshal.FreeHGlobal(unmanagedBaseData.matchGuid);
            Marshal.FreeHGlobal(unmanagedBaseData.playerGuid);

            return result;
        }
        
        /**
         * SendPositionAction:
         *
         **/
        static public int SendPositionAction(SendPositionActionInfo info)
        {
            var unmanagedBaseData = new GetgudSDK_calls.GetgudSDK_calls.BaseActionDataWrapper
            {
                actionTimeEpoch = info.baseData.actionTimeEpoch,
                matchGuid = Marshal.StringToHGlobalAnsi(info.baseData.matchGuid),
                matchGuidSize = info.baseData.matchGuid.Length,
                playerGuid = Marshal.StringToHGlobalAnsi(info.baseData.playerGuid),
                playerGuidSize = info.baseData.playerGuid.Length
            };

            var result = GetgudSDK_calls.GetgudSDK_calls.SendPositionAction(unmanagedBaseData, info.position, info.rotation);

            Marshal.FreeHGlobal(unmanagedBaseData.matchGuid);
            Marshal.FreeHGlobal(unmanagedBaseData.playerGuid);

            return result;
        }
        /**
         * SendInMatchReport:
         *
         * Send a report which belongs to specifc match which is now live
         **/
        static public int SendInMatchReport(ReportInfo info)
        {
            var unmanagedBaseData = new GetgudSDK_calls.GetgudSDK_calls.ReportInfoWrapper
            {
                matchGuid = Marshal.StringToHGlobalAnsi(info.matchGuid),
                matchGuidSize = info.matchGuid.Length,
                reporterName = Marshal.StringToHGlobalAnsi(info.reporterName),
                reporterNameSize = info.reporterName.Length,
                reporterType = info.reporterType,
                reporterSubType = info.reporterSubType,
                suspectedPlayerGuid = Marshal.StringToHGlobalAnsi(info.suspectedPlayerGuid),
                suspectedPlayerGuidSize = info.suspectedPlayerGuid.Length,
                tbType = info.tbType,
                tbTimeEpoch = info.tbTimeEpoch,
                suggestedToxicityScore = info.suggestedToxicityScore,
                reportedTimeEpoch = info.reportedTimeEpoch
            };

            var result = GetgudSDK_calls.GetgudSDK_calls.SendInMatchReport(unmanagedBaseData);

            Marshal.FreeHGlobal(unmanagedBaseData.matchGuid);
            Marshal.FreeHGlobal(unmanagedBaseData.reporterName);
            Marshal.FreeHGlobal(unmanagedBaseData.suspectedPlayerGuid);

            return result;
        }

        /**
         * SendChatMessage:
         *
         *  Send a message which belongs to specifc match which is now live
         **/
        static public int SendChatMessage(ChatMessageInfo info)
        {
            var unmanagedBaseData = new GetgudSDK_calls.GetgudSDK_calls.ChatMessageWrapper
            {
                messageTimeEpoch = info.messageTimeEpoch,
                matchGuid = Marshal.StringToHGlobalAnsi(info.matchGuid),
                matchGuidSize = info.matchGuid.Length,
                playerGuid = Marshal.StringToHGlobalAnsi(info.playerGuid),
                playerGuidSize = info.playerGuid.Length,
                message = Marshal.StringToHGlobalAnsi(info.message),
                messageSize = info.message.Length
            };

            var result = GetgudSDK_calls.GetgudSDK_calls.SendChatMessage(unmanagedBaseData);

            Marshal.FreeHGlobal(unmanagedBaseData.matchGuid);
            Marshal.FreeHGlobal(unmanagedBaseData.playerGuid);
            Marshal.FreeHGlobal(unmanagedBaseData.message);

            return result;
        }
        
        /**
         * SendReport:
         *
         * Send report which are outside of the live match
         **/
        static public int SendReport(int titleId,
          string privateKey, ReportInfo report)
        {

            IntPtr privateKeyPtr = Marshal.StringToHGlobalAnsi(privateKey);
            int privateKeySize = privateKey.Length;

            var unmanagedBaseData = new GetgudSDK_calls.GetgudSDK_calls.ReportInfoWrapper
            {
                matchGuid = Marshal.StringToHGlobalAnsi(report.matchGuid),
                matchGuidSize = report.matchGuid.Length,
                reporterName = Marshal.StringToHGlobalAnsi(report.reporterName),
                reporterNameSize = report.reporterName.Length,
                reporterType = report.reporterType,
                reporterSubType = report.reporterSubType,
                suspectedPlayerGuid = Marshal.StringToHGlobalAnsi(report.suspectedPlayerGuid),
                suspectedPlayerGuidSize = report.suspectedPlayerGuid.Length,
                tbType = report.tbType,
                tbTimeEpoch = report.tbTimeEpoch,
                suggestedToxicityScore = report.suggestedToxicityScore,
                reportedTimeEpoch = report.reportedTimeEpoch
            };

            var result = GetgudSDK_calls.GetgudSDK_calls.SendReport(titleId, privateKeyPtr, privateKeySize, unmanagedBaseData);

            Marshal.FreeHGlobal(privateKeyPtr);
            Marshal.FreeHGlobal(unmanagedBaseData.matchGuid);
            Marshal.FreeHGlobal(unmanagedBaseData.reporterName);
            Marshal.FreeHGlobal(unmanagedBaseData.suspectedPlayerGuid);

            return result;

        }
        
        /**
         * UpdatePlayer:
         *
         * Update player info outside of the live match
         **/
        static public int UpdatePlayer(int titleId, string privateKey, PlayerInfo player)
        {
            IntPtr privateKeyPtr = Marshal.StringToHGlobalAnsi(privateKey);
            int privateKeySize = privateKey.Length;

            var unmanagedBaseData = new GetgudSDK_calls.GetgudSDK_calls.PlayerInfoWrapper
            {
                playerGuid = Marshal.StringToHGlobalAnsi(player.playerGuid),
                playerGuidSize = player.playerGuid.Length,
                playerNickname = Marshal.StringToHGlobalAnsi(player.playerNickname),
                playerNicknameSize = player.playerNickname.Length,
                playerEmail = Marshal.StringToHGlobalAnsi(player.playerEmail),
                playerEmailSize = player.playerEmail?.Length ?? 0,
                playerRank = player.playerRank,
                playerJoinDateEpoch = player.playerJoinDateEpoch,
                playerSuspectScore = Marshal.StringToHGlobalAnsi(player.playerSuspectScore),
                playerSuspectScoreSize = player.playerSuspectScore?.Length ?? 0,
                playerReputation = Marshal.StringToHGlobalAnsi(player.playerReputation),
                playerReputationSize = player.playerReputation?.Length ?? 0,
                playerStatus = Marshal.StringToHGlobalAnsi(player.playerStatus),
                playerStatusSize = player.playerStatus?.Length ?? 0,
                playerCampaign = Marshal.StringToHGlobalAnsi(player.playerCampaign),
                playerCampaignSize = player.playerCampaign?.Length ?? 0,
                playerNotes = Marshal.StringToHGlobalAnsi(player.playerNotes),
                playerNotesSize = player.playerNotes?.Length ?? 0,
                playerDevice = Marshal.StringToHGlobalAnsi(player.playerDevice),
                playerDeviceSize = player.playerDevice?.Length ?? 0,
                playerOS = Marshal.StringToHGlobalAnsi(player.playerOS),
                playerOSSize = player.playerOS?.Length ?? 0,
                playerAge = player.playerAge,
                playerGender = Marshal.StringToHGlobalAnsi(player.playerGender),
                playerGenderSize = player.playerGender?.Length ?? 0,
                playerLocation = Marshal.StringToHGlobalAnsi(player.playerLocation),
                playerLocationSize = player.playerLocation?.Length ?? 0,
                transactions = IntPtr.Zero,  // This will be set later if there are transactions
                transactionsSize = 0  // This will be updated later if there are transactions
            };

             // Handle transactions
            if (player.transactions != null && player.transactions.Count > 0)
            {
                unmanagedBaseData.transactions = Marshal.AllocHGlobal(Marshal.SizeOf<GetgudSDK.PlayerTransactions>() * player.transactions.Count);
                unmanagedBaseData.transactionsSize = player.transactions.Count;

                for (int i = 0; i < player.transactions.Count; i++)
                {
                    var transaction = new GetgudSDK.PlayerTransactions
                    {
                        TransactionGuid = Marshal.StringToHGlobalAnsi(player.transactions[i].TransactionGuid),
                        TransactionGuidSize = player.transactions[i].TransactionGuid.Length,
                        TransactionName = Marshal.StringToHGlobalAnsi(player.transactions[i].TransactionName),
                        TransactionNameSize = player.transactions[i].TransactionName.Length,
                        TransactionDateEpoch = player.transactions[i].TransactionDateEpoch,
                        TransactionValueUSD = player.transactions[i].TransactionValueUSD
                    };

                    IntPtr transactionPtr = new IntPtr(unmanagedBaseData.transactions.ToInt64() + i * Marshal.SizeOf<GetgudSDK.PlayerTransactions>());
                    Marshal.StructureToPtr(transaction, transactionPtr, false);
                }
            }
            else
            {
                unmanagedBaseData.transactions = IntPtr.Zero;
                unmanagedBaseData.transactionsSize = 0;
            }

            var result = GetgudSDK_calls.GetgudSDK_calls.UpdatePlayer(titleId, privateKeyPtr, privateKeySize, unmanagedBaseData);

            // Free allocated memory
            Marshal.FreeHGlobal(privateKeyPtr);
            Marshal.FreeHGlobal(unmanagedBaseData.playerGuid);
            Marshal.FreeHGlobal(unmanagedBaseData.playerNickname);
            Marshal.FreeHGlobal(unmanagedBaseData.playerEmail);
            Marshal.FreeHGlobal(unmanagedBaseData.playerSuspectScore);
            Marshal.FreeHGlobal(unmanagedBaseData.playerReputation);
            Marshal.FreeHGlobal(unmanagedBaseData.playerStatus);
            Marshal.FreeHGlobal(unmanagedBaseData.playerCampaign);
            Marshal.FreeHGlobal(unmanagedBaseData.playerNotes);
            Marshal.FreeHGlobal(unmanagedBaseData.playerDevice);
            Marshal.FreeHGlobal(unmanagedBaseData.playerOS);
            Marshal.FreeHGlobal(unmanagedBaseData.playerGender);
            Marshal.FreeHGlobal(unmanagedBaseData.playerLocation);

            // Free transaction memory if allocated
            if (unmanagedBaseData.transactions != IntPtr.Zero)
            {
                for (int i = 0; i < unmanagedBaseData.transactionsSize; i++)
                {
                    IntPtr transactionPtr = new IntPtr(unmanagedBaseData.transactions.ToInt64() + i * Marshal.SizeOf<GetgudSDK.PlayerTransactions>());
                    var transaction = Marshal.PtrToStructure<GetgudSDK.PlayerTransactions>(transactionPtr);
                    
                    Marshal.FreeHGlobal(transaction.TransactionGuid);
                    Marshal.FreeHGlobal(transaction.TransactionName);
                }
                Marshal.FreeHGlobal(unmanagedBaseData.transactions);
            }

            return result;
        }

        /**
         * Dispose:
         *
         **/
        static public void dispose()
        {
            GetgudSDK_calls.GetgudSDK_calls.dispose();
        }

#pragma warning restore CS8601, CS0649
    }

}
