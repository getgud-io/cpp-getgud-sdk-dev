using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace GetGudSdk
{
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

    public struct SendHealActionInfo
    {
        public BaseActionData baseData;
        public float healthGained;
    };

    public struct SendSpawnActionInfo
    {
        public BaseActionData baseData;
        public string characterGuid;
        public int teamId;
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
    };
    public struct StartMatchInfo
    {
        public string gameGuid;
        public string matchMode;
        public string mapName;
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
        public PlayerInfo(string playerGuid,
                          string playerNickname = "",
                          string playerEmail = "",
                          int playerRank = -1,
                          long playerJoinDateEpoch = -1)
        {
            this.playerGuid = playerGuid;
            this.playerNickname = playerNickname;
            this.playerEmail = playerEmail;
            this.playerRank = playerRank;
            this.playerJoinDateEpoch = playerJoinDateEpoch;
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
            int tbType = -1,
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

    static internal class Methods
    {
#pragma warning disable CS8601, CS0649
        /**
        * Init:
        *
        * Init Getgud SDK
        **/
        static public int Init()
        {
            return GetGudSdk_calls.GetGudSdk_calls.init();
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
            var unmanagedInfo = new GetGudSdk_calls.GetGudSdk_calls.StartGameInfoWrapper
            {
                titleId = info.TitleId,
                privateKey = Marshal.StringToHGlobalAnsi(info.PrivateKey),
                privateKeySize = info.PrivateKey.Length,
                serverGuid = Marshal.StringToHGlobalAnsi(info.ServerGuid),
                serverGuidSize = info.ServerGuid.Length,
                gameMode = Marshal.StringToHGlobalAnsi(info.GameMode),
                gameModeSize = info.GameMode.Length
            };

            // call unmanaged function
            IntPtr gameGuidOutPtr = Marshal.AllocHGlobal(36);
            var result = GetGudSdk_calls.GetGudSdk_calls.StartGame(unmanagedInfo, gameGuidOutPtr);

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
            var unmanagedInfo = new GetGudSdk_calls.GetGudSdk_calls.StartMatchInfoWrapper
            {
                gameGuid = Marshal.StringToHGlobalAnsi(info.gameGuid),
                gameGuidSize = info.gameGuid.Length,
                matchMode = Marshal.StringToHGlobalAnsi(info.matchMode),
                matchModeSize = info.matchMode.Length,
                mapName = Marshal.StringToHGlobalAnsi(info.mapName),
                mapNameSize = info.mapName.Length
            };

            // call unmanaged function
            IntPtr matchGuidOutPtr = Marshal.AllocHGlobal(36);
            var result = GetGudSdk_calls.GetGudSdk_calls.StartMatch(unmanagedInfo, matchGuidOutPtr);

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
            var result = GetGudSdk_calls.GetGudSdk_calls.MarkEndGame(unmanagedGameGuid, gameGuid.Length);

            Marshal.FreeHGlobal(unmanagedGameGuid);

            return result;
        }

        /**
         * SendAffectAction:
         *
         **/
        static public int SendAffectkAction(SendAffectActionInfo info)
        {
            var unmanagedWeaponGuid = Marshal.StringToHGlobalAnsi(info.affectGuid);
            var unmanagedBaseData = new GetGudSdk_calls.GetGudSdk_calls.BaseActionDataWrapper
            {
                actionTimeEpoch = info.baseData.actionTimeEpoch,
                matchGuid = Marshal.StringToHGlobalAnsi(info.baseData.matchGuid),
                matchGuidSize = info.baseData.matchGuid.Length,
                playerGuid = Marshal.StringToHGlobalAnsi(info.baseData.playerGuid),
                playerGuidSize = info.baseData.playerGuid.Length
            };

            var result = GetGudSdk_calls.GetGudSdk_calls.SendAffectAction(unmanagedBaseData, unmanagedWeaponGuid, info.affectGuid.Length, info.affectState);

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
            var unmanagedBaseData = new GetGudSdk_calls.GetGudSdk_calls.BaseActionDataWrapper
            {
                actionTimeEpoch = info.baseData.actionTimeEpoch,
                matchGuid = Marshal.StringToHGlobalAnsi(info.baseData.matchGuid),
                matchGuidSize = info.baseData.matchGuid.Length,
                playerGuid = Marshal.StringToHGlobalAnsi(info.baseData.playerGuid),
                playerGuidSize = info.baseData.playerGuid.Length
            };

            var result = GetGudSdk_calls.GetGudSdk_calls.SendAttackAction(unmanagedBaseData, unmanagedWeaponGuid, info.weaponGuid.Length);

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

            var unmanagedBaseData = new GetGudSdk_calls.GetGudSdk_calls.BaseActionDataWrapper
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

            var result = GetGudSdk_calls.GetGudSdk_calls.SendDamageAction(unmanagedBaseData, victimPlayerGuid, victimPlayerGuidSize, info.damageDone, weaponGuid, weaponGuidSize);

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
            var unmanagedBaseData = new GetGudSdk_calls.GetGudSdk_calls.BaseActionDataWrapper
            {
                actionTimeEpoch = info.baseData.actionTimeEpoch,
                matchGuid = Marshal.StringToHGlobalAnsi(info.baseData.matchGuid),
                matchGuidSize = info.baseData.matchGuid.Length,
                playerGuid = Marshal.StringToHGlobalAnsi(info.baseData.playerGuid),
                playerGuidSize = info.baseData.playerGuid.Length
            };

            var result = GetGudSdk_calls.GetGudSdk_calls.SendHealAction(unmanagedBaseData, info.healthGained);

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
            var unmanagedBaseData = new GetGudSdk_calls.GetGudSdk_calls.BaseActionDataWrapper
            {
                actionTimeEpoch = info.baseData.actionTimeEpoch,
                matchGuid = Marshal.StringToHGlobalAnsi(info.baseData.matchGuid),
                matchGuidSize = info.baseData.matchGuid.Length,
                playerGuid = Marshal.StringToHGlobalAnsi(info.baseData.playerGuid),
                playerGuidSize = info.baseData.playerGuid.Length
            };

            IntPtr characterGuid = Marshal.StringToHGlobalAnsi(info.characterGuid);
            int characterGuidSize = info.characterGuid.Length;

            var result = GetGudSdk_calls.GetGudSdk_calls.SendSpawnAction(unmanagedBaseData, characterGuid, characterGuidSize, info.teamId, info.initialHealth,
                info.position, info.rotation);

            Marshal.FreeHGlobal(unmanagedBaseData.matchGuid);
            Marshal.FreeHGlobal(unmanagedBaseData.playerGuid);
            Marshal.FreeHGlobal(characterGuid);

            return result;
        }

        /**
         * SendDeathAction:
         *
         **/
        static public int SendDeathAction(BaseActionData info)
        {
            var unmanagedBaseData = new GetGudSdk_calls.GetGudSdk_calls.BaseActionDataWrapper
            {
                actionTimeEpoch = info.actionTimeEpoch,
                matchGuid = Marshal.StringToHGlobalAnsi(info.matchGuid),
                matchGuidSize = info.matchGuid.Length,
                playerGuid = Marshal.StringToHGlobalAnsi(info.playerGuid),
                playerGuidSize = info.playerGuid.Length
            };

            var result = GetGudSdk_calls.GetGudSdk_calls.SendDeathAction(unmanagedBaseData);

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
            var unmanagedBaseData = new GetGudSdk_calls.GetGudSdk_calls.BaseActionDataWrapper
            {
                actionTimeEpoch = info.baseData.actionTimeEpoch,
                matchGuid = Marshal.StringToHGlobalAnsi(info.baseData.matchGuid),
                matchGuidSize = info.baseData.matchGuid.Length,
                playerGuid = Marshal.StringToHGlobalAnsi(info.baseData.playerGuid),
                playerGuidSize = info.baseData.playerGuid.Length
            };

            var result = GetGudSdk_calls.GetGudSdk_calls.SendPositionAction(unmanagedBaseData, info.position, info.rotation);

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
            var unmanagedBaseData = new GetGudSdk_calls.GetGudSdk_calls.ReportInfoWrapper
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

            var result = GetGudSdk_calls.GetGudSdk_calls.SendInMatchReport(unmanagedBaseData);

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
            var unmanagedBaseData = new GetGudSdk_calls.GetGudSdk_calls.ChatMessageWrapper
            {
                messageTimeEpoch = info.messageTimeEpoch,
                matchGuid = Marshal.StringToHGlobalAnsi(info.matchGuid),
                matchGuidSize = info.matchGuid.Length,
                playerGuid = Marshal.StringToHGlobalAnsi(info.playerGuid),
                playerGuidSize = info.playerGuid.Length,
                message = Marshal.StringToHGlobalAnsi(info.message),
                messageSize = info.message.Length
            };

            var result = GetGudSdk_calls.GetGudSdk_calls.SendChatMessage(unmanagedBaseData);

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

            var unmanagedBaseData = new GetGudSdk_calls.GetGudSdk_calls.ReportInfoWrapper
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

            var result = GetGudSdk_calls.GetGudSdk_calls.SendReport(titleId, privateKeyPtr, privateKeySize, unmanagedBaseData);

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
        static public int UpdatePlayer(int titleId,
        string privateKey, PlayerInfo player)
        {

            IntPtr privateKeyPtr = Marshal.StringToHGlobalAnsi(privateKey);
            int privateKeySize = privateKey.Length;

            var unmanagedBaseData = new GetGudSdk_calls.GetGudSdk_calls.PlayerInfoWrapper
            {
                playerGuid = Marshal.StringToHGlobalAnsi(player.playerGuid),
                playerGuidSize = player.playerGuid.Length,
                playerNickname = Marshal.StringToHGlobalAnsi(player.playerNickname),
                playerNicknameSize = player.playerNickname.Length,
                playerEmail = Marshal.StringToHGlobalAnsi(player.playerEmail),
                playerEmailSize = player.playerEmail.Length,
                playerRank = player.playerRank,
                playerJoinDateEpoch = player.playerJoinDateEpoch
            };

            var result = GetGudSdk_calls.GetGudSdk_calls.UpdatePlayer(titleId, privateKeyPtr, privateKeySize, unmanagedBaseData);

            Marshal.FreeHGlobal(privateKeyPtr);
            Marshal.FreeHGlobal(unmanagedBaseData.playerGuid);
            Marshal.FreeHGlobal(unmanagedBaseData.playerNickname);
            Marshal.FreeHGlobal(unmanagedBaseData.playerEmail);

            return result;
        }

        /**
         * Dispose:
         *
         **/
        static public void dispose()
        {
            GetGudSdk_calls.GetGudSdk_calls.dispose();
        }

#pragma warning restore CS8601, CS0649
    }

}
