using GetGudSdk;
using System.Runtime.InteropServices;

class Program
{
    static void Main(string[] args)
    {
        int status = GetGudSdk.Methods.Init();
        Console.WriteLine(status);

        string gameGuid;

        GetGudSdk.StartGameInfo gameInfo;
        gameInfo.ServerGuid = "123";
        gameInfo.PrivateKey = "8526b010-0c56-11ee-bc1d-9f343a78df6b";
        gameInfo.TitleId = 30;
        gameInfo.GameMode = "gameMode";

        status = GetGudSdk.Methods.StartGame(gameInfo, out gameGuid);
        if (status > 0)
            Console.WriteLine(gameGuid);

        string matchGuid;

        GetGudSdk.StartMatchInfo matchInfo;
        matchInfo.gameGuid = gameGuid;
        matchInfo.mapName = "map";
        matchInfo.matchMode = "matchMode";

        status = GetGudSdk.Methods.StartMatch(matchInfo, out matchGuid);
        if (status > 0)
            Console.WriteLine(matchGuid);

        GetGudSdk.SendSpawnActionInfo SpawnInfo;
        SpawnInfo.baseData.matchGuid = matchGuid;
        SpawnInfo.baseData.playerGuid = "player-1";
        SpawnInfo.baseData.actionTimeEpoch = 1684059337532;
        SpawnInfo.characterGuid = "LL";
        SpawnInfo.initialHealth = 100;
        SpawnInfo.teamId = 1;
        SpawnInfo.position = new GetGudSdk.PositionF{ X = 10F, Y = 10F, Z = 0.0001F };
        SpawnInfo.rotation = new GetGudSdk.RotationF{ Roll = 240F, Pitch = 180F, Yaw = 0F };

        GetGudSdk.Methods.SendSpawnAction(SpawnInfo);

        SpawnInfo.characterGuid = "DD";
        SpawnInfo.baseData.playerGuid = "player-2";

        GetGudSdk.Methods.SendSpawnAction(SpawnInfo);

        GetGudSdk.SendAttackActionInfo AttackInfo;
        AttackInfo.baseData.matchGuid = matchGuid;
        AttackInfo.baseData.playerGuid = "dsafear";
        AttackInfo.baseData.actionTimeEpoch = 1684059337532;
        AttackInfo.weaponGuid = "G2";
        GetGudSdk.Methods.SendAttackAction(AttackInfo);

        GetGudSdk.BaseActionData DeathInfo;
        DeathInfo.matchGuid = matchGuid;
        DeathInfo.playerGuid = "player-2";
        DeathInfo.actionTimeEpoch = 1684059337532;
        GetGudSdk.Methods.SendDeathAction(DeathInfo);

        GetGudSdk.Methods.MarkEndGame(gameGuid);

        while(true)
        {

        }
    }
}