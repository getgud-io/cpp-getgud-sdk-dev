using GetGudSdk;
using System.Runtime.InteropServices;

class Program
{
    static void Main(string[] args)
    {
        // Initialize the SDK:
        int status = GetGudSdk.Methods.Init();
        Console.WriteLine(status);
        
        // Initialize input and output data for a Game
        string gameGuid;

        GetGudSdk.StartGameInfo gameInfo;
        gameInfo.ServerGuid = "123";
        gameInfo.PrivateKey = "8526b010-0c56-11ee-bc1d-9f343a78df6b";
        gameInfo.TitleId = 30;
        gameInfo.GameMode = "gameMode";

        // Start a Game:
        status = GetGudSdk.Methods.StartGame(gameInfo, out gameGuid);
        if (status > 0)
            Console.WriteLine(gameGuid);

        // Initialize input and output data for a Match
        string matchGuid;

        GetGudSdk.StartMatchInfo matchInfo;
        matchInfo.gameGuid = gameGuid;
        matchInfo.mapName = "map";
        matchInfo.matchMode = "matchMode";

        // Start a Match of the Game:
        status = GetGudSdk.Methods.StartMatch(matchInfo, out matchGuid);
        if (status > 0)
            Console.WriteLine(matchGuid);

        // Once you have a match, you can send Actions to it. Let's create a vector of
        // Actions and send it to the match:

        // Initialize input data for an Action
        GetGudSdk.SendSpawnActionInfo SpawnInfo;
        SpawnInfo.baseData.matchGuid = matchGuid;
        SpawnInfo.baseData.playerGuid = "player-1";
        SpawnInfo.baseData.actionTimeEpoch = 1684059337532;
        SpawnInfo.characterGuid = "LL";
        SpawnInfo.initialHealth = 100;
        SpawnInfo.teamId = 1;
        SpawnInfo.position = new GetGudSdk.PositionF{ X = 10F, Y = 10F, Z = 0.0001F };
        SpawnInfo.rotation = new GetGudSdk.RotationF{ Roll = 240F, Pitch = 180F, Yaw = 0F };

        // Send the action to the SDK:
        GetGudSdk.Methods.SendSpawnAction(SpawnInfo);

        // Change the data for a new player:
        SpawnInfo.characterGuid = "DD";
        SpawnInfo.baseData.playerGuid = "player-2";

        // Send the updated action to the SDK:
        GetGudSdk.Methods.SendSpawnAction(SpawnInfo);

        // Emulate a little behavior:
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

        GetGudSdk.ReportInfo reportInfo = new GetGudSdk.ReportInfo(matchGuid, 1684059337532, "player-2");

        // Make a report about the player:
        GetGudSdk.Methods.SendReport(0, // Take the title id from the environment
            "", // Take the private key from the environment
            reportInfo);

        GetGudSdk.PlayerInfo playerInfo = new PlayerInfo("player-2");
        GetGudSdk.Methods.UpdatePlayer(0, // Take the title id from the environment
            "", // Take the private key from the environment
            playerInfo);

        // End a game (All Game's Matches will close as well):
        GetGudSdk.Methods.MarkEndGame(gameGuid);

        //...
        // Continue to work with games and matches
        //...

        // Wait until packets will be sent.
        // The wait timeout is based on the config parameters
        Thread.Sleep(4000);

        // Close and Dispose of the SDK:
        GetGudSdk.Methods.dispose();

        while (true)
        {

        }
    }
}