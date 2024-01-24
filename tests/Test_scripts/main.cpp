#include <GetgudSDK.h>
#include <iostream>

#include <chrono>
#include <random>
#include <thread>

std::random_device
    rdMain;  // a system based device that generates data randomly
std::uniform_int_distribution<int> actionTypeDist(0, 5);
std::uniform_int_distribution<int> actionOrNotDist(0, 1000);

int randomActionIterator = 0;

std::string g_matchGuid;

void CreatePlayerUpdates(int numberOfPlayerUpdates, std::string playerGuid);

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
          new GetgudSDK::DeathActionData(matchGuid, curTimeEpoch, "player-0", "attackerGuid");
      break;
    case 3:
      outAction = new GetgudSDK::HealActionData(matchGuid, curTimeEpoch,
                                                ":plkayer-5", 20.32100F);
      break;
    case 4:
      outAction = new GetgudSDK::PositionActionData(
          matchGuid, curTimeEpoch, "player" + std::to_string(randomActionIterator % 2),
          GetgudSDK::PositionF{20.32000f + randomActionIterator, 50.001421f - randomActionIterator, 0.30021f / randomActionIterator },
          GetgudSDK::RotationF{10.f + randomActionIterator, 20.f - randomActionIterator, 30.f / randomActionIterator});
      break;
    case 5:
      outAction = new GetgudSDK::SpawnActionData(
          matchGuid, curTimeEpoch, "player-10", "ttr", 0, 100.f,
          GetgudSDK::PositionF{1, 2, 3}, GetgudSDK::RotationF{10, 20, 30});
      break;
  }

  randomActionIterator++;

  return outAction;
}

void CreateReports(int numberOfReports, std::string match_guid) {
  if (numberOfReports == 0) return;
  std::deque<GetgudSDK::ReportInfo> reports;
  std::string privateKey = "41e99370-b12f-11ee-89f0-4b4e9fccc950";
  for (int gameNum = 0; gameNum < numberOfReports; gameNum++) {
    GetgudSDK::ReportInfo reportInfo;
    reportInfo.MatchGuid = match_guid;
    reportInfo.ReportedTimeEpoch = 1705332732077;
    reportInfo.ReporterName = "reporter_name";
    reportInfo.ReporterSubType = GetgudSDK::ReporterSubtype::Custom;
    reportInfo.ReporterType = GetgudSDK::ReporterType::Custom;
    reportInfo.SuggestedToxicityScore = 99;
    reportInfo.SuspectedPlayerGuid = "76561197990301053";
    reportInfo.TbTimeEpoch = 1705332732077;
    reportInfo.TbType = GetgudSDK::TbType::Laghack;

    reports.push_back(reportInfo);
  }

  GetgudSDK::SendReports(132, privateKey, reports);
}

void CreateGames(int numberOfGames, int numberOfMatches, int numberOfItems, int reports) {
  if (numberOfItems == 0) return;
  std::vector<std::string> gameGuidMap;
  for (int gameNum = 0; gameNum < numberOfGames; gameNum++) {
    std::string gameGuid =
        GetgudSDK::StartGame(30, "8526b010-0c56-11ee-bc1d-9f343a78df6b",
                             "tests_round", "default_create_game", "UK");
    gameGuidMap.push_back(gameGuid);
    long long curEpochTime = 1684059337532;
    for (int matchNum = 0; matchNum < numberOfMatches; matchNum++) {
      std::string matchGuid =
          GetgudSDK::StartMatch(gameGuid, "hypermode_tester", "empty_map");
      g_matchGuid = matchGuid;
      for (int actionNum = 0; actionNum < numberOfItems; actionNum++) {
          GetgudSDK::BaseActionData* outAction =
              MakeRandomAction(matchGuid, curEpochTime);
          curEpochTime += 2;
          GetgudSDK::SendAction(outAction);
          delete outAction;
      }
    }
  }

  // std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  for (int i = 0; i < gameGuidMap.size(); i++) {
    GetgudSDK::MarkEndGame(gameGuidMap[i]);
  }
}

void CreateGames(int games, int matches, int actions, int reports, int messages) {
  if (actions == 0) return;
  std::vector<std::string> gameGuidMap;
  for (int gameNum = 0; gameNum < games; gameNum++) {
    std::string gameGuid =
      GetgudSDK::StartGame(132, "41e99370-b12f-11ee-89f0-4b4e9fccc950",
        "tests_round", "games_with_reports_messages", "UK");
    gameGuidMap.push_back(gameGuid);
    long long curEpochTime = 1684059337532;
    for (int matchNum = 0; matchNum < matches; matchNum++) {
      int matchReports = reports;
      int matchMessages = messages;
      std::string matchGuid =
        GetgudSDK::StartMatch(gameGuid, "hypermode_tester", "empty_map");
      for (int actionNum = 0; actionNum < actions; actionNum++) {
        curEpochTime += 1;
          GetgudSDK::BaseActionData* outAction =
            MakeRandomAction(matchGuid, curEpochTime);
          curEpochTime += 2;
          GetgudSDK::SendAction(outAction);

          if (matchReports)
          {
            matchReports--;

            GetgudSDK::ReportInfo reportInfo;
            reportInfo.MatchGuid = matchGuid;
            reportInfo.ReportedTimeEpoch = curEpochTime;
            reportInfo.ReporterName = "reporter_name";
            reportInfo.ReporterSubType = GetgudSDK::ReporterSubtype::Custom;
            reportInfo.ReporterType = GetgudSDK::ReporterType::Custom;
            reportInfo.SuggestedToxicityScore = 100;
            reportInfo.SuspectedPlayerGuid = outAction->m_playerGuid;
            reportInfo.TbTimeEpoch = curEpochTime;
            reportInfo.TbType = GetgudSDK::TbType::Laghack;
            GetgudSDK::SendInMatchReport(reportInfo);
          }
          if (matchMessages)
          {
            matchMessages--;

            GetgudSDK::ChatMessageInfo messageInfo;
            messageInfo.message = outAction->ToString();
            messageInfo.messageTimeEpoch = curEpochTime;
            messageInfo.playerGuid = outAction->m_playerGuid;
            GetgudSDK::SendChatMessage(matchGuid, messageInfo);
          }

          CreatePlayerUpdates(1, outAction->m_playerGuid);
          delete outAction;
      }
    }
  }

  // std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  for (int i = 0; i < gameGuidMap.size(); i++) {
    GetgudSDK::MarkEndGame(gameGuidMap[i]);
  }
}

void CreateGamesSingleAction(int games, int matches, int actions) {
  if (actions == 0) return;
  std::vector<std::string> gameGuidMap;
  for (int gameNum = 0; gameNum < games; gameNum++) {
    std::string gameGuid =
      GetgudSDK::StartGame(132, "41e99370-b12f-11ee-89f0-4b4e9fccc950",
        "tests_round", "games_with_reports_messages", "UK");
    gameGuidMap.push_back(gameGuid);
    long long curEpochTime = 1684059337532;
    for (int matchNum = 0; matchNum < matches; matchNum++) {
      std::string matchGuid =
        GetgudSDK::StartMatch(gameGuid, "hypermode_tester", "empty_map");
      for (int actionNum = 0; actionNum < actions; actionNum++) {
        curEpochTime += 1;
        GetgudSDK::BaseActionData* outAction =
          new GetgudSDK::AffectActionData(matchGuid, curEpochTime,
            "player-5", "abc", GetgudSDK::AffectState::Activate);
        curEpochTime += 2;
        GetgudSDK::SendAction(outAction);
        delete outAction;
      }
    }
  }

  // std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  for (int i = 0; i < gameGuidMap.size(); i++) {
    GetgudSDK::MarkEndGame(gameGuidMap[i]);
  }
}

void CreatePlayerUpdates(int numberOfPlayerUpdates, std::string playerGuid) {
    if (numberOfPlayerUpdates == 0) return;
    std::deque<GetgudSDK::PlayerInfo> playerInfos;
    std::string privateKey = "41e99370-b12f-11ee-89f0-4b4e9fccc950";
    for (int playerUpdateNum = 0; playerUpdateNum < numberOfPlayerUpdates;
        playerUpdateNum++) {
        GetgudSDK::PlayerInfo playerInfo;
        playerInfo.PlayerGuid = playerGuid;
        playerInfo.PlayerNickname = "test";
        playerInfo.PlayerEmail = "test@test.com";
        playerInfo.PlayerRank = 99;
        playerInfo.PlayerJoinDateEpoch = 1684059337532;
        playerInfo.PlayerStatus = "status_1";
        playerInfos.push_back(playerInfo);
    }

    GetgudSDK::UpdatePlayers(132, privateKey, playerInfos);
}

void CreatePlayerUpdates(int numberOfPlayerUpdates) {
  if (numberOfPlayerUpdates == 0) return;
  std::deque<GetgudSDK::PlayerInfo> playerInfos;
  std::string privateKey = "41e99370-b12f-11ee-89f0-4b4e9fccc950";
  for (int playerUpdateNum = 0; playerUpdateNum < numberOfPlayerUpdates;
       playerUpdateNum++) {
    GetgudSDK::PlayerInfo playerInfo;
    playerInfo.PlayerGuid = "76561197990301053" + std::to_string(playerUpdateNum);
    playerInfo.PlayerNickname = "test";
    playerInfo.PlayerEmail = "test@test.com";
    playerInfo.PlayerRank = 99;
    playerInfo.PlayerJoinDateEpoch = 1684059337532;
    playerInfo.PlayerStatus = "status_1";
    playerInfos.push_back(playerInfo);
  }
  
  GetgudSDK::UpdatePlayers(132, privateKey, playerInfos);
}

void RunSenders(int reports, int players, int games, int matches, int actions)
{
  /*Run Report Sender, Player Updater, Game Sender at the same time.
  Push 10, 100, 1000 reports to report sender, 10, 100, 1000 player updates
  to player updater and 1,5,10 games with 10 matches each with 20,000 
  actions each. Make sure everything reaches MW
*/
  CreateGames(games, matches, actions, reports);

}

void FeedGameWithMessagesAndReports(int games, int matches, int actions, int reports, int messages)
{
  /*Push a game with 2-3 reports per match and 
  with 10-20 chat messages per match, make sure 
  reports and messages reach MW
*/
  CreateGames(games, matches, actions, reports, messages);
}

void InvalidGuid(int games, int matches, int actions, int invalid_games, int invalid_matches)
{
  /*Inject invalid match id,
  game id to any method which 
  accepts it - should get warning logs
  */

  std::vector<std::string> gameGuidMap;
  for (int gameNum = 0; gameNum < games; gameNum++) {
    std::string gameGuid;
    if (invalid_games)
    {
      invalid_games--;
      gameGuid =
        GetgudSDK::StartGame(0, "8526b010-0c56-11ee-bc1d-9f343a78df6b",
          "tests_round", "invalid_guid", "UK");
    }
    else
    {
      gameGuid =
        GetgudSDK::StartGame(30, "8526b010-0c56-11ee-bc1d-9f343a78df6b",
          "tests_round", "random_actions", "UK");
    }

    gameGuidMap.push_back(gameGuid);
    long long curEpochTime = 1684059337532;
    for (int matchNum = 0; matchNum < matches; matchNum++) {
      std::string matchGuid;
      if (invalid_matches)
      {
        invalid_matches--;
        matchGuid =
          GetgudSDK::StartMatch(gameGuid, "dasdads\2dasdsaf", "empty_map");
      }
      else
      {
        matchGuid =
          GetgudSDK::StartMatch(gameGuid, "dsadasdasds", "empty_map");
      }

      for (int actionNum = 0; actionNum < actions; actionNum++) {
        int actionOrNot = actionOrNotDist(rdMain);
        if (actionOrNot > 3) {
          GetgudSDK::BaseActionData* outAction =
            MakeRandomAction(matchGuid, curEpochTime);
          curEpochTime += 2;
          GetgudSDK::SendAction(outAction);
          delete outAction;
        }
      }
    }
  }

  // std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  for (int i = 0; i < gameGuidMap.size(); i++) {
    GetgudSDK::MarkEndGame(gameGuidMap[i]);
  }

}

void InvalidPacket(int games, int matches, int actions)
{
  /*Try to send invalid JSON packet to MW,
  make sure 400-405 error for that is printed
  in the log file 
  */

  std::vector<std::string> gameGuidMap;
  for (int gameNum = 0; gameNum < games; gameNum++) {
    std::string gameGuid =
      GetgudSDK::StartGame(30, "8526b010-0c56-11ee-bc1d-9f343a78df6b",
        "tests_round", "invalid_packet", "UK");
    gameGuidMap.push_back(gameGuid);
    long long curEpochTime = 1684059337532;
    for (int matchNum = 0; matchNum < matches; matchNum++) {
      std::string matchGuid =
        GetgudSDK::StartMatch(gameGuid, "hypermode_tester", "empty_map");
      for (int actionNum = 0; actionNum < actions; actionNum++) {
        int actionOrNot = actionOrNotDist(rdMain);
        if (actionOrNot > 3) {
          GetgudSDK::BaseActionData* outAction =
            MakeRandomAction(matchGuid, curEpochTime);
          curEpochTime += 2;
          GetgudSDK::SendAction(outAction);
          delete outAction;
        }
      }
      GetgudSDK::BaseData invalidData = { GetgudSDK::Actions::Attack, 1684059337532, "123", matchGuid };
      GetgudSDK::BaseActionData* invalidAction = new GetgudSDK::BaseActionData(invalidData);
      GetgudSDK::SendAction(invalidAction);
      delete invalidAction;
    }
  }

  // std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  for (int i = 0; i < gameGuidMap.size(); i++) {
    GetgudSDK::MarkEndGame(gameGuidMap[i]);
  }

}

void GamePerTitle(int games, int matches, int actions)
{
  int title_id = 30;
  std::string server_guid = "8526b010-0c56-11ee-bc1d-9f343a78df6b";
  /*
  Push 10 games for 2 different titles at the same time,
  make sure the behavior is correct and games reach MW.
  */

  std::vector<std::string> gameGuidMap;
  for (int gameNum = 0; gameNum < games; gameNum++) {
    std::string gameGuid =
      GetgudSDK::StartGame(title_id, server_guid,
        "tests_round", "GamePerTitle", "UK");
    gameGuidMap.push_back(gameGuid);
    long long curEpochTime = 1684059337532;
    for (int matchNum = 0; matchNum < matches; matchNum++) {
      std::string matchGuid =
        GetgudSDK::StartMatch(gameGuid, "dsafsafes", "empty_map");
      for (int actionNum = 0; actionNum < actions; actionNum++) {
        int actionOrNot = actionOrNotDist(rdMain);
        if (actionOrNot > 3) {
          GetgudSDK::BaseActionData* outAction =
            MakeRandomAction(matchGuid, curEpochTime);
          curEpochTime += 2;
          GetgudSDK::SendAction(outAction);
          delete outAction;
        }
      }
    }
    if (title_id == 30)
    {
      title_id = 28;
      server_guid = "28482640-f571-11ed-8460-89c45273f291";
    }
  }

  // std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  for (int i = 0; i < gameGuidMap.size(); i++) {
    GetgudSDK::MarkEndGame(gameGuidMap[i]);
  }
}

void GamePositions(int games, int matches, int actions)
{
  int title_id = 28;
  std::string server_guid = "28482640-f571-11ed-8460-89c45273f291";
  /*
  Push 10 games for 2 different titles at the same time,
  make sure the behavior is correct and games reach MW.
  */

  std::vector<std::string> gameGuidMap;
  for (int gameNum = 0; gameNum < games; gameNum++) {
    std::string gameGuid =
      GetgudSDK::StartGame(title_id, server_guid,
        "tests_round", "positions", "UK");
    gameGuidMap.push_back(gameGuid);
    long long curEpochTime = 1684059337532;
    for (int matchNum = 0; matchNum < matches; matchNum++) {
      std::string matchGuid =
        GetgudSDK::StartMatch(gameGuid, "ewqewdafe", "empty_map");
      for (int actionNum = 0; actionNum < actions / 10; actionNum++) {

        for (int i = 0; i < 10; i++)
        {
          GetgudSDK::BaseActionData* outAction = new GetgudSDK::PositionActionData(
            matchGuid, curEpochTime, "player" + std::to_string(i),
            GetgudSDK::PositionF{ 20.32000f + (actionNum / 100.f), 20.32000f - (actionNum / 100.f), 50.001421f - ((actionNum / 100.f) / actionNum) },
            GetgudSDK::RotationF{ 10.f + (actionNum / 100.f), 20.f - (actionNum / 100.f), 30.f - ((actionNum / 100.f) / actionNum) });
          curEpochTime += 2;
          GetgudSDK::SendAction(outAction);
          delete outAction;
        }
      }
    }
  }

  // std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  for (int i = 0; i < gameGuidMap.size(); i++) {
    GetgudSDK::MarkEndGame(gameGuidMap[i]);
  }
}

void GamePositionsInvalid(int games, int matches, int actions)
{
  int title_id = 28;
  /*
  Push 10 games for 2 different titles at the same time,
  make sure the behavior is correct and games reach MW.
  */

  std::vector<std::string> gameGuidMap;
  for (int gameNum = 0; gameNum < games; gameNum++) {
    std::string gameGuid =
      GetgudSDK::StartGame(title_id, "28482640-f571-11ed-8460-89c45273f291",
        "tests_round", "positionsInvalid", "UK");
    gameGuidMap.push_back(gameGuid);
    long long curEpochTime = 1684059337532;
    for (int matchNum = 0; matchNum < matches; matchNum++) {
      std::string matchGuid =
        GetgudSDK::StartMatch(gameGuid, "hypermode_tester", "empty_map");
      for (int actionNum = 0; actionNum < actions / 10; actionNum++) {

        for (int i = 0; i < 10; i++)
        {
          GetgudSDK::BaseActionData* outAction = new GetgudSDK::PositionActionData(
            matchGuid, curEpochTime, "player" + std::to_string(i),
            GetgudSDK::PositionF{ FLT_MIN, FLT_MAX, actionNum * 0.00000001f },
            GetgudSDK::RotationF{ 10.f + (i / 100.f), 20.f - (i / 100.f), 30.f - ((i / 100.f) / i) });
          curEpochTime += 2;
          GetgudSDK::SendAction(outAction);
          delete outAction;
        }
        //Uncomment this if you want test float overload
        //{
        //  GetgudSDK::BaseActionData* outAction = new GetgudSDK::PositionActionData(
        //    matchGuid, curEpochTime, "player" + std::to_string(i),
        //    GetgudSDK::PositionF{ FLT_MIN - 1, FLT_MAX + 1, actionNum * 0.00000001f },
        //    GetgudSDK::RotationF{ 10.f + (i / 100.f), 20.f - (i / 100.f), 30.f - ((i / 100.f) / i) });
        //  curEpochTime += 2;
        //  GetgudSDK::SendAction(outAction);
        //  delete outAction;
        //}
      }
    }
  }

  // std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  for (int i = 0; i < gameGuidMap.size(); i++) {
    GetgudSDK::MarkEndGame(gameGuidMap[i]);
  }
}

void general_test()
{
  std::uniform_int_distribution<int> gamesAmount(1, 1);
  std::uniform_int_distribution<int> matchesAmount(1, 1);
  std::uniform_int_distribution<int> actionsAmount(3000, 3000);
  std::uniform_int_distribution<int> reportsAmount(300, 300);
  std::uniform_int_distribution<int> playerUpdatesAmount(1, 1);

  GetgudSDK::Init();
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  std::vector<std::thread> testThreads;

  for (int i = 0; i < 1; i++) {
    testThreads.push_back(std::thread([&]() {
      CreateGames(gamesAmount(rdMain), matchesAmount(rdMain),
      actionsAmount(rdMain), 0);
    //CreateReports(reportsAmount(rdMain));
    //CreatePlayerUpdates(playerUpdatesAmount(rdMain));
      }));
  }

  for (auto& thread : testThreads) {
    thread.join();
  }
  testThreads.clear();
  std::this_thread::sleep_for(std::chrono::milliseconds(15000));
  // GetgudSDK::Dispose();
}

int main() {
  GetgudSDK::Init();
  //RunSenders(int reports, int players, int games, int matches, int actions);
  //FeedGameWithMessagesAndReports(int games, int matches, int actions, int reports, int messages);
  //InvalidGuid(int games, int matches, int actions, int invalid_games, int invalid_matches);
  //InvalidPacket(int games, int matches, int actions); //Wrong test, because should be tested from the GameSender::SendPacket(...)
  //GamePerTitle(int games, int matches, int actions);
  
  //GamePerTitle(2, 2, 100);
  //general_test();
  // InvalidGuid(2, 2, 100, 0, 1);
  //std::thread t1([](){
  //  RunSenders(0, 0, 1, 1, 200); });
  //CreateGamesSingleAction(1, 1, 100);
  FeedGameWithMessagesAndReports(1, 1, 1, 1, 1);

  //std::thread t2([]() {CreatePlayerUpdates(1000); });
  //std::thread t3([]() {CreateReports(1000, "e7863816-8764-fa8b-ee31-fc5da7fe11f9"); });

  //CreatePlayerUpdates(4);
  //CreateReports(4, "ebf5a926-ae7c-09bc-e25e-25b9f56c61e4");

  //t1.join();
  //t2.join();
  //t3.join();

  while (true) {
  };  // let SDK run in background separetly from the main thread, in order to
      // see the behavior

  return 0;
}