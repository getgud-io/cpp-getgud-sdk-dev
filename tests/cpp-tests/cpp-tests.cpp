#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include "GetgudSDK.h"

// Simple test framework
#define TEST(name) void name()
#define ASSERT(condition) if (!(condition)) { std::cerr << "Assertion failed: " #condition << std::endl; throw std::runtime_error("Test failed"); }

// Helper function to generate random float
float randomFloat(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

TEST(testGetgudSDK) {
    // Initialize SDK
    ASSERT(GetgudSDK::Init());

    // Start Game
    std::string gameGuid = GetgudSDK::StartGame(
        219, // titleId
        "12bebc20-6aa6-11ef-b599-37b14b47bb25",
        "server-123",
        "deathmatch",
        "US-West"
    );
    ASSERT(!gameGuid.empty());
    std::cout << "Game started with GUID: " << gameGuid << std::endl;

    // Start Match
    std::string matchGuid = GetgudSDK::StartMatch(
        gameGuid,
        "5v5",
        "de_dust2"
    );
    ASSERT(!matchGuid.empty());
    std::cout << "Match started with GUID: " << matchGuid << std::endl;

    // Spawn players and perform actions
    std::vector<std::string> playerIds = {"player1", "player2", "player3", "player4"};
    for (const auto& playerId : playerIds) {
        // Create PositionF and RotationF objects
        GetgudSDK::PositionF position;
        position.X = randomFloat(0, 100);
        position.Y = randomFloat(0, 100);
        position.Z = randomFloat(0, 100);

        GetgudSDK::RotationF rotation;
        rotation.Yaw = randomFloat(0, 360);
        rotation.Pitch = randomFloat(-90, 90);
        rotation.Roll = 0;

        // Spawn action
        ASSERT(GetgudSDK::SendSpawnAction(
            matchGuid,
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count(),
            playerId,
            "character1",
            "team1",
            100.0f,
            position,
            rotation
        ));

        // Position action
        ASSERT(GetgudSDK::SendPositionAction(
            matchGuid,
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count(),
            playerId,
            position,
            rotation
        ));

        // Attack action
        ASSERT(GetgudSDK::SendAttackAction(
            matchGuid,
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count(),
            playerId,
            "weapon1"
        ));

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "All actions sent!" << std::endl;

    // Update player
    GetgudSDK::PlayerInfo playerInfo;
    playerInfo.PlayerGuid = "player1";
    playerInfo.PlayerNickname = "TestPlayer";
    playerInfo.PlayerEmail = "test@example.com";
    playerInfo.PlayerRank = 10;
    std::deque<GetgudSDK::PlayerInfo> players = {playerInfo};
    ASSERT(GetgudSDK::UpdatePlayers(219, "12bebc20-6aa6-11ef-b599-37b14b47bb25", players));

    std::cout << "Update player sent!" << std::endl;
    // Send report
    GetgudSDK::ReportInfo reportInfo;
    reportInfo.MatchGuid = matchGuid;
    reportInfo.ReporterName = "reporter1";
    reportInfo.ReporterType = GetgudSDK::ReporterType::Player;
    reportInfo.SuspectedPlayerGuid = "player2";
    reportInfo.TbType = GetgudSDK::TbType::Aimbot;
    reportInfo.ReportedTimeEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
    ASSERT(GetgudSDK::SendInMatchReport(reportInfo));
    
    std::cout << "Report sent!" << std::endl;

    // End game
    ASSERT(GetgudSDK::MarkEndGame(gameGuid));
    
    std::cout << "Game marked as ended!" << std::endl;

    // Wait for any pending operations to complete
    // std::this_thread::sleep_for(std::chrono::seconds(5));

    // Dispose of SDK
    GetgudSDK::Dispose();

    std::cout << "All tests passed successfully!" << std::endl;
}

int main() {
    try {
        testGetgudSDK();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}