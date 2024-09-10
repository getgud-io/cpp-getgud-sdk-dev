#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "GetgudSDK_C.h"

// Helper function to generate random float
float randomFloat(float min, float max) {
    return min + ((float)rand() / RAND_MAX) * (max - min);
}

void testGetgudSDK() {
    // Initialize SDK
    if (!init()) {
        fprintf(stderr, "SDK Initialization failed\n");
        return;
    }

    // Start Game
    char gameGuid[64] = {0};
    struct StartGameInfo gameInfo;
    gameInfo.titleId = 219;
    gameInfo.privateKey = "12bebc20-6aa6-11ef-b599-37b14b47bb25";
    gameInfo.privateKeySize = strlen(gameInfo.privateKey);
    gameInfo.serverGuid = "server-123";
    gameInfo.serverGuidSize = strlen(gameInfo.serverGuid);
    gameInfo.gameMode = "deathmatch";
    gameInfo.gameModeSize = strlen(gameInfo.gameMode);
    gameInfo.serverLocation = "US-West";
    gameInfo.serverLocationSize = strlen(gameInfo.serverLocation);

    if (!StartGame(gameInfo, gameGuid)) {
        fprintf(stderr, "Failed to start the game\n");
        return;
    }
    printf("Game started with GUID: %s\n", gameGuid);

    // Start Match
    char matchGuid[64] = {0};
    struct StartMatchInfo matchInfo;
    matchInfo.gameGuid = gameGuid;
    matchInfo.gameGuidSize = strlen(gameGuid);
    matchInfo.matchMode = "5v5";
    matchInfo.matchModeSize = strlen(matchInfo.matchMode);
    matchInfo.mapName = "de_dust2";
    matchInfo.mapNameSize = strlen(matchInfo.mapName);
    matchInfo.customField = NULL;
    matchInfo.customFieldSize = 0;

    if (!StartMatch(matchInfo, matchGuid)) {
        fprintf(stderr, "Failed to start the match\n");
        return;
    }
    printf("Match started with GUID: %s\n", matchGuid);

    // Spawn players and perform actions
    char* playerIds[] = {"player1", "player2", "player3", "player4"};
    for (int i = 0; i < 4; ++i) {
        struct PositionF position;
        position.X = randomFloat(0, 100);
        position.Y = randomFloat(0, 100);
        position.Z = randomFloat(0, 100);

        struct RotationF rotation;
        rotation.Yaw = randomFloat(0, 360);
        rotation.Pitch = randomFloat(-90, 90);
        rotation.Roll = 0;

        // Base action data
        struct BaseActionData actionData;
        actionData.actionTimeEpoch = (long long)time(NULL) * 1000;
        actionData.matchGuid = matchGuid;
        actionData.matchGuidSize = strlen(matchGuid);
        actionData.playerGuid = playerIds[i];
        actionData.playerGuidSize = strlen(playerIds[i]);

        // Spawn action
        if (!SendSpawnAction(actionData, "character1", strlen("character1"), "team1", strlen("team1"), 100.0f, position, rotation)) {
            fprintf(stderr, "Failed to send spawn action for player %s\n", playerIds[i]);
            return;
        }

        // Position action
        if (!SendPositionAction(actionData, position, rotation)) {
            fprintf(stderr, "Failed to send position action for player %s\n", playerIds[i]);
            return;
        }

        // Attack action
        if (!SendAttackAction(actionData, "weapon1", strlen("weapon1"))) {
            fprintf(stderr, "Failed to send attack action for player %s\n", playerIds[i]);
            return;
        }

        usleep(100 * 1000); // Sleep for 100 milliseconds
    }
    printf("All actions sent!\n");

    // Update player info
    struct PlayerInfo playerInfo;
    playerInfo.playerGuid = "player1";
    playerInfo.playerGuidSize = strlen(playerInfo.playerGuid);
    playerInfo.playerNickname = "TestPlayer";
    playerInfo.playerNicknameSize = strlen(playerInfo.playerNickname);
    playerInfo.playerEmail = "test@example.com";
    playerInfo.playerEmailSize = strlen(playerInfo.playerEmail);
    playerInfo.playerRank = 10;
    playerInfo.playerJoinDateEpoch = (long long)time(NULL) * 1000;
    playerInfo.transactions = NULL;
    playerInfo.transactionsSize = 0;

    if (!UpdatePlayer(219, "12bebc20-6aa6-11ef-b599-37b14b47bb25", strlen("12bebc20-6aa6-11ef-b599-37b14b47bb25"), playerInfo)) {
        fprintf(stderr, "Failed to update player\n");
        return;
    }
    printf("Player info updated!\n");

    // Send in-match report
    struct ReportInfo reportInfo;
    reportInfo.matchGuid = matchGuid;
    reportInfo.matchGuidSize = strlen(matchGuid);
    reportInfo.reporterName = "reporter1";
    reportInfo.reporterNameSize = strlen(reportInfo.reporterName);
    reportInfo.reporterType = 0; // Assuming 0 for player type
    reportInfo.suspectedPlayerGuid = "player2";
    reportInfo.suspectedPlayerGuidSize = strlen(reportInfo.suspectedPlayerGuid);
    reportInfo.tbType = 0; // Assuming 0 for aimbot
    reportInfo.reportedTimeEpoch = (long long)time(NULL) * 1000;

    if (!SendInMatchReport(reportInfo)) {
        fprintf(stderr, "Failed to send in-match report\n");
        return;
    }
    printf("Report sent!\n");

    // End game
    if (!MarkEndGame(gameGuid, strlen(gameGuid))) {
        fprintf(stderr, "Failed to mark game as ended\n");
        return;
    }
    printf("Game marked as ended!\n");

    // Dispose SDK
    dispose();
    printf("SDK disposed and test passed!\n");
}

int main() {
    srand((unsigned int)time(NULL)); // Seed for random float generation
    testGetgudSDK();
    return 0;
}