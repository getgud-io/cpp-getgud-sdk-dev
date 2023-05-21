#include <iostream>
#include <GetGudSdk.h>

#include <chrono>
#include <thread>
#include <random>

std::random_device rd;  // a system based device that generates data randomly
std::uniform_int_distribution<int> actionTypeDist(0,5);
std::uniform_int_distribution<int> actionOrNotDist(0, 10);

GetGudSdk::BaseActionData* MakeRandomAction(std::string matchGuid, long long curTimeEpoch) {
    int actionType = actionTypeDist(rd);

    GetGudSdk::BaseActionData* outAction = nullptr;
    switch (actionType) {
    case 0:
        outAction = new GetGudSdk::AttackActionData(matchGuid, curTimeEpoch, "player-5",
            "abc");
        break;
    case 1:
        outAction = new GetGudSdk::DamageActionData(matchGuid, curTimeEpoch, "player-5", "player-0", 50.f, "glc");
        break;
    case 2:
        outAction =
            new GetGudSdk::DeathActionData(matchGuid, curTimeEpoch, "player-0");
        break;
    case 3:
        outAction =
            new GetGudSdk::HealActionData(matchGuid, curTimeEpoch, ":plkayer-5", 20.f);
        break;
    case 4:
        outAction = new GetGudSdk::PositionActionData(matchGuid, curTimeEpoch, "player-5", GetGudSdk::PositionF{1,2,3},
            GetGudSdk::RotationF{10,20});
        break;
    case 5:
        outAction = new GetGudSdk::SpawnActionData(matchGuid, curTimeEpoch, "player-10", "ttr", 0, 100.f, GetGudSdk::PositionF{ 1,2,3 },
            GetGudSdk::RotationF{ 10,20 });
        break;
    }

    return outAction;
}


void CreateGames(int numberOfGames, int numberOfMatches, int numberOfItems) {
    int actions_pushed = 0;
    std::vector<std::string> gameGuidMap;
    for (int gameNum = 0; gameNum < numberOfGames; gameNum++) {
        std::string gameGuid = GetGudSdk::StartGame(1, "private_key", "hypermode_tester", "random_actions");
        gameGuidMap.push_back(gameGuid);
        long long curEpochTime = 1684059337532;
        for (int matchNum = 0; matchNum < numberOfMatches; matchNum++) {
            std::string matchGuid = GetGudSdk::StartMatch(gameGuid, "hypermode_tester", "empty_map");
            for (int actionNum = 0; actionNum < numberOfItems; actionNum++) {
                int actionOrNot = actionOrNotDist(rd);
                if (actionOrNot > 3) {
                    GetGudSdk::BaseActionData* outAction = MakeRandomAction(matchGuid, curEpochTime);
                    curEpochTime += 2;
                    GetGudSdk::SendAction(outAction);
                }
                else if (actionOrNot <= 1) {
                    GetGudSdk::ReportInfo reportInfo;
                    reportInfo.MatchGuid = matchGuid;
                    reportInfo.ReportedTimeEpoch = curEpochTime;
                    reportInfo.ReporterName = "player1";
                    reportInfo.ReporterSubType = 0;
                    reportInfo.ReporterType = 0;
                    reportInfo.SuggestedToxicityScore = 100;
                    reportInfo.SuspectedPlayerGuid = "player1";
                    reportInfo.TbSubType = 0;
                    reportInfo.TbTimeEpoch = curEpochTime;
                    reportInfo.TbType = 0;
                    GetGudSdk::SendInMatchReport(reportInfo);
                }
                else {
                    GetGudSdk::ChatMessageInfo messageInfo;
                    messageInfo.message = "hi there";
                    messageInfo.messageTimeEpoch = curEpochTime;
                    messageInfo.playerGuid = "player1";
                    GetGudSdk::SendChatMessage(matchGuid, messageInfo);
                }
            }
        }
    }

    std::this_thread::sleep_for(
        std::chrono::milliseconds(10000));

    for (int i = 0; i < gameGuidMap.size(); i++) {
        GetGudSdk::MarkEndGame(gameGuidMap[i]);
    }
    gameGuidMap.erase(gameGuidMap.begin(), gameGuidMap.end());
}


int main()
{
    {
        GetGudSdk::Init();

        std::thread t1([&]() { CreateGames(5, 2, 1000);  });
        std::thread t2([&]() { CreateGames(5, 10, 10000);  });
        std::thread t3([&]() { CreateGames(5, 5, 100);  });

        t1.join();
        t2.join();
        t3.join();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    while (true) {}; //let SDK run in background separetly from the main thread, in order to see the behavior

    return 0;
}