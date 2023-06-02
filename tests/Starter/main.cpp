#include <GetGudSdk.h>
#include <iostream>

#include <chrono>
#include <random>
#include <thread>

std::random_device
    rdMain;  // a system based device that generates data randomly
std::uniform_int_distribution<int> actionTypeDist(0, 5);
std::uniform_int_distribution<int> actionOrNotDist(0, 1000);

GetGudSdk::BaseActionData* MakeRandomAction(std::string matchGuid,
                                            long long curTimeEpoch) {
  int actionType = actionTypeDist(rdMain);

  GetGudSdk::BaseActionData* outAction = nullptr;
  switch (actionType) {
    case 0:
      outAction = new GetGudSdk::AttackActionData(matchGuid, curTimeEpoch,
                                                  "player-5", "abc");
      break;
    case 1:
      outAction = new GetGudSdk::DamageActionData(
          matchGuid, curTimeEpoch, "player-5", "player-0", 50.001421f, "glc");
      break;
    case 2:
      outAction =
          new GetGudSdk::DeathActionData(matchGuid, curTimeEpoch, "player-0");
      break;
    case 3:
      outAction = new GetGudSdk::HealActionData(matchGuid, curTimeEpoch,
                                                ":plkayer-5", 20.32100F);
      break;
    case 4:
      outAction = new GetGudSdk::PositionActionData(
          matchGuid, curTimeEpoch, "player-5",
          GetGudSdk::PositionF{20.32000f, 50.001421f, 0.30021f},
          GetGudSdk::RotationF{10, 20});
      break;
    case 5:
      outAction = new GetGudSdk::SpawnActionData(
          matchGuid, curTimeEpoch, "player-10", "ttr", 0, 100.f,
          GetGudSdk::PositionF{1, 2, 3}, GetGudSdk::RotationF{10, 20});
      break;
  }

  return outAction;
}

void CreateGames(int numberOfGames, int numberOfMatches, int numberOfItems) {
  int actions_pushed = 0;
  std::vector<std::string> gameGuidMap;
  for (int gameNum = 0; gameNum < numberOfGames; gameNum++) {
    std::string gameGuid =
        GetGudSdk::StartGame(28, "28482640-f571-11ed-8460-89c45273f291",
                             "hypermod e_tester", "random_actions");
    gameGuidMap.push_back(gameGuid);
    long long curEpochTime = 1684059337532;
    for (int matchNum = 0; matchNum < numberOfMatches; matchNum++) {
      std::string matchGuid =
          GetGudSdk::StartMatch(gameGuid, "hypermo de_tester", "empty_map");
      for (int actionNum = 0; actionNum < numberOfItems; actionNum++) {
        int actionOrNot = actionOrNotDist(rdMain);
        if (actionOrNot > 3) {
          GetGudSdk::BaseActionData* outAction =
              MakeRandomAction(matchGuid, curEpochTime);
          curEpochTime += 2;
          GetGudSdk::SendAction(outAction);
          delete outAction;
        } //else if (actionOrNot <= 1) {
        //  GetGudSdk::ReportInfo reportInfo;
        //  reportInfo.MatchGuid = matchGuid;
        //  reportInfo.ReportedTimeEpoch = curEpochTime;
        //  reportInfo.ReporterName = "SDK";
        //  reportInfo.ReporterSubType = 1;
        //  reportInfo.ReporterType = 1;
        //  reportInfo.SuggestedToxicityScore = 100;
        //  reportInfo.SuspectedPlayerGuid = "player-5";
        //  reportInfo.TbSubType = 1;
        //  reportInfo.TbTimeEpoch = curEpochTime;
        //  reportInfo.TbType = 1;
        //  GetGudSdk::SendInMatchReport(reportInfo);
        //} else {
        //  GetGudSdk::ChatMessageInfo messageInfo;
        //  messageInfo.message = "hi there";
        //  messageInfo.messageTimeEpoch = curEpochTime;
        //  messageInfo.playerGuid = "player1";
        //  GetGudSdk::SendChatMessage(matchGuid, messageInfo);
        //}
      }
    }
  }

  // std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  for (int i = 0; i < gameGuidMap.size(); i++) {
    GetGudSdk::MarkEndGame(gameGuidMap[i]);
  }
  gameGuidMap.erase(gameGuidMap.begin());
}

void CreateGamesDelayed(int numberOfGames,
                        int numberOfMatches,
                        int numberOfItems) {
  int actions_pushed = 0;
  std::vector<std::string> gameGuidMap;
  for (int gameNum = 0; gameNum < numberOfGames; gameNum++) {
    std::string gameGuid =
        GetGudSdk::StartGame(28, "28482640-f571-11ed-8460-89c45273f291",
                             "hypermode_tester", "random_actions");
    gameGuidMap.push_back(gameGuid);
    long long curEpochTime = 1684059337532;
    for (int matchNum = 0; matchNum < numberOfMatches; matchNum++) {
      std::string matchGuid =
          GetGudSdk::StartMatch(gameGuid, "hypermode_tester", "empty_map");
      for (int actionNum = 0; actionNum < numberOfItems; actionNum++) {
        int actionOrNot = actionOrNotDist(rdMain);
        if (actionOrNot > 10) {
          GetGudSdk::BaseActionData* outAction =
              MakeRandomAction(matchGuid, curEpochTime);
          curEpochTime += 2;
          GetGudSdk::SendAction(outAction);
          delete outAction;
        } else if (actionOrNot <= 5) {
          GetGudSdk::ReportInfo reportInfo;
          reportInfo.MatchGuid = matchGuid;
          reportInfo.ReportedTimeEpoch = curEpochTime;
          reportInfo.ReporterName = "SDK";
          reportInfo.ReporterSubType = 1;
          reportInfo.ReporterType = 1;
          reportInfo.SuggestedToxicityScore = 100;
          reportInfo.SuspectedPlayerGuid = "player-5";
          reportInfo.TbSubType = 1;
          reportInfo.TbTimeEpoch = curEpochTime;
          reportInfo.TbType = 1;
          GetGudSdk::SendInMatchReport(reportInfo);
        } else {
          GetGudSdk::ChatMessageInfo messageInfo;
          messageInfo.message = "hi there";
          messageInfo.messageTimeEpoch = curEpochTime;
          messageInfo.playerGuid = "player1";
          GetGudSdk::SendChatMessage(matchGuid, messageInfo);
        }
        std::this_thread::sleep_for(std::chrono::microseconds(1));
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // std::this_thread::sleep_for(std::chrono::milliseconds(30000));

  // endGame as soon as possible
  for (int i = 0; i < gameGuidMap.size(); i++) {
    GetGudSdk::MarkEndGame(gameGuidMap[i]);
  }
  gameGuidMap.erase(gameGuidMap.begin(), gameGuidMap.end());
}

void CreateReports(int numberOfReports) {
  std::deque<GetGudSdk::ReportInfo> reports;
  std::string privateKey = "28482640-f571-11ed-8460-89c45273f291";
  for (int gameNum = 0; gameNum < numberOfReports; gameNum++) {
    GetGudSdk::ReportInfo reportInfo;
    reportInfo.MatchGuid = "26663474-506b-1773-9458-1755761d5e1a";
    reportInfo.ReportedTimeEpoch = 1684059337532;
    reportInfo.ReporterName = "player-5";
    reportInfo.ReporterSubType = 1;
    reportInfo.ReporterType = 1;
    reportInfo.SuggestedToxicityScore = 100;
    reportInfo.SuspectedPlayerGuid = "player-5";
    reportInfo.TbSubType = 1;
    reportInfo.TbTimeEpoch = 1684059337532;
    reportInfo.TbType = 1;

    reports.push_back(reportInfo);
  }

  GetGudSdk::SendReports(28, privateKey, reports);
}

void CreatePlayerUpdates(int numberOfPlayerUpdates) {
  std::deque<GetGudSdk::PlayerInfo> playerInfos;
  std::string privateKey = "28482640-f571-11ed-8460-89c45273f291";
  for (int playerUpdateNum = 0; playerUpdateNum < numberOfPlayerUpdates;
       playerUpdateNum++) {
    GetGudSdk::PlayerInfo playerInfo;
    playerInfo.PlayerGuid = "player-5";
    playerInfo.PlayerNickname = "test";
    playerInfo.PlayerEmail = "test@test.com";
    playerInfo.PlayerRank = 10;
    playerInfo.PlayerJoinDateEpoch = 1684059337532;
    playerInfos.push_back(playerInfo);
  }

  GetGudSdk::UpdatePlayers(28, privateKey, playerInfos);
}

int main() {
  {
    std::uniform_int_distribution<int> gamesAmount(1, 1);
    std::uniform_int_distribution<int> matchesAmount(1, 1);
    std::uniform_int_distribution<int> actionsAmount(10000, 10000);
    std::uniform_int_distribution<int> reportsAmount(300, 300);
    std::uniform_int_distribution<int> playerUpdatesAmount(1, 1);

    GetGudSdk::Init();
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::vector<std::thread> testThreads;

    for (int i = 0; i < 3; i++) {
      testThreads.push_back(std::thread([&]() {
        CreateGames(gamesAmount(rdMain), matchesAmount(rdMain),
                     actionsAmount(rdMain));
        //CreateReports(reportsAmount(rdMain));
        //CreatePlayerUpdates(playerUpdatesAmount(rdMain));
      }));
    }

    for (auto& thread : testThreads) {
      thread.join();
    }
    testThreads.clear();
    std::this_thread::sleep_for(std::chrono::milliseconds(15000));
    // GetGudSdk::Dispose();
  }
  while (true) {
  };  // let SDK run in background separetly from the main thread, in order to
      // see the behavior

  return 0;
}