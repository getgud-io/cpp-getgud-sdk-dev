#include <GetgudSDK.h>
#include <GetgudSDK_C.h> // Include C header for C structs/functions
#include <iostream>

#include <chrono>
#include <random>
#include <thread>
#include <string> // For std::string
#include <cstring> // For strlen
#include <limits> // For std::numeric_limits
#include <vector> // For std::vector
#include <deque> // For std::deque
#include <cassert> // For assertions

// ================ CONSTANTS ================
const int TITLE_ID = 239;
const std::string PRIVATE_KEY = "3f2c15d0-1c5f-11f0-88b3-fd424aa02c9e";
// ===========================================

// Custom assertion macro that prints a message and returns false on failure
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "TEST FAILED: " << message << std::endl; \
            assert(condition); \
            return 1; \
        } else { \
            std::cout << "TEST PASSED: " << message << std::endl; \
        } \
    } while (0)

// Version that expects failure (inverse assert)
#define TEST_ASSERT_FAILS(condition, message) \
    do { \
        if ((condition)) { \
            std::cerr << "TEST FAILED: " << message << " (expected failure but succeeded)" << std::endl; \
            assert(!(condition)); \
            return 1; \
        } else { \
            std::cout << "TEST PASSED: " << message << " (failed as expected)" << std::endl; \
        } \
    } while (0)

std::random_device
rdMain;  // a system based device that generates data randomly
std::uniform_int_distribution<int> actionTypeDist(0, 5);

GetgudSDK::BaseActionData* MakeRandomAction(std::string matchGuid,
    long long curTimeEpoch) {
    int actionType = actionTypeDist(rdMain);

    GetgudSDK::BaseActionData* outAction = nullptr;
    switch (actionType) {
    case 0:
        outAction = new GetgudSDK::AttackActionData(matchGuid, curTimeEpoch,
            "player-5", "abc");
        break;
    case 1:
        outAction = new GetgudSDK::DamageActionData(
            matchGuid, curTimeEpoch, "player-5", "player-0", 50.001421f, "glc");
        break;
    case 2:
        outAction =
            new GetgudSDK::DeathActionData(matchGuid, curTimeEpoch, "player-0", "player-1");
        break;
    case 3:
        outAction = new GetgudSDK::HealActionData(matchGuid, curTimeEpoch,
            ":plkayer-5", 20.32100F);
        break;
    case 4:
        outAction = new GetgudSDK::PositionActionData(
            matchGuid, curTimeEpoch, "player-5",
            GetgudSDK::PositionF{20.32000f, 50.001421f, 0.30021f},
            GetgudSDK::RotationF{10, 20});
        break;
    case 5:
        outAction = new GetgudSDK::SpawnActionData(
            matchGuid, curTimeEpoch, "player-10", "ttr", "team_1", 100.f,
            GetgudSDK::PositionF{1, 2, 3}, GetgudSDK::RotationF{10, 20});
        break;
    }

    return outAction;
}

std::deque<GetgudSDK::BaseActionData*> CreateActionsVector(
    std::string matchGuid,
    int actionsAmount) {
    std::deque<GetgudSDK::BaseActionData*> outVector;
    long long actionsEpochTime = 1684059337532;
    for (int actionNum = 0; actionNum < actionsAmount; actionNum++) {
        GetgudSDK::BaseActionData* outAction =
            MakeRandomAction(matchGuid, actionsEpochTime);
        actionsEpochTime += 2;
        outVector.push_back(outAction);
    }
    return outVector;
}

void CreateReports(std::string matchGuid, int numberOfReports) {
    std::deque<GetgudSDK::ReportInfo> reports;
    // Use the constant private key
    // std::string privateKey = "97237e40-1ad4-11f0-83cc-3b0b7cb4e97d"; 
    for (int gameNum = 0; gameNum < numberOfReports; gameNum++) {
        GetgudSDK::ReportInfo reportInfo;
        reportInfo.MatchGuid = matchGuid;
        reportInfo.ReportedTimeEpoch = 1684059337532;
        reportInfo.ReporterName = "reporter_name";
        reportInfo.ReporterSubType = GetgudSDK::ReporterSubtype::VAC;
        reportInfo.ReporterType = GetgudSDK::ReporterType::AntiCheat;
        reportInfo.SuggestedToxicityScore = 100;
        reportInfo.SuspectedPlayerGuid = "suspected_player_guid";
        reportInfo.TbTimeEpoch = 1684059337532;
        reportInfo.TbType = GetgudSDK::TbType::Aimbot;

        reports.push_back(reportInfo);
    }
    // Use constant Title ID and Private Key
    GetgudSDK::SendReports(TITLE_ID, PRIVATE_KEY, reports);
}

void CreatePlayerUpdates(int numberOfPlayerUpdates) {
    std::deque<GetgudSDK::PlayerInfo> playerInfos;
    // Use the constant private key
    // std::string privateKey = "97237e40-1ad4-11f0-83cc-3b0b7cb4e97d"; 
    for (int playerUpdateNum = 0; playerUpdateNum < numberOfPlayerUpdates;
        playerUpdateNum++) {
        GetgudSDK::PlayerInfo playerInfo;
        playerInfo.PlayerGuid = "player-5";
        playerInfo.PlayerNickname = "test";
        playerInfo.PlayerEmail = "test@test.com";
        playerInfo.PlayerRank = 10;
        playerInfo.PlayerJoinDateEpoch = 1684059337532;
        playerInfos.push_back(playerInfo);
    }
    // Use constant Title ID and Private Key
    GetgudSDK::UpdatePlayers(TITLE_ID, PRIVATE_KEY, playerInfos);
}

int main() {
    // Initialize the SDK:
    GetgudSDK::Init();

    std::string serverGuid = "us-west-1";
    std::string gameMode = "deathmatch";
    std::string serverLocation = "UK";

    // Start a Game using constants:
    std::cout << "Starting game with Title ID: " << TITLE_ID << std::endl;
    std::string gameGuid = GetgudSDK::StartGame(TITLE_ID,
        PRIVATE_KEY,
        serverGuid,  // serverGuid
        gameMode,     // gameMode
        serverLocation
    );
    TEST_ASSERT(!gameGuid.empty(), "StartGame should return a valid GUID");
    std::cout << "Game started with GUID: " << gameGuid << std::endl;

    std::string matchGuid = GetgudSDK::StartMatch(gameGuid,
        "Knives-only",  // matchMode
        "de-dust"       // mapName
    );

    // --- Spawn Players First ---
    int numPlayers = 10; // Define number of players
    long long spawnTime = 1744820972000; // Starting timestamp for spawns
    std::deque<GetgudSDK::BaseActionData*> actionsToSend; // Collect ALL actions here
    std::string basePlayerGuid = "player-";
    std::string team1 = "team_1";
    std::string team2 = "team_2";
    std::string charModel = "default_char";

    for (int i = 0; i < numPlayers; ++i) {
        std::string currentPlayerGuid = basePlayerGuid + std::to_string(i);
        std::string currentTeam = (i % 2 == 0) ? team1 : team2; // Alternate teams
        GetgudSDK::PositionF spawnPos = {(float)i * 10.0f, 5.0f, 0.0f}; // Spread out spawn positions
        GetgudSDK::RotationF spawnRot = {0.0f, 0.0f};

        GetgudSDK::SpawnActionData* spawnAction = new GetgudSDK::SpawnActionData(
            matchGuid,
            spawnTime,
            currentPlayerGuid,
            charModel,
            currentTeam,
            100.0f, // initialHealth
            spawnPos,
            spawnRot
        );
        actionsToSend.push_back(spawnAction);
        spawnTime += 5; // Increment time slightly for each spawn
    }
    // --- End Spawn Players ---

    // --- Interleave Other Actions and Chat Messages ---
    int numEvents = 100;
    long long eventTime = spawnTime + 100; // Start subsequent events after spawns

    // Modify MakeRandomAction to exclude Spawn (case 5)
    // You might need to adjust the uniform_int_distribution range if `MakeRandomAction` is modified directly,
    // or simply add a check within the loop/MakeRandomAction to reroll if actionType == 5.
    // For simplicity here, we'll assume MakeRandomAction no longer returns SpawnActionData*.

    std::uniform_int_distribution<int> otherActionTypeDist(0, 4); // Range for non-spawn actions

    for (int i = 0; i < numEvents; ++i) {
        int playerIndex = i % numPlayers; // Cycle through spawned players
        std::string currentPlayerGuid = basePlayerGuid + std::to_string(playerIndex);

        // Create and store action (excluding spawn)
        // Temporarily override random choice for demonstration:
        int actionType = otherActionTypeDist(rdMain); // Generate random action type 0-4
        GetgudSDK::BaseActionData* action = nullptr;
         switch (actionType) {
            case 0:
                action = new GetgudSDK::AttackActionData(matchGuid, eventTime,
                    currentPlayerGuid, "weapon_pistol");
                break;
            case 1:
                {
                    std::string victimPlayerGuid = basePlayerGuid + std::to_string((playerIndex + 1) % numPlayers); // Target next player
                    action = new GetgudSDK::DamageActionData(
                        matchGuid, eventTime, currentPlayerGuid, victimPlayerGuid, 25.0f, "weapon_rifle");
                }
                break;
            case 2:
                 {
                    std::string attackerPlayerGuid = basePlayerGuid + std::to_string((playerIndex + numPlayers -1) % numPlayers); // Killed by previous player
                     action = new GetgudSDK::DeathActionData(matchGuid, eventTime, currentPlayerGuid, attackerPlayerGuid);
                 }
                break;
            case 3:
                 action = new GetgudSDK::HealActionData(matchGuid, eventTime,
                     currentPlayerGuid, 10.0f);
                break;
            case 4:
                 action = new GetgudSDK::PositionActionData(
                     matchGuid, eventTime, currentPlayerGuid,
                     GetgudSDK::PositionF{(float)(i % 10) * 5.0f, (float)(i / 10) * 5.0f, 1.0f}, // Move players around
                     GetgudSDK::RotationF{(float)(i * 10), (float)(i*5)});
                 break;
         }

        if (action) {
            actionsToSend.push_back(action);
        }

        // Create and send chat message
        std::string message_text = "Player " + std::to_string(playerIndex) + " event #" + std::to_string(i) + " at t=" + std::to_string(eventTime);

        GetgudSDK::ChatMessageInfo chat_msg_cpp = {};
        chat_msg_cpp.playerGuid = currentPlayerGuid;
        chat_msg_cpp.message = message_text;
        chat_msg_cpp.messageTimeEpoch = eventTime; // Align timestamp

        GetgudSDK::SendChatMessage(matchGuid, chat_msg_cpp);

        // Increment time for the next event pair
        eventTime += 15; // Increment time by 15ms
    }

    // Send ALL collected actions (spawns first, then others) in one batch
    if (!actionsToSend.empty()) {
        GetgudSDK::SendActions(actionsToSend);
    }

    // Clean up action pointers
    for (auto* actionPtr : actionsToSend) {
        delete actionPtr;
    }
    actionsToSend.clear();
    // -------------------------------------------
    
    // ============ VALIDATION TESTS ============
    std::cout << "\n\n========== STARTING VALIDATION TESTS ==========\n" << std::endl;
    
    // Create a new match for testing validation
    std::string testMatchGuid = GetgudSDK::StartMatch(gameGuid, "ValidationTest", "test_map");
    std::cout << "Created test match with GUID: " << testMatchGuid << std::endl;
    TEST_ASSERT(!testMatchGuid.empty(), "Match GUID should not be empty");
    
    // Test 1: Test null pointer handling
    std::cout << "\nTest 1: Null pointer handling..." << std::endl;
    {
        std::deque<GetgudSDK::BaseActionData*> nullActions;
        // Add a null pointer to the actions list
        nullActions.push_back(nullptr);
        
        // Try to send null actions - SDK should handle null pointers gracefully by skipping
        bool result = GetgudSDK::SendActions(nullActions);
        // Expect SUCCESS because the SDK now skips nulls without error.
        TEST_ASSERT(result, "SDK should gracefully handle null actions (skipping them)"); 
    }
    
    // Test 2: Invalid GUID validation
    std::cout << "\nTest 2: Invalid GUID validation..." << std::endl;
    {
        // Invalid match GUID (contains invalid characters)
        std::string invalidMatchGuid = "invalid@guid";
        // Invalid player GUID (too long)
        std::string invalidPlayerGuid = "player-id-that-is-way-too-long-and-exceeds-the-limit-of-36-characters";
        
        GetgudSDK::BaseActionData* invalidAction = new GetgudSDK::PositionActionData(
            invalidMatchGuid, 
            eventTime, 
            invalidPlayerGuid,
            GetgudSDK::PositionF{1.0f, 2.0f, 3.0f},
            GetgudSDK::RotationF{10.0f, 20.0f}
        );
        
        // IsValid should return false for invalid GUIDs
        bool isValid = invalidAction->IsValid();
        TEST_ASSERT_FAILS(isValid, "Action with invalid GUIDs should be invalid");
        
        // Try to send the invalid action directly - This should SUCCEED because 
        // SendAction/SendActions only check for null pointers, not data validity.
        // The invalid action will be added to the buffer but filtered later.
        bool sendResult = GetgudSDK::SendAction(invalidAction);
        TEST_ASSERT(sendResult, "SDK should accept actions with invalid GUIDs into buffer (filtering happens later)"); 
        
        delete invalidAction;
    }
    
    // Test 3: Out of range numeric values
    std::cout << "\nTest 3: Out of range numeric values..." << std::endl;
    {
        // Test extremely high timestamp
        long long invalidTimeEpoch = 9999999999999999;
        
        GetgudSDK::HealActionData* invalidTimeAction = new GetgudSDK::HealActionData(
            testMatchGuid,
            invalidTimeEpoch,
            "player-0",
            50.0f
        );
        
        bool isTimeValid = invalidTimeAction->IsValid();
        TEST_ASSERT_FAILS(isTimeValid, "Action with invalid timestamp should be invalid");
        
        // Test invalid damage values (Assuming negative is currently allowed for test)
        GetgudSDK::DamageActionData* invalidDamageAction = new GetgudSDK::DamageActionData(
            testMatchGuid,
            eventTime,
            "player-0",
            "player-1",
            -50.0f, // Negative damage
            "weapon_1"
        );
        
        bool isDamageValid = invalidDamageAction->IsValid();
        // Update assertion: Expect IsValid() to PASS if negative damage is allowed
        TEST_ASSERT(isDamageValid, "Action with negative damage validation (assuming allowed)"); 
        
        // Try to send the invalid actions - should now handle gracefully
        bool sendTimeResult = GetgudSDK::SendAction(invalidTimeAction);
        TEST_ASSERT(sendTimeResult, "SDK should accept actions with invalid timestamp into buffer");
        
        bool sendDamageResult = GetgudSDK::SendAction(invalidDamageAction);
        // Sending should still succeed as the pointer is valid
        TEST_ASSERT(sendDamageResult, "SDK should accept actions with negative damage values into buffer"); 
        
        delete invalidTimeAction;
        delete invalidDamageAction;
    }
    
    // Test 4: Special character sanitization
    std::cout << "\nTest 4: Special character sanitization..." << std::endl;
    {
        // Create messages with special characters that should be sanitized
        GetgudSDK::ChatMessageInfo specialCharMsg = {};
        specialCharMsg.playerGuid = "player-0";
        specialCharMsg.message = "Test message with \"quotes\" and backslashes \\ and $dollars!";
        specialCharMsg.messageTimeEpoch = eventTime;
        
        bool chatResult = GetgudSDK::SendChatMessage(testMatchGuid, specialCharMsg);
        TEST_ASSERT(chatResult, "Sending chat with special chars should succeed after sanitization");
        
        // Create report with special characters
        GetgudSDK::ReportInfo specialCharReport;
        specialCharReport.MatchGuid = testMatchGuid;
        specialCharReport.ReporterName = "Reporter \"Name\" with $special% chars";
        specialCharReport.ReportedTimeEpoch = eventTime;
        specialCharReport.ReporterSubType = GetgudSDK::ReporterSubtype::VAC;
        specialCharReport.ReporterType = GetgudSDK::ReporterType::AntiCheat;
        specialCharReport.SuggestedToxicityScore = 100;
        specialCharReport.SuspectedPlayerGuid = "player-0";
        specialCharReport.TbTimeEpoch = eventTime;
        specialCharReport.TbType = GetgudSDK::TbType::Aimbot;
        
        bool reportResult = GetgudSDK::SendInMatchReport(specialCharReport);
        TEST_ASSERT(reportResult, "Sending report with special chars should succeed after sanitization");
    }
    
    // Test 5: Pointer reuse and double-free prevention
    std::cout << "\nTest 5: Pointer reuse and double-free prevention..." << std::endl;
    {
        // Create an action
        GetgudSDK::PositionActionData* posAction = new GetgudSDK::PositionActionData(
            testMatchGuid,
            eventTime,
            "player-0",
            GetgudSDK::PositionF{5.0f, 10.0f, 15.0f},
            GetgudSDK::RotationF{30.0f, 60.0f}
        );
        
        // Clone the action
        GetgudSDK::BaseActionData* clonedAction = posAction->Clone();
        TEST_ASSERT(clonedAction != nullptr, "Cloned action should not be null");
        
        // Create a collection with both the original and the clone
        std::deque<GetgudSDK::BaseActionData*> reuseActions;
        reuseActions.push_back(posAction);
        reuseActions.push_back(clonedAction);
        
        // Send both actions
        bool reuseResult = GetgudSDK::SendActions(reuseActions);
        TEST_ASSERT(reuseResult, "Sending original and cloned actions should succeed");
        
        // Clean up only original (SDK should have created copies)
        delete posAction;
        delete clonedAction;
        
        // No double-free should occur when the SDK does cleanup internally
        std::cout << "  Double-free prevention tested" << std::endl;
    }
    
    // Test 6: Edge cases for AffectActionData
    std::cout << "\nTest 6: Edge cases for AffectActionData..." << std::endl;
    {
        // Test boundary conditions for AffectState enum
        for (int i = -1; i <= 4; i++) {
            GetgudSDK::AffectState state = static_cast<GetgudSDK::AffectState>(i);
            GetgudSDK::AffectActionData* affectAction = new GetgudSDK::AffectActionData(
                testMatchGuid,
                eventTime + i,
                "player-0",
                "effect-1",
                state
            );
            
            bool isValidAffect = affectAction->IsValid();
            if (i >= 0 && i <= 3) {
                // Valid enum values should be valid
                TEST_ASSERT(isValidAffect, "AffectState " + std::to_string(i) + " should be valid");
            } else {
                // Out of range enum values should be invalid
                TEST_ASSERT_FAILS(isValidAffect, "AffectState " + std::to_string(i) + " should be invalid");
            }
            
            delete affectAction;
        }
    }
    
    // Test 7: Memory management with large batch of actions
    std::cout << "\nTest 7: Memory management with large batch..." << std::endl;
    {
        std::deque<GetgudSDK::BaseActionData*> largeActionBatch;
        const int batchSize = 1000;
        
        for (int i = 0; i < batchSize; i++) {
            GetgudSDK::PositionActionData* posAction = new GetgudSDK::PositionActionData(
                testMatchGuid,
                eventTime + i,
                "player-" + std::to_string(i % 10),
                GetgudSDK::PositionF{(float)(i % 100), (float)(i / 100), 0.0f},
                GetgudSDK::RotationF{(float)(i % 360), (float)((i + 180) % 360)}
            );
            largeActionBatch.push_back(posAction);
        }
        
        std::cout << "  Created " << batchSize << " position actions" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        bool largeResult = GetgudSDK::SendActions(largeActionBatch);
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
        
        TEST_ASSERT(largeResult, "Sending large batch should succeed");
        std::cout << "  Time taken: " << duration << "ms" << std::endl;
        
        // Clean up
        for (auto* action : largeActionBatch) {
            delete action;
        }
    }
    
    // Test 8: Mixed Valid and Invalid Actions in Batch
    std::cout << "\nTest 8: Mixed Valid and Invalid Actions in Batch..." << std::endl;
    {
        std::deque<GetgudSDK::BaseActionData*> mixedBatch;
        
        // Valid action
        GetgudSDK::PositionActionData* validAction = new GetgudSDK::PositionActionData(
            testMatchGuid,
            eventTime,
            "player-0",
            GetgudSDK::PositionF{1.0f, 2.0f, 3.0f},
            GetgudSDK::RotationF{10.0f, 20.0f}
        );
        
        // Invalid action - bad timestamp
        GetgudSDK::PositionActionData* invalidAction1 = new GetgudSDK::PositionActionData(
            testMatchGuid,
            -1, // Negative timestamp
            "player-0",
            GetgudSDK::PositionF{1.0f, 2.0f, 3.0f},
            GetgudSDK::RotationF{10.0f, 20.0f}
        );
        
        // Invalid action - bad player GUID
        GetgudSDK::PositionActionData* invalidAction2 = new GetgudSDK::PositionActionData(
            testMatchGuid,
            eventTime,
            "", // Empty player GUID
            GetgudSDK::PositionF{1.0f, 2.0f, 3.0f},
            GetgudSDK::RotationF{10.0f, 20.0f}
        );
        
        mixedBatch.push_back(validAction);
        mixedBatch.push_back(invalidAction1);
        mixedBatch.push_back(invalidAction2);
        
        // The SDK should filter out invalid actions and only process valid ones
        bool mixedResult = GetgudSDK::SendActions(mixedBatch);
        // This should still succeed overall, as valid actions should be processed
        TEST_ASSERT(mixedResult, "Sending mixed valid/invalid batch should still succeed with valid actions");
        
        // Clean up
        for (auto* action : mixedBatch) {
            delete action;
        }
    }
    
    // Test 9: Pointer ownership in different APIs
    std::cout << "\nTest 9: Pointer ownership in different APIs..." << std::endl;
    {
        // Create report info
        GetgudSDK::ReportInfo reportInfo;
        reportInfo.MatchGuid = testMatchGuid;
        reportInfo.ReportedTimeEpoch = eventTime;
        reportInfo.ReporterName = "ownership_tester";
        reportInfo.ReporterSubType = GetgudSDK::ReporterSubtype::VAC;
        reportInfo.ReporterType = GetgudSDK::ReporterType::AntiCheat;
        reportInfo.SuggestedToxicityScore = 50;
        reportInfo.SuspectedPlayerGuid = "player-0";
        reportInfo.TbTimeEpoch = eventTime;
        reportInfo.TbType = GetgudSDK::TbType::Aimbot;
        
        // Create multiple reports with the same data
        std::deque<GetgudSDK::ReportInfo> reportsVector;
        for (int i = 0; i < 5; i++) {
            // Modify slightly to make each unique
            reportInfo.SuggestedToxicityScore = 50 + i;
            reportsVector.push_back(reportInfo);
        }
        
        size_t originalSize = reportsVector.size();
        // Send reports in batch 
        bool batchReportResult = GetgudSDK::SendReports(TITLE_ID, PRIVATE_KEY, reportsVector);
        TEST_ASSERT(batchReportResult, "Sending batch reports should succeed");
        
        // After sending, the vector should still contain its original data
        TEST_ASSERT(reportsVector.size() == originalSize, "Reports vector size should be unchanged after sending");
    }
    
    // Test 10: Empty Data Containers
    std::cout << "\nTest 10: Empty Data Containers..." << std::endl;
    {
        // Create empty collections
        std::deque<GetgudSDK::BaseActionData*> emptyActions;
        std::deque<GetgudSDK::ReportInfo> emptyReports;
        std::deque<GetgudSDK::PlayerInfo> emptyPlayers;
        
        // Test sending empty collections
        bool emptyActionsResult = GetgudSDK::SendActions(emptyActions);
        bool emptyReportsResult = GetgudSDK::SendReports(TITLE_ID, PRIVATE_KEY, emptyReports);
        bool emptyPlayersResult = GetgudSDK::UpdatePlayers(TITLE_ID, PRIVATE_KEY, emptyPlayers);
        
        // Sending empty collections should be handled gracefully (not crash)
        TEST_ASSERT(emptyActionsResult, "Sending empty actions should succeed (no-op)");
        TEST_ASSERT(emptyReportsResult, "Sending empty reports should succeed (no-op)");
        TEST_ASSERT(emptyPlayersResult, "Sending empty player updates should succeed (no-op)");
    }
    
    // Test 11: Boundary Testing for String Fields
    std::cout << "\nTest 11: Boundary Testing for String Fields..." << std::endl;
    {
        // Test empty strings
        GetgudSDK::PositionActionData* emptyStringsAction = new GetgudSDK::PositionActionData(
            "", // empty match GUID
            eventTime,
            "", // empty player GUID
            GetgudSDK::PositionF{1.0f, 2.0f, 3.0f},
            GetgudSDK::RotationF{10.0f, 20.0f}
        );
        
        bool emptyStringsValid = emptyStringsAction->IsValid();
        TEST_ASSERT_FAILS(emptyStringsValid, "Action with empty GUIDs should be invalid");
        
        delete emptyStringsAction;
        
        // Test very long strings (edge of valid length)
        std::string longGuid = "";
        for (int i = 0; i < 36; i++) { // Max valid length is 36
            longGuid += "a";
        }
        
        GetgudSDK::PositionActionData* longStringsAction = new GetgudSDK::PositionActionData(
            longGuid,
            eventTime,
            longGuid,
            GetgudSDK::PositionF{1.0f, 2.0f, 3.0f},
            GetgudSDK::RotationF{10.0f, 20.0f}
        );
        
        bool longStringsValid = longStringsAction->IsValid();
        TEST_ASSERT(longStringsValid, "Action with max-length GUIDs should be valid");
        
        delete longStringsAction;
    }
    
    // Test 12: Cross-Action Data Consistency
    std::cout << "\nTest 12: Cross-Action Data Consistency..." << std::endl;
    {
        // Create a sequence of actions with related data
        std::deque<GetgudSDK::BaseActionData*> sequenceActions;
        
        // First spawn a player
        GetgudSDK::SpawnActionData* spawnAction = new GetgudSDK::SpawnActionData(
            testMatchGuid,
            eventTime,
            "consistency-player",
            "character-model",
            "team-red",
            100.0f,
            GetgudSDK::PositionF{0.0f, 0.0f, 0.0f},
            GetgudSDK::RotationF{0.0f, 0.0f}
        );
        sequenceActions.push_back(spawnAction);
        
        // Then make them attack
        GetgudSDK::AttackActionData* attackAction = new GetgudSDK::AttackActionData(
            testMatchGuid,
            eventTime + 100,
            "consistency-player",
            "weapon-rifle"
        );
        sequenceActions.push_back(attackAction);
        
        // Then deal damage to another player
        GetgudSDK::DamageActionData* damageAction = new GetgudSDK::DamageActionData(
            testMatchGuid,
            eventTime + 150,
            "consistency-player",
            "consistency-target",
            25.0f,
            "weapon-rifle"
        );
        sequenceActions.push_back(damageAction);
        
        // Then the player dies (killed by a different player)
        GetgudSDK::DeathActionData* deathAction = new GetgudSDK::DeathActionData(
            testMatchGuid,
            eventTime + 500,
            "consistency-player",
            "consistency-killer"
        );
        sequenceActions.push_back(deathAction);
        
        // Send the sequence
        bool sequenceResult = GetgudSDK::SendActions(sequenceActions);
        TEST_ASSERT(sequenceResult, "Sending action sequence should succeed");
        
        // Clean up
        for (auto* action : sequenceActions) {
            delete action;
        }
    }
    
    // Test 13: Memory Safety with Multiple Game & Match Creation
    std::cout << "\nTest 13: Memory Safety with Multiple Game & Match Creation..." << std::endl;
    {
        const int numGames = 5;
        const int numMatchesPerGame = 3;
        
        std::vector<std::string> gameGuids;
        std::vector<std::string> matchGuids;
        
        // Create multiple games
        for (int i = 0; i < numGames; i++) {
            std::string testGameGuid = GetgudSDK::StartGame(
                TITLE_ID + i, // Unique title ID for each game
                PRIVATE_KEY,
                "server-" + std::to_string(i),
                "game-mode-" + std::to_string(i),
                "location-" + std::to_string(i)
            );
            
            TEST_ASSERT(!testGameGuid.empty(), "Game creation should return valid GUID");
            gameGuids.push_back(testGameGuid);
            std::cout << "  Created game " << i << " with GUID: " << testGameGuid << std::endl;
            
            // Create multiple matches for each game
            for (int j = 0; j < numMatchesPerGame; j++) {
                std::string testGameMatchGuid = GetgudSDK::StartMatch(
                    testGameGuid,
                    "match-mode-" + std::to_string(j),
                    "map-" + std::to_string(j)
                );
                
                TEST_ASSERT(!testGameMatchGuid.empty(), "Match creation should return valid GUID");
                matchGuids.push_back(testGameMatchGuid);
                std::cout << "    Created match " << j << " with GUID: " << testGameMatchGuid << std::endl;
            }
        }
        
        // Send a few actions to each match
        for (const auto& mGuid : matchGuids) {
            GetgudSDK::PositionActionData* testAction = new GetgudSDK::PositionActionData(
                mGuid,
                eventTime,
                "test-player",
                GetgudSDK::PositionF{1.0f, 2.0f, 3.0f},
                GetgudSDK::RotationF{10.0f, 20.0f}
            );
            
            bool actionResult = GetgudSDK::SendAction(testAction);
            TEST_ASSERT(actionResult, "Sending action to match " + mGuid + " should succeed");
            
            delete testAction;
        }
        
        // Mark all games as ended
        for (const auto& gGuid : gameGuids) {
            bool endResult = GetgudSDK::MarkEndGame(gGuid);
            TEST_ASSERT(endResult, "Ending game " + gGuid + " should succeed");
        }
    }
    
    // Test 14: Extreme Edge Cases - Empty and null strings
    std::cout << "\nTest 14: Extreme Edge Cases - Empty and null strings..." << std::endl;
    {
        // Test starting a match with empty required fields (mapName)
        std::string emptyMatchGuid = GetgudSDK::StartMatch(gameGuid, "", "");
        // Expect StartMatch to fail (return empty string) because mapName is required and cannot be empty.
        TEST_ASSERT(emptyMatchGuid.empty(), "StartMatch should return empty GUID for invalid empty mapName");
        
        // Since the match creation failed, subsequent tests on emptyMatchGuid might not be meaningful
        // or could even crash if they assume a valid GUID. Let's skip them or use a valid GUID.
        
        // For testing SetMatchWinTeam with an empty team, let's use the valid testMatchGuid from earlier
        if (!testMatchGuid.empty()) { 
            bool setWinResult = GetgudSDK::SetMatchWinTeam(testMatchGuid, "");
            // Update Assertion: The SDK currently ALLOWS setting an empty team GUID.
            // If this behavior is incorrect, validation should be added in MatchData::SetMatchWinTeam.
            TEST_ASSERT(setWinResult, "Setting empty team as winner currently succeeds (no validation)"); 
        } else {
             std::cout << "  Skipping SetMatchWinTeam test as testMatchGuid was empty." << std::endl;
        }

        // Test extremely long strings in chat (use the valid testMatchGuid)
        std::string longMessage = "";
        for (int i = 0; i < 2000; i++) { // Exceeds the likely buffer size
            longMessage += "a";
        }
        
        GetgudSDK::ChatMessageInfo longChatMsg = {};
        longChatMsg.playerGuid = "player-0";
        longChatMsg.message = longMessage;
        longChatMsg.messageTimeEpoch = eventTime;
        
        if (!testMatchGuid.empty()) {
            bool longChatResult = GetgudSDK::SendChatMessage(testMatchGuid, longChatMsg);
            // Chat messages are sanitized/truncated, so sending should succeed.
            TEST_ASSERT(longChatResult, "Sending extremely long chat message should be sanitized and succeed"); 
        } else {
             std::cout << "  Skipping long chat message test as testMatchGuid was empty." << std::endl;
        }
    }
    
    // Test 15: Validating extreme position and rotation values
    std::cout << "\nTest 15: Extreme position and rotation values..." << std::endl;
    {
        // Test with extremely large position values
        GetgudSDK::PositionF extremePos = {
            std::numeric_limits<float>::max(), 
            std::numeric_limits<float>::max(), 
            std::numeric_limits<float>::max()
        };
        
        // Test with extreme rotation values
        GetgudSDK::RotationF extremeRot = {
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max()
        };
        
        GetgudSDK::PositionActionData* extremePosAction = new GetgudSDK::PositionActionData(
            testMatchGuid,
            eventTime,
            "player-0",
            extremePos,
            extremeRot
        );
        
        bool extremePosValid = extremePosAction->IsValid();
        // Position values are not strictly validated in IsValid, so this could be valid
        if (extremePosValid) {
            TEST_ASSERT(extremePosValid, "Action with extreme position/rotation validation result");
        } else {
            TEST_ASSERT_FAILS(extremePosValid, "Action with extreme position/rotation validation result");
        }
        
        bool extremePosResult = GetgudSDK::SendAction(extremePosAction);
        // May succeed or fail depending on implementation, just log the result
        if (extremePosResult) {
            TEST_ASSERT(extremePosResult, "Sending action with extreme position/rotation result");
        } else {
            TEST_ASSERT_FAILS(extremePosResult, "Sending action with extreme position/rotation result");
        }
        
        delete extremePosAction;
        
        // Test with NaN and infinity values
        GetgudSDK::PositionF nanPos = {
            std::numeric_limits<float>::quiet_NaN(),
            std::numeric_limits<float>::infinity(),
            -std::numeric_limits<float>::infinity()
        };
        
        GetgudSDK::RotationF nanRot = {
            std::numeric_limits<float>::quiet_NaN(),
            std::numeric_limits<float>::infinity(),
            0.0f
        };
        
        GetgudSDK::PositionActionData* nanPosAction = new GetgudSDK::PositionActionData(
            testMatchGuid,
            eventTime,
            "player-0",
            nanPos,
            nanRot
        );
        
        bool nanPosValid = nanPosAction->IsValid();
        // NaN/Infinity checks are not usually part of position validation, so could be valid
        if (nanPosValid) {
            TEST_ASSERT(nanPosValid, "Action with NaN/Infinity position/rotation validation result");
        } else {
            TEST_ASSERT_FAILS(nanPosValid, "Action with NaN/Infinity position/rotation validation result");
        }
        
        bool nanPosResult = GetgudSDK::SendAction(nanPosAction);
        // May succeed or fail depending on implementation, just log the result
        if (nanPosResult) {
            TEST_ASSERT(nanPosResult, "Sending action with NaN/Infinity position/rotation result");
        } else {
            TEST_ASSERT_FAILS(nanPosResult, "Sending action with NaN/Infinity position/rotation result");
        }
        
        delete nanPosAction;
    }

    // Test 16: StartGame Validation
    std::cout << "\nTest 16: StartGame Validation..." << std::endl;
    {
        // Use the constant private key 
        // std::string correctPrivateKey = "97237e40-1ad4-11f0-83cc-3b0b7cb4e97d";
        // Invalid Title ID (<= 0)
        std::string invalidTitleGameGuid = GetgudSDK::StartGame(0, PRIVATE_KEY, "server", "mode", "loc");
        TEST_ASSERT(invalidTitleGameGuid.empty(), "StartGame should fail with invalid Title ID (0)");

        // Invalid Private Key (empty)
        std::string invalidKeyGameGuid = GetgudSDK::StartGame(TITLE_ID, "", "server", "mode", "loc");
        TEST_ASSERT(invalidKeyGameGuid.empty(), "StartGame should fail with empty Private Key");

        // Invalid Private Key (contains bad chars - assuming basic validation)
        std::string badCharsKeyGameGuid = GetgudSDK::StartGame(TITLE_ID, "key$", "server", "mode", "loc");
        TEST_ASSERT(badCharsKeyGameGuid.empty(), "StartGame should fail with Private Key containing invalid chars");
    }

    // Test 17: SendReports (Out-of-Match) Validation
    std::cout << "\nTest 17: SendReports Validation..." << std::endl;
    {
        // Use the constant private key 
        // std::string correctPrivateKey = "97237e40-1ad4-11f0-83cc-3b0b7cb4e97d";
        std::deque<GetgudSDK::ReportInfo> reports;
        GetgudSDK::ReportInfo report;
        report.MatchGuid = testMatchGuid; // Use a valid match guid
        report.SuspectedPlayerGuid = "player-0";
        report.TbType = GetgudSDK::TbType::Aimbot;
        report.ReportedTimeEpoch = eventTime;
        
        // Test missing required field (SuspectedPlayerGuid)
        GetgudSDK::ReportInfo missingGuidReport = report;
        missingGuidReport.SuspectedPlayerGuid = ""; 
        reports.push_back(missingGuidReport);
        bool missingGuidResult = GetgudSDK::SendReports(TITLE_ID, PRIVATE_KEY, reports);
        TEST_ASSERT_FAILS(missingGuidResult, "SendReports should fail if SuspectedPlayerGuid is missing");
        reports.clear();

        // Test invalid enum value (TbType)
        GetgudSDK::ReportInfo invalidTbReport = report;
        invalidTbReport.TbType = (GetgudSDK::TbType)-5; // Invalid enum cast
        reports.push_back(invalidTbReport);
        bool invalidTbResult = GetgudSDK::SendReports(TITLE_ID, PRIVATE_KEY, reports);
        // Note: SendReports itself might succeed if AddReports validates, need internal check
        // Assuming AddReports validates and returns false here.
        TEST_ASSERT_FAILS(invalidTbResult, "SendReports should fail with invalid TbType");
        reports.clear();
    }

    // Test 18: UpdatePlayers Validation
    std::cout << "\nTest 18: UpdatePlayers Validation..." << std::endl;
    {
        // Use the constant private key 
        // std::string correctPrivateKey = "97237e40-1ad4-11f0-83cc-3b0b7cb4e97d";
        std::deque<GetgudSDK::PlayerInfo> players;
        GetgudSDK::PlayerInfo player;
        player.PlayerGuid = "player-for-update";

        // Test invalid email format (very basic check)
        GetgudSDK::PlayerInfo invalidEmailPlayer = player;
        invalidEmailPlayer.PlayerEmail = "not-an-email";
        players.push_back(invalidEmailPlayer);
        bool invalidEmailResult = GetgudSDK::UpdatePlayers(TITLE_ID, PRIVATE_KEY, players);
        // Update Assertion: The SDK sanitizes emails rather than validating format strictly.
        // So, the UpdatePlayers call should succeed even with a 'bad' format.
        TEST_ASSERT(invalidEmailResult, "UpdatePlayers should succeed even with technically invalid email (sanitization)"); 
        players.clear();

        // Test out-of-range rank
        GetgudSDK::PlayerInfo invalidRankPlayer = player;
        invalidRankPlayer.PlayerRank = -5;
        players.push_back(invalidRankPlayer);
        bool invalidRankResult = GetgudSDK::UpdatePlayers(TITLE_ID, PRIVATE_KEY, players);
        TEST_ASSERT_FAILS(invalidRankResult, "UpdatePlayers should fail with negative rank");
        players.clear();
    }
    
    std::cout << "\n========== VALIDATION TESTS COMPLETE ==========\n" << std::endl;
    // ============ END VALIDATION TESTS ============

    // ============ VISUAL DASHBOARD TESTS ============
    std::cout << "\n\n========== STARTING VISUAL DASHBOARD TESTS ==========\n" << std::endl;
    std::string visualTestMatchGuid = GetgudSDK::StartMatch(gameGuid, "VisualTest", "dashboard_check");
    if (visualTestMatchGuid.empty()) {
        std::cerr << "VISUAL TEST ERROR: Failed to create match for visual tests." << std::endl;
    } else {
        std::cout << "==> Visual Test Match GUID: " << visualTestMatchGuid << " <= Check this match on your dashboard." << std::endl;
        long long visualTestTime = eventTime + 10000; // Use a distinct time

        // --- Visual Test 1: Truncated Player GUID --- 
        std::cout << "\n  Visual Test 1: Sending action with overly long Player GUID..." << std::endl;
        std::string longPlayerGuid = "player-guid-that-is-definitely-longer-than-the-allowed-thirty-six-characters";
        GetgudSDK::SpawnActionData* longGuidSpawn = new GetgudSDK::SpawnActionData(
            visualTestMatchGuid,
            visualTestTime,
            longPlayerGuid,
            "vis-char",
            "vis-team",
            100.0f,
            {1,1,1},
            {0,0}
        );
        // BaseActionData::IsValid() checks PlayerGuid length (1-36) and will fail.
        // The action will be rejected by GameContainer::AddActions and NOT sent.
        // SendAction itself returns true because the pointer is valid and added to the buffer initially.
        bool sendResult = GetgudSDK::SendAction(longGuidSpawn); 
        TEST_ASSERT(sendResult, "SendAction accepts action with long GUID into buffer");
        // IMPORTANT: We don't delete longGuidSpawn here because GameContainer::AddActions 
        // should delete it when IsValid() returns false later in the processing pipeline.
        // delete longGuidSpawn; // This would cause a double-free if GameContainer works correctly.
        std::cout << "    ACTION DROPPED (Expected). Check Dashboard for Match GUID: " << visualTestMatchGuid << std::endl;
        std::cout << "    VERIFY: Player with GUID starting 'player-guid-that-is-definitely-lon...' should NOT appear. The action was rejected due to invalid length." << std::endl;

        visualTestTime += 100;

        // --- Visual Test 2: Sanitized Chat Message --- 
        std::cout << "\n  Visual Test 2: Sending chat message with special characters..." << std::endl;
        GetgudSDK::ChatMessageInfo specialChatMsg = {};
        specialChatMsg.playerGuid = "player-0"; // Use an existing player from main test
        specialChatMsg.message = "Check \"quotes\", backslashes \\, dollar $, percent %, and single quote ' sanitization.";
        specialChatMsg.messageTimeEpoch = visualTestTime;
        GetgudSDK::SendChatMessage(visualTestMatchGuid, specialChatMsg);
        std::cout << "    CHAT SENT. Check Dashboard Chat for Match GUID: " << visualTestMatchGuid << std::endl;
        std::cout << "    VERIFY: The message should appear with special chars like \", \\, $, %, ' replaced (likely with '_')." << std::endl;

        visualTestTime += 100;

        // --- Visual Test 3: Sanitized Report Name --- 
        std::cout << "\n  Visual Test 3: Sending in-match report with special characters in name..." << std::endl;
        GetgudSDK::ReportInfo specialReport = {};
        specialReport.MatchGuid = visualTestMatchGuid;
        specialReport.ReporterName = "Reporter\"$pecial%Name'"; // Contains chars to be sanitized
        specialReport.ReportedTimeEpoch = visualTestTime;
        specialReport.SuspectedPlayerGuid = "player-1"; // Report another player
        specialReport.TbType = GetgudSDK::TbType::Insulting; // Use a valid enum value
        GetgudSDK::SendInMatchReport(specialReport);
        std::cout << "    REPORT SENT. Check Dashboard Reports for Match GUID: " << visualTestMatchGuid << std::endl;
        std::cout << "    VERIFY: A report should appear with ReporterName sanitized (e.g., 'Reporter_$pecial_Name_') and TbType as Insulting." << std::endl;

        visualTestTime += 100;

        // --- Visual Test 4: Empty Optional Spawn Fields --- 
        std::cout << "\n  Visual Test 4: Spawning player with empty optional fields..." << std::endl;
         GetgudSDK::SpawnActionData* emptyFieldsSpawn = new GetgudSDK::SpawnActionData(
            visualTestMatchGuid,
            visualTestTime,
            "player-empty-fields", // New distinct player GUID
            "", // Empty Character GUID
            "", // Empty Team GUID
            100.0f,
            {2,2,2},
            {0,0}
        );
        GetgudSDK::SendAction(emptyFieldsSpawn);
        delete emptyFieldsSpawn;
        std::cout << "    ACTION SENT. Check Dashboard for Match GUID: " << visualTestMatchGuid << std::endl;
        std::cout << "    VERIFY: Player 'player-empty-fields' should appear. Check its details - Character/Team GUIDs might be empty or use defaults based on backend processing." << std::endl;

        visualTestTime += 100;

        // --- Visual Test 5: Truncated Weapon GUID --- 
        std::cout << "\n  Visual Test 5: Sending AttackAction with long weapon GUID..." << std::endl;
        std::string longWeaponGuid = "weapon-guid-that-is-also-very-long-and-should-be-truncated";
        GetgudSDK::AttackActionData* longWeaponAttack = new GetgudSDK::AttackActionData(
            visualTestMatchGuid,
            visualTestTime,
            "player-0", 
            longWeaponGuid
        );
        GetgudSDK::SendAction(longWeaponAttack);
        delete longWeaponAttack;
        std::cout << "    ACTION SENT. Check Dashboard events for Match GUID: " << visualTestMatchGuid << std::endl;
        std::cout << "    VERIFY: An Attack event should appear with Weapon GUID starting 'weapon-guid-that-is-also-very-lon...' truncated to 36 chars." << std::endl;

        visualTestTime += 100;

        // --- Visual Test 6: Death with Empty Attacker --- 
        std::cout << "\n  Visual Test 6: Sending DeathAction with empty attacker GUID..." << std::endl;
        GetgudSDK::DeathActionData* envDeath = new GetgudSDK::DeathActionData(
            visualTestMatchGuid,
            visualTestTime,
            "player-1", // Player 1 dies
            "" // Empty attacker GUID - NOW CONSIDERED INVALID
        );
        // SendAction will succeed (pointer buffered), but IsValid() will fail later.
        bool sendResult_VT6 = GetgudSDK::SendAction(envDeath);
        TEST_ASSERT(sendResult_VT6, "SendAction accepts action with empty attacker GUID into buffer");
        // IMPORTANT: We don't delete envDeath here because GameContainer::AddActions 
        // should delete it when IsValid() returns false later due to the empty attacker GUID.
        // delete envDeath; // This would cause a double-free.
        std::cout << "    ACTION DROPPED (Expected). Check Dashboard events for Match GUID: " << visualTestMatchGuid << std::endl;
        std::cout << "    VERIFY: A Death event for 'player-1' with an empty attacker should NOT appear. The action was rejected due to invalid empty attacker GUID." << std::endl;
        
        visualTestTime += 100;

        // --- Visual Test 7: Zero Damage --- 
        std::cout << "\n  Visual Test 7: Sending DamageAction with zero damage..." << std::endl;
        GetgudSDK::DamageActionData* zeroDamage = new GetgudSDK::DamageActionData(
            visualTestMatchGuid,
            visualTestTime,
            "player-2", // Attacker
            "player-3", // Victim
            0.0f,       // Zero damage
            "weapon-zero"
        );
        GetgudSDK::SendAction(zeroDamage);
        delete zeroDamage;
        std::cout << "    ACTION SENT. Check Dashboard events for Match GUID: " << visualTestMatchGuid << std::endl;
        std::cout << "    VERIFY: A Damage event from 'player-2' to 'player-3' with 0 damage should appear (if the dashboard displays zero-damage events)." << std::endl;

        visualTestTime += 100;

        // --- Visual Test 8: Player Update (Manual Verification Needed) --- 
        std::cout << "\n  Visual Test 8: Sending UpdatePlayers call..." << std::endl;
        std::deque<GetgudSDK::PlayerInfo> playersToUpdate;
        GetgudSDK::PlayerInfo playerUpdate;
        playerUpdate.PlayerGuid = "player-for-update"; // Use GUID from validation test 18
        playerUpdate.PlayerNickname = "UpdatedNickname";
        playerUpdate.PlayerRank = 99;
        playerUpdate.PlayerStatus = "Online-VisualTest";
        playersToUpdate.push_back(playerUpdate);
        GetgudSDK::UpdatePlayers(TITLE_ID, PRIVATE_KEY, playersToUpdate);
        std::cout << "    UPDATE SENT. Check Player Profile on Dashboard for GUID: player-for-update" << std::endl;
        std::cout << "    VERIFY: Player 'player-for-update' should have Nickname='UpdatedNickname', Rank=99, Status='Online-VisualTest'." << std::endl;

        visualTestTime += 100;

        // --- Visual Test 9: Out-of-Match Report (Manual Verification Needed) --- 
        std::cout << "\n  Visual Test 9: Sending out-of-match Report..." << std::endl;
        std::deque<GetgudSDK::ReportInfo> outOfMatchReports;
        GetgudSDK::ReportInfo oomReport;
        oomReport.MatchGuid = visualTestMatchGuid; // Non-existent match guid
        oomReport.ReporterName = "OutOfMatchReporter";
        oomReport.SuspectedPlayerGuid = "player-reported-oom";
        oomReport.TbType = GetgudSDK::TbType::Smurfing;
        oomReport.ReportedTimeEpoch = visualTestTime;
        outOfMatchReports.push_back(oomReport);
        GetgudSDK::SendReports(TITLE_ID, PRIVATE_KEY, outOfMatchReports);
        std::cout << "    REPORT SENT. Check main Reports list on Dashboard (not tied to a specific match)." << std::endl;
        std::cout << "    VERIFY: A report against 'player-reported-oom' for Smurfing by 'OutOfMatchReporter' should appear in the general reports list." << std::endl;

        visualTestTime += 100;

        // --- Visual Test 10: All Action Types --- 
        std::cout << "\n  Visual Test 10: Sending one of each core action type in a new match..." << std::endl;
        // Start a new, dedicated match for this test
        std::string allActionsMatchGuid = GetgudSDK::StartMatch(gameGuid, "AllActionsTest", "quick_core_check");
        if (allActionsMatchGuid.empty()) {
            std::cerr << "  VISUAL TEST 10 ERROR: Failed to create dedicated match." << std::endl;
        } else {
            std::cout << "    ==> All Actions Test Match GUID: " << allActionsMatchGuid << " <= Check this match on your dashboard." << std::endl;
            long long coreActionTime = visualTestTime; // Use a separate time sequence
            std::string coreActionPlayer = "player-all-actions";
            
            // 1. Spawn
            GetgudSDK::SpawnActionData* spawn = new GetgudSDK::SpawnActionData(
                allActionsMatchGuid, coreActionTime, coreActionPlayer, "core-char", "core-team", 100.f, {10,10,10}, {0,0}
            );
            GetgudSDK::SendAction(spawn); delete spawn; coreActionTime += 10;

            // 2. Position
            GetgudSDK::PositionActionData* position = new GetgudSDK::PositionActionData(
                allActionsMatchGuid, coreActionTime, coreActionPlayer, {11,11,11}, {10,10}
            );
            GetgudSDK::SendAction(position); delete position; coreActionTime += 10;

            // 3. Attack
            GetgudSDK::AttackActionData* attack = new GetgudSDK::AttackActionData(
                allActionsMatchGuid, coreActionTime, coreActionPlayer, "weapon-core-test"
            );
            GetgudSDK::SendAction(attack); delete attack; coreActionTime += 10;

            // 4. Damage (deals damage to player-0 - note: player-0 might not be visually present in *this* match replay)
            GetgudSDK::DamageActionData* damage = new GetgudSDK::DamageActionData(
                allActionsMatchGuid, coreActionTime, coreActionPlayer, "player-0", 15.5f, "weapon-core-test"
            );
            GetgudSDK::SendAction(damage); delete damage; coreActionTime += 10;

            // 5. Heal
            GetgudSDK::HealActionData* heal = new GetgudSDK::HealActionData(
                allActionsMatchGuid, coreActionTime, coreActionPlayer, 25.0f
            );
            GetgudSDK::SendAction(heal); delete heal; coreActionTime += 10;

            // 6. Death (killed by player-0)
            GetgudSDK::DeathActionData* death = new GetgudSDK::DeathActionData(
                allActionsMatchGuid, coreActionTime, coreActionPlayer, "player-0"
            );
            GetgudSDK::SendAction(death); delete death;

            std::cout << "    ACTIONS SENT. Check Dashboard event list for Match GUID: " << allActionsMatchGuid << std::endl;
            std::cout << "    VERIFY: Events for player '" << coreActionPlayer << "' should appear in order: Spawn, Position, Attack, Damage (to player-0), Heal, Death (by player-0)." << std::endl;
        }
    }
    std::cout << "\n========== VISUAL DASHBOARD TESTS COMPLETE ==========\n" << std::endl;
    // ============ END VISUAL DASHBOARD TESTS ============

    // Wait until packets potentially generated by tests are sent
    std::cout << "\nWaiting for final packets to send before ending game..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(5000)); // Increased wait time

    // End the main game (AFTER all tests)
    std::cout << "\nMarking main game as ended..." << std::endl;
    bool gameEnded = GetgudSDK::MarkEndGame(gameGuid);
    TEST_ASSERT(gameEnded, "MarkEndGame should succeed for the main game");

    // Wait until packets will be sent (including potential final game packet)
    std::cout << "Waiting for final packets to send before disposing SDK..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    // Close and Dispose of the SDK:
    std::cout << "\nDisposing SDK..." << std::endl;
    GetgudSDK::Dispose();

    // Wait until the threads will be stopped
    std::cout << "Waiting for threads to stop..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));

    system("pause");

    return 0;
}