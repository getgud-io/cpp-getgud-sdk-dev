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


        //cSharp-production\examples\Starter_cSharp_dll\bin\Debug\net6.0\Starter_cSharp_dll.exe
        //const string dllPath = "..\\..\\..\\..\\..\\lib\\GetGudSdk.dll";
        //sdk_project\tests\Starter_cSharp_dll\bin\Debug\net6.0
        const string dllPath = "..\\..\\..\\..\\..\\build\\_build\\Debug\\GetGudSdk.dll";
        //const string dllPath = "..\\..\\..\\..\\..\\build\\_build\\Release\\GetGudSdk.dll";


        [DllImport(dllPath)]
        public static extern int init();

        [DllImport(dllPath)]
        public static extern int StartGame(ref StartGameInfoWrapper gameInfo, IntPtr gameGuidOut);

        [DllImport(dllPath)]
        public static extern int StartMatch(ref StartMatchInfoWrapper matchInfo, IntPtr matchGuidOut);

        [DllImport(dllPath)]
        public static extern int MarkEndGame(IntPtr matchGuid, int matchGuidSize);

        [DllImport(dllPath)]
        public static extern int SendAffectAction(ref BaseActionDataWrapper matchInfo, IntPtr affectGuid, int affectGuidSize, AffectState p_affectState);

        [DllImport(dllPath)]
        public static extern int SendAttackAction(ref BaseActionDataWrapper matchInfo, IntPtr weaponGuid, int weaponGuidSize);

        [DllImport(dllPath)]
        public static extern int SendDamageAction(ref BaseActionDataWrapper matchInfo, IntPtr victimPlayerGuid, int victimPlayerGuidSize,
            float damageDone, IntPtr weaponGuid, int weaponGuidSize);

        [DllImport(dllPath)]
        public static extern int SendHealAction(ref BaseActionDataWrapper matchInfo, float healthGained);

        [DllImport(dllPath)]
        public static extern int SendSpawnAction(ref BaseActionDataWrapper matchInfo, IntPtr characterGuid, int characterGuidGuidSize,
            int teamId, float initialHealth, PositionF position, RotationF rotation);

        [DllImport(dllPath)]
        public static extern int SendDeathAction(ref BaseActionDataWrapper matchInfo);

        [DllImport(dllPath)]
        public static extern int SendPositionAction(ref BaseActionDataWrapper matchInfo, PositionF position, RotationF rotation);

        [DllImport(dllPath)]
        public static extern int SendInMatchReport(ref ReportInfoWrapper reportInfo);

        [DllImport(dllPath)]
        public static extern int SendChatMessage(ref ChatMessageWrapper messageInfo);

        [DllImport(dllPath)]
        public static extern int SendReport(int titleId,
        IntPtr privateKey, int privateKeySize, ref ReportInfoWrapper report);

        [DllImport(dllPath)]
        public static extern int UpdatePlayer(int titleId,
        IntPtr privateKey, int privateKeySize, ref PlayerInfoWrapper report);

        [DllImport(dllPath)]
        public static extern void dispose();

    }
}
