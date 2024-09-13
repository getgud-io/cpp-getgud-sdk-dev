using System;
using GetgudSDK;
using System.Collections.Generic;

class Program
{
    static void Main(string[] args)
    {
        // Initialize SDK
        string configPath = "C:\\Users\\Shadow\\Downloads\\Getgud\\cpp-getgud-sdk-dev\\tests\\csharp-tests\\config.json";
        int result = Methods.InitConfPath(configPath);
        if (result != 1)
        {
            Console.WriteLine("Failed to initialize Getgud SDK.");
            return;
        }

        // Start a game
        StartGameInfo gameInfo = new StartGameInfo
        {
            TitleId = 219,
            PrivateKey = "12bebc20-6aa6-11ef-b599-37b14b47bb25",
            ServerGuid = "server-123",
            GameMode = "deathmatch",
            ServerLocation = "US-West"
        };

        string gameGuid;
        if (Methods.StartGame(gameInfo, out gameGuid) == 0)
        {
            Console.WriteLine("Failed to start game.");
            Methods.dispose();
            return;
        }
        Console.WriteLine($"Game started with GUID: {gameGuid}");

        // Start a match
        StartMatchInfo matchInfo = new StartMatchInfo
        {
            gameGuid = gameGuid,
            matchMode = "5v5",
            mapName = "de_dust2",
            customField = ""
        };

        string matchGuid;
        if (Methods.StartMatch(matchInfo, out matchGuid) == 0)
        {
            Console.WriteLine("Failed to start match.");
            Methods.dispose();
            return;
        }
        Console.WriteLine($"Match started with GUID: {matchGuid}");

        // Simulate sending player actions and reporting toxic behavior
        BaseActionData baseAction = new BaseActionData
        {
            actionTimeEpoch = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds(),
            matchGuid = matchGuid,
            playerGuid = "player1"
        };

        SendSpawnActionInfo spawnAction = new SendSpawnActionInfo
        {
            baseData = baseAction,
            characterGuid = "character1",
            teamGuid = "team1",
            initialHealth = 100.0f,
            position = new PositionF { X = 10.0f, Y = 20.0f, Z = 30.0f },
            rotation = new RotationF { Yaw = 45.0f, Pitch = 0.0f, Roll = 0.0f }
        };
        Methods.SendSpawnAction(spawnAction);

        // Report toxic behavior during the match
        ReportInfo matchReport = new ReportInfo
        {
            matchGuid = matchGuid,
            reporterName = "reporter1",
            reporterType = 1, // Player
            reporterSubType = -1,
            suspectedPlayerGuid = "player2",
            tbType = 1, // Aimbot
            tbTimeEpoch = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds(),
            suggestedToxicityScore = -1,
            reportedTimeEpoch = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()
        };
        Methods.SendInMatchReport(matchReport);
        Console.WriteLine("In-match report sent!");

        // Update player information
        PlayerInfo playerInfo = new PlayerInfo
        {
            playerGuid = "player1",
            playerNickname = "TestPlayer",
            playerEmail = "test@example.com",
            playerRank = 10,
            playerJoinDateEpoch = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds(),
            playerSuspectScore = "",
            playerReputation = "",
            playerStatus = "active",
            playerCampaign = "TestCampaign",
            playerNotes = "Test Notes",
            playerDevice = "PC",
            playerOS = "Windows",
            playerAge = 25,
            playerGender = "Male",
            playerLocation = "US"
        };

        result = Methods.UpdatePlayer(219, "12bebc20-6aa6-11ef-b599-37b14b47bb25", playerInfo);
        if (result == 1)
        {
            Console.WriteLine("Player info updated!");
        }
        else
        {
            Console.WriteLine("Failed to update player info.");
        }

        // End the game
        Methods.MarkEndGame(gameGuid);
        Console.WriteLine("Game marked as ended!");

        // Report toxic behavior outside the match
        ReportInfo outsideMatchReport = new ReportInfo
        {
            matchGuid = "outside-match-guid",
            reporterName = "outside-reporter",
            reporterType = 1, // Player
            reporterSubType = -1,
            suspectedPlayerGuid = "suspectedPlayerGuid",
            tbType = 1, // Aimbot
            tbTimeEpoch = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds(),
            suggestedToxicityScore = -1,
            reportedTimeEpoch = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()
        };

        result = Methods.SendReport(219, "12bebc20-6aa6-11ef-b599-37b14b47bb25", outsideMatchReport);
        if (result == 1)
        {
            Console.WriteLine("Outside match report sent!");
        }
        else
        {
            Console.WriteLine("Failed to send outside match report.");
        }

        // Clean up SDK resources
        Methods.dispose();
        Console.WriteLine("SDK disposed.");
    }
}