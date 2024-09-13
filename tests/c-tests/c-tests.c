#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "GetgudSDK_C.h"

// Define titleId and privateKey globally
const int titleId = 219;
const char* privateKey = "";

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
    gameInfo.titleId = titleId;  // Use the defined titleId
    gameInfo.privateKey = privateKey;  // Use the defined privateKey
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

    if (!UpdatePlayer(titleId, privateKey, strlen(privateKey), playerInfo)) {
        fprintf(stderr, "Failed to update player\n");
        return;
    }
    printf("Player info updated!\n");

    // Send in-match report with correctly defined fields
    struct ReportInfo reportInfo;
    reportInfo.matchGuid = matchGuid;
    reportInfo.matchGuidSize = strlen(matchGuid);
    reportInfo.reporterName = "reporter1";
    reportInfo.reporterNameSize = strlen(reportInfo.reporterName);
    reportInfo.reporterType = 1;  // Player (based on your enum definition)
    reportInfo.reporterSubType = -1;  // None (or provide a subtype if available)
    reportInfo.suspectedPlayerGuid = "player2";
    reportInfo.suspectedPlayerGuidSize = strlen(reportInfo.suspectedPlayerGuid);
    reportInfo.tbType = 1;  // Aimbot (based on your enum definition)
    reportInfo.tbTimeEpoch = (long long)time(NULL) * 1000;  // Set a valid time
    reportInfo.suggestedToxicityScore = -1;  // Optional: -1 if not suggested
    reportInfo.reportedTimeEpoch = (long long)time(NULL) * 1000;  // Set the report time

    if (!SendInMatchReport(reportInfo)) {
        fprintf(stderr, "Failed to send in-match report\n");
        return;
    }
    printf("In-match report sent!\n");

    // End game
    if (!MarkEndGame(gameGuid, strlen(gameGuid))) {
        fprintf(stderr, "Failed to mark game as ended\n");
        return;
    }
    printf("Game marked as ended!\n");

    // Send report outside the match
    struct ReportInfo reportOutsideMatch;
    reportOutsideMatch.matchGuid = gameGuid;  // Use gameGuid for outside match report
    reportOutsideMatch.matchGuidSize = strlen(gameGuid);
    reportOutsideMatch.reporterName = "reporter2";
    reportOutsideMatch.reporterNameSize = strlen(reportOutsideMatch.reporterName);
    reportOutsideMatch.reporterType = 2;  // Moderator
    reportOutsideMatch.reporterSubType = -1;  // None (or provide a subtype if available)
    reportOutsideMatch.suspectedPlayerGuid = "player3";
    reportOutsideMatch.suspectedPlayerGuidSize = strlen(reportOutsideMatch.suspectedPlayerGuid);
    reportOutsideMatch.tbType = 16;  // Insulting
    reportOutsideMatch.tbTimeEpoch = (long long)time(NULL) * 1000;  // Set the toxic behavior time
    reportOutsideMatch.suggestedToxicityScore = -1;  // Optional: -1 if not suggested
    reportOutsideMatch.reportedTimeEpoch = (long long)time(NULL) * 1000;  // Set the report time

    if (!SendReport(titleId, privateKey, strlen(privateKey), reportOutsideMatch)) {
        fprintf(stderr, "Failed to send report outside match\n");
        return;
    }
    printf("Report sent outside match!\n");

    // Dispose SDK
    dispose();
    printf("SDK disposed and test passed!\n");
}

int main() {
    srand((unsigned int)time(NULL)); // Seed for random float generation
    testGetgudSDK();
    return 0;
}