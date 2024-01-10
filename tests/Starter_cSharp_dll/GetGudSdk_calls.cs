using GetGudSdk;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
//using static GetGudSdk.SDKWrapper;

namespace GetGudSdk
{
    public enum Actions { None = -1, Affect, Attack, Damage, Death, Heal, Position, Spawn };
    public enum AffectState { Attach, Activate, Deactivate, Detach };

    public struct PositionF
    {
        public float X;
        public float Y;
        public float Z;
    };

    public struct RotationF
    {
        public float Yaw;
        public float Pitch;
        public float Roll;
    };

    public struct PlayerTransactions
    {
        public IntPtr TransactionGuid;
        public int TransactionGuidSize;
        public IntPtr TransactionName;
        public int TransactionNameSize;
        public long TransactionDateEpoch;
        public float TransactionValueUSD;
    };
}

namespace GetGudSdk_calls
{
    static class GetGudSdk_calls
    {
#pragma warning disable CS8601, CS0649

        public struct StartGameInfoWrapper
        {
            public int titleId;
            public IntPtr privateKey;
            public int privateKeySize;
            public IntPtr serverGuid;
            public int serverGuidSize;
            public IntPtr gameMode;
            public int gameModeSize;
            public IntPtr serverLocation;
            public int serverLocationSize;
        }

        public struct StartMatchInfoWrapper
        {
            public IntPtr gameGuid;
            public int gameGuidSize;
            public IntPtr matchMode;
            public int matchModeSize;
            public IntPtr mapName;
            public int mapNameSize;
        };

        public struct ChatMessageWrapper
        {
            public long messageTimeEpoch;
            public IntPtr matchGuid;
            public int matchGuidSize;
            public IntPtr playerGuid;
            public int playerGuidSize;
            public IntPtr message;
            public int messageSize;
        };

        public struct PlayerInfoWrapper
        {
            public IntPtr playerGuid;
            public int playerGuidSize;
            public IntPtr playerNickname;
            public int playerNicknameSize;
            public IntPtr playerEmail;
            public int playerEmailSize;
            public int playerRank;
            public long playerJoinDateEpoch;
            public IntPtr playerSuspectScore;
            public int playerSuspectScoreSize;
            public IntPtr playerReputation;
            public int playerReputationSize;
            public IntPtr playerStatus;
            public int playerStatusSize;
            public IntPtr playerCompaign;
            public int playerCompaignSize;
            public IntPtr playerNotes;
            public int playerNotesSize;
            public IntPtr playerDevice;
            public int playerDeviceSize;
            public IntPtr playerOS;
            public int playerOSSize;
            public int playerAge;
            public IntPtr playerGender;
            public int playerGenderSize;
            public IntPtr playerLocation;
            public int playerLocationSize;
            public IntPtr transactions;
            public int transactionsSize;
        };

        public struct ReportInfoWrapper
        {
            public IntPtr matchGuid;
            public int matchGuidSize;
            public IntPtr reporterName;
            public int reporterNameSize;
            public int reporterType;
            public int reporterSubType;
            public IntPtr suspectedPlayerGuid;
            public int suspectedPlayerGuidSize;
            public int tbType;
            public long tbTimeEpoch;
            public int suggestedToxicityScore;
            public long reportedTimeEpoch;
        };

        public struct BaseActionDataWrapper
        {
            public long actionTimeEpoch;
            public IntPtr matchGuid;
            public int matchGuidSize;
            public IntPtr playerGuid;
            public int playerGuidSize;
        }
        public struct SendAffectActionWrapper
        {
            public BaseActionDataWrapper baseData;
            public IntPtr affectGuid;
            public int affectGuidSize;
            public AffectState affectState;
        }
        public struct SendAttackActionWrapper
        {
            public BaseActionDataWrapper baseData;
            public IntPtr weaponGuid;
            public int weaponGuidSize;
        }
        public struct SendDamageActionWrapper
        {
            public BaseActionDataWrapper baseData;
            public IntPtr victimPlayerGuid;
            public int victimPlayerGuidSize;
            public float damageDone;
            public IntPtr weaponGuid;
            public int weaponGuidSize;
        }
        public struct SendDeathActionWrapper
        {
            public BaseActionDataWrapper baseData;
            public IntPtr attackerGuid;
            public int attackerGuidSize;
        }

        public struct SendHealActionWrapper
        {
            public BaseActionDataWrapper baseData;
            public float healthGained;
        }

        public struct SendSpawnActionWrapper
        {
            public BaseActionDataWrapper baseData;
            public IntPtr characterGuid;
            public int characterGuidSize;
            public int teamId;
            public float initialHealth;
            public PositionF position;
            public RotationF rotation;
        }

        public struct SendPositionActionWrapper
        {
            public BaseActionDataWrapper baseData;
            public PositionF position;
            public RotationF rotation;
        }

#pragma warning restore CS8601, CS0649

		// Windows build path
        //sdk_project\tests\Starter_cSharp_dll\bin\Debug\net6.0
		// Linux build path
        //sdk_project\tests\Starter_cSharp_so\bin\Debug\net7.0\linux-64\
		//
		// Linux deploy path
		//examples\Starter_cSharp_so\bin\Debug\net7.0\linux-64\
        const string dllPath = "../../../../../../lib/libGetGudSdk.so";


        [DllImport(dllPath)]
        public static extern int init();

        [DllImport(dllPath)]
        public static extern int StartGame(StartGameInfoWrapper gameInfo, IntPtr gameGuidOut);

        [DllImport(dllPath)]
        public static extern int StartMatch(StartMatchInfoWrapper matchInfo, IntPtr matchGuidOut);

        [DllImport(dllPath)]
        public static extern int MarkEndGame(IntPtr matchGuid, int matchGuidSize);

        [DllImport(dllPath)]
        public static extern int SendAffectAction(BaseActionDataWrapper matchInfo, IntPtr affectGuid, int affectGuidSize, AffectState p_affectState);

        [DllImport(dllPath)]
        public static extern int SendAttackAction(BaseActionDataWrapper matchInfo, IntPtr weaponGuid, int weaponGuidSize);

        [DllImport(dllPath)]
        public static extern int SendDamageAction(BaseActionDataWrapper matchInfo, IntPtr victimPlayerGuid, int victimPlayerGuidSize,
            float damageDone, IntPtr weaponGuid, int weaponGuidSize);

        [DllImport(dllPath)]
        public static extern int SendHealAction(BaseActionDataWrapper matchInfo, float healthGained);

        [DllImport(dllPath)]
        public static extern int SendSpawnAction(BaseActionDataWrapper matchInfo, IntPtr characterGuid, int characterGuidGuidSize,
            int teamId, float initialHealth, PositionF position, RotationF rotation);

        [DllImport(dllPath)]
        public static extern int SendDeathAction(BaseActionDataWrapper matchInfo, IntPtr attackerGuid, int attackerGuidSize);

        [DllImport(dllPath)]
        public static extern int SendPositionAction(BaseActionDataWrapper matchInfo, PositionF position, RotationF rotation);

        [DllImport(dllPath)]
        public static extern int SendInMatchReport(ReportInfoWrapper reportInfo);

        [DllImport(dllPath)]
        public static extern int SendChatMessage(ChatMessageWrapper messageInfo);

        [DllImport(dllPath)]
        public static extern int SendReport(int titleId,
        IntPtr privateKey, int privateKeySize, ReportInfoWrapper report);

        [DllImport(dllPath)]
        public static extern int UpdatePlayer(int titleId,
        IntPtr privateKey, int privateKeySize, PlayerInfoWrapper report);

        [DllImport(dllPath)]
        public static extern void dispose();

    }
}
