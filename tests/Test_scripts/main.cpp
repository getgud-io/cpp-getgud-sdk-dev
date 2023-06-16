#include <GetGudSdk.h>
#include <iostream>

#include <chrono>
#include <random>
#include <thread>

std::random_device
    rdMain;  // a system based device that generates data randomly
std::uniform_int_distribution<int> actionTypeDist(0, 5);
std::uniform_int_distribution<int> actionOrNotDist(0, 1000);

int randomActionIterator = 0;

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
          matchGuid, curTimeEpoch, "player" + std::to_string(randomActionIterator % 2),
          GetGudSdk::PositionF{20.32000f + randomActionIterator, 50.001421f - randomActionIterator, 0.30021f / randomActionIterator },
          GetGudSdk::RotationF{10.f + randomActionIterator, 20.f - randomActionIterator, 30.f / randomActionIterator});
      break;
    case 5:
      outAction = new GetGudSdk::SpawnActionData(
          matchGuid, curTimeEpoch, "player-10", "ttr", 0, 100.f,
          GetGudSdk::PositionF{1, 2, 3}, GetGudSdk::RotationF{10, 20, 30});
      break;
  }

  randomActionIterator++;

  return outAction;
}

void CreateGames(int numberOfGames, int numberOfMatches, int numberOfItems) {
  std::vector<std::string> gameGuidMap;
  for (int gameNum = 0; gameNum < numberOfGames; gameNum++) {
    std::string gameGuid =
        GetGudSdk::StartGame(30, "8526b010-0c56-11ee-bc1d-9f343a78df6b",
                             "tests_round", "default_create_game");
    gameGuidMap.push_back(gameGuid);
    long long curEpochTime = 1684059337532;
    for (int matchNum = 0; matchNum < numberOfMatches; matchNum++) {
      std::string matchGuid =
          GetGudSdk::StartMatch(gameGuid, "hypermode_tester", "empty_map");
      for (int actionNum = 0; actionNum < numberOfItems; actionNum++) {
        int actionOrNot = actionOrNotDist(rdMain);
        if (actionOrNot > 3) {
          GetGudSdk::BaseActionData* outAction =
              MakeRandomAction(matchGuid, curEpochTime);
          curEpochTime += 2;
          GetGudSdk::SendAction(outAction);
          delete outAction;
        }// else if (actionOrNot <= 1) {
        //  GetGudSdk::ReportInfo reportInfo;
        //  reportInfo.MatchGuid = matchGuid;
        //  reportInfo.ReportedTimeEpoch = 1684059337532;
        //  reportInfo.ReporterName = "reporter_name";
        //  reportInfo.ReporterSubType = GetGudSdk::ReporterSubtype::Custom;
        //  reportInfo.ReporterType = GetGudSdk::ReporterType::Custom;
        //  reportInfo.SuggestedToxicityScore = 100;
        //  reportInfo.SuspectedPlayerGuid = "suspected_player_guid";
        //  reportInfo.TbTimeEpoch = 1684059337532;
        //  reportInfo.TbType = GetGudSdk::TbType::Laghack;
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
}

void CreateGames(int games, int matches, int actions, int reports, int messages) {
  std::vector<std::string> gameGuidMap;
  for (int gameNum = 0; gameNum < games; gameNum++) {
    std::string gameGuid =
      GetGudSdk::StartGame(30, "8526b010-0c56-11ee-bc1d-9f343a78df6b",
        "tests_round", "games_with_reports_messages");
    gameGuidMap.push_back(gameGuid);
    long long curEpochTime = 1684059337532;
    for (int matchNum = 0; matchNum < matches; matchNum++) {
      int matchReports = reports;
      int matchMessages = messages;
      std::string matchGuid =
        GetGudSdk::StartMatch(gameGuid, "hypermode_tester", "empty_map");
      for (int actionNum = 0; actionNum < actions; actionNum++) {
        int actionOrNot = actionOrNotDist(rdMain);
        curEpochTime += 1;
        if (actionOrNot > 3) {
          GetGudSdk::BaseActionData* outAction =
            MakeRandomAction(matchGuid, curEpochTime);
          curEpochTime += 2;
          GetGudSdk::SendAction(outAction);

          if (matchReports)
          {
            matchReports--;

            GetGudSdk::ReportInfo reportInfo;
            reportInfo.MatchGuid = matchGuid;
            reportInfo.ReportedTimeEpoch = curEpochTime;
            reportInfo.ReporterName = "reporter_name";
            reportInfo.ReporterSubType = GetGudSdk::ReporterSubtype::Custom;
            reportInfo.ReporterType = GetGudSdk::ReporterType::Custom;
            reportInfo.SuggestedToxicityScore = 100;
            reportInfo.SuspectedPlayerGuid = outAction->m_playerGuid;
            reportInfo.TbTimeEpoch = curEpochTime;
            reportInfo.TbType = GetGudSdk::TbType::Laghack;
            GetGudSdk::SendInMatchReport(reportInfo);
          }
          if (matchMessages)
          {
            matchMessages--;

            GetGudSdk::ChatMessageInfo messageInfo;
            messageInfo.message = outAction->ToString();
            messageInfo.messageTimeEpoch = curEpochTime;
            messageInfo.playerGuid = outAction->m_playerGuid;
            GetGudSdk::SendChatMessage(matchGuid, messageInfo);
          }
          delete outAction;
        } 
      }
    }
  }

  // std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  for (int i = 0; i < gameGuidMap.size(); i++) {
    GetGudSdk::MarkEndGame(gameGuidMap[i]);
  }
}

void CreateReports(int numberOfReports) {
  std::deque<GetGudSdk::ReportInfo> reports;
  std::string privateKey = "8526b010-0c56-11ee-bc1d-9f343a78df6b";
  for (int gameNum = 0; gameNum < numberOfReports; gameNum++) {
    GetGudSdk::ReportInfo reportInfo;
    reportInfo.MatchGuid = "28482640-f571-11ed-8460-89c45273f291";
    reportInfo.ReportedTimeEpoch = 1684059337532;
    reportInfo.ReporterName = "reporter_name";
    reportInfo.ReporterSubType = GetGudSdk::ReporterSubtype::Custom;
    reportInfo.ReporterType = GetGudSdk::ReporterType::Custom;
    reportInfo.SuggestedToxicityScore = 100;
    reportInfo.SuspectedPlayerGuid = "suspected_player_guid";
    reportInfo.TbTimeEpoch = 1684059337532;
    reportInfo.TbType = GetGudSdk::TbType::Laghack;

    reports.push_back(reportInfo);
  }

  GetGudSdk::SendReports(30, privateKey, reports);
}

void CreatePlayerUpdates(int numberOfPlayerUpdates) {
  std::deque<GetGudSdk::PlayerInfo> playerInfos;
  std::string privateKey = "8526b010-0c56-11ee-bc1d-9f343a78df6b";
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

  GetGudSdk::UpdatePlayers(30, privateKey, playerInfos);
}

void RunSenders(int reports, int players, int games, int matches, int actions)
{
  /*Run Report Sender, Player Updater, Game Sender at the same time.
  Push 10, 100, 1000 reports to report sender, 10, 100, 1000 player updates
  to player updater and 1,5,10 games with 10 matches each with 20,000 
  actions each. Make sure everything reaches MW
*/
  CreateReports(reports);
  CreatePlayerUpdates(players);
  CreateGames(games, matches, actions);

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
        GetGudSdk::StartGame(0, "8526b010-0c56-11ee-bc1d-9f343a78df6b",
          "tests_round", "invalid_guid");
    }
    else
    {
      gameGuid =
        GetGudSdk::StartGame(30, "8526b010-0c56-11ee-bc1d-9f343a78df6b",
          "tests_round", "random_actions");
    }

    gameGuidMap.push_back(gameGuid);
    long long curEpochTime = 1684059337532;
    for (int matchNum = 0; matchNum < matches; matchNum++) {
      std::string matchGuid;
      if (invalid_matches)
      {
        invalid_matches--;
        matchGuid =
          GetGudSdk::StartMatch(gameGuid, "dasdads\2dasdsaf", "empty_map");
      }
      else
      {
        matchGuid =
          GetGudSdk::StartMatch(gameGuid, "dsadasdasds", "empty_map");
      }

      for (int actionNum = 0; actionNum < actions; actionNum++) {
        int actionOrNot = actionOrNotDist(rdMain);
        if (actionOrNot > 3) {
          GetGudSdk::BaseActionData* outAction =
            MakeRandomAction(matchGuid, curEpochTime);
          curEpochTime += 2;
          GetGudSdk::SendAction(outAction);
          delete outAction;
        }
      }
    }
  }

  // std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  for (int i = 0; i < gameGuidMap.size(); i++) {
    GetGudSdk::MarkEndGame(gameGuidMap[i]);
  }

}

void InvalidPacket(int games, int matches, int actions)
{
  /*Try to send invalid JSON packet to MW,
  make sure 400-405 error for that is printed
  in the log file 
  */

  //TODO must be changed from the SDK, because have no ways to do it externaly

  std::vector<std::string> gameGuidMap;
  for (int gameNum = 0; gameNum < games; gameNum++) {
    std::string gameGuid =
      GetGudSdk::StartGame(30, "8526b010-0c56-11ee-bc1d-9f343a78df6b",
        "tests_round", "invalid_packet");
    gameGuidMap.push_back(gameGuid);
    long long curEpochTime = 1684059337532;
    for (int matchNum = 0; matchNum < matches; matchNum++) {
      std::string matchGuid =
        GetGudSdk::StartMatch(gameGuid, "hypermode_tester", "empty_map");
      for (int actionNum = 0; actionNum < actions; actionNum++) {
        int actionOrNot = actionOrNotDist(rdMain);
        if (actionOrNot > 3) {
          GetGudSdk::BaseActionData* outAction =
            MakeRandomAction(matchGuid, curEpochTime);
          curEpochTime += 2;
          GetGudSdk::SendAction(outAction);
          delete outAction;
        }
      }
      GetGudSdk::BaseData invalidData = { GetGudSdk::Actions::Attack, 1684059337532, "123", matchGuid };
      GetGudSdk::BaseActionData* invalidAction = new GetGudSdk::BaseActionData(invalidData);
      GetGudSdk::SendAction(invalidAction);
      delete invalidAction;
    }
  }

  // std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  for (int i = 0; i < gameGuidMap.size(); i++) {
    GetGudSdk::MarkEndGame(gameGuidMap[i]);
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
      GetGudSdk::StartGame(title_id, server_guid,
        "tests_round", "GamePerTitle");
    gameGuidMap.push_back(gameGuid);
    long long curEpochTime = 1684059337532;
    for (int matchNum = 0; matchNum < matches; matchNum++) {
      std::string matchGuid =
        GetGudSdk::StartMatch(gameGuid, "dsafsafes", "empty_map");
      for (int actionNum = 0; actionNum < actions; actionNum++) {
        int actionOrNot = actionOrNotDist(rdMain);
        if (actionOrNot > 3) {
          GetGudSdk::BaseActionData* outAction =
            MakeRandomAction(matchGuid, curEpochTime);
          curEpochTime += 2;
          GetGudSdk::SendAction(outAction);
          delete outAction;
        }
      }
    }
    if (gameNum > games / 2 && title_id == 28)
    {
      title_id = 28;
      server_guid = "28482640-f571-11ed-8460-89c45273f291";
    }
  }

  // std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  for (int i = 0; i < gameGuidMap.size(); i++) {
    GetGudSdk::MarkEndGame(gameGuidMap[i]);
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
      GetGudSdk::StartGame(title_id, server_guid,
        "tests_round", "positions");
    gameGuidMap.push_back(gameGuid);
    long long curEpochTime = 1684059337532;
    for (int matchNum = 0; matchNum < matches; matchNum++) {
      std::string matchGuid =
        GetGudSdk::StartMatch(gameGuid, "ewqewdafe", "empty_map");
      for (int actionNum = 0; actionNum < actions / 10; actionNum++) {

        for (int i = 0; i < 10; i++)
        {
          GetGudSdk::BaseActionData* outAction = new GetGudSdk::PositionActionData(
            matchGuid, curEpochTime, "player" + std::to_string(i),
            GetGudSdk::PositionF{ 20.32000f + (actionNum / 100.f), 20.32000f - (actionNum / 100.f), 50.001421f - ((actionNum / 100.f) / actionNum) },
            GetGudSdk::RotationF{ 10.f + (actionNum / 100.f), 20.f - (actionNum / 100.f), 30.f - ((actionNum / 100.f) / actionNum) });
          curEpochTime += 2;
          GetGudSdk::SendAction(outAction);
          delete outAction;
        }
      }
    }
  }

  // std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  for (int i = 0; i < gameGuidMap.size(); i++) {
    GetGudSdk::MarkEndGame(gameGuidMap[i]);
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
      GetGudSdk::StartGame(title_id, "28482640-f571-11ed-8460-89c45273f291",
        "tests_round", "positionsInvalid");
    gameGuidMap.push_back(gameGuid);
    long long curEpochTime = 1684059337532;
    for (int matchNum = 0; matchNum < matches; matchNum++) {
      std::string matchGuid =
        GetGudSdk::StartMatch(gameGuid, "hypermode_tester", "empty_map");
      for (int actionNum = 0; actionNum < actions / 10; actionNum++) {

        for (int i = 0; i < 10; i++)
        {
          GetGudSdk::BaseActionData* outAction = new GetGudSdk::PositionActionData(
            matchGuid, curEpochTime, "player" + std::to_string(i),
            GetGudSdk::PositionF{ FLT_MIN, FLT_MAX, actionNum * 0.00000001f },
            GetGudSdk::RotationF{ 10.f + (i / 100.f), 20.f - (i / 100.f), 30.f - ((i / 100.f) / i) });
          curEpochTime += 2;
          GetGudSdk::SendAction(outAction);
          delete outAction;
        }
        //Uncomment this if you want test float overload
        //{
        //  GetGudSdk::BaseActionData* outAction = new GetGudSdk::PositionActionData(
        //    matchGuid, curEpochTime, "player" + std::to_string(i),
        //    GetGudSdk::PositionF{ FLT_MIN - 1, FLT_MAX + 1, actionNum * 0.00000001f },
        //    GetGudSdk::RotationF{ 10.f + (i / 100.f), 20.f - (i / 100.f), 30.f - ((i / 100.f) / i) });
        //  curEpochTime += 2;
        //  GetGudSdk::SendAction(outAction);
        //  delete outAction;
        //}
      }
    }
  }

  // std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  for (int i = 0; i < gameGuidMap.size(); i++) {
    GetGudSdk::MarkEndGame(gameGuidMap[i]);
  }
}

void general_test()
{
  std::uniform_int_distribution<int> gamesAmount(1, 1);
  std::uniform_int_distribution<int> matchesAmount(1, 1);
  std::uniform_int_distribution<int> actionsAmount(3000, 3000);
  std::uniform_int_distribution<int> reportsAmount(300, 300);
  std::uniform_int_distribution<int> playerUpdatesAmount(1, 1);

  GetGudSdk::Init();
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  std::vector<std::thread> testThreads;

  for (int i = 0; i < 1; i++) {
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

int main() {
  GetGudSdk::Init();
  //RunSenders(int reports, int players, int games, int matches, int actions);
  //FeedGameWithMessagesAndReports(int games, int matches, int actions, int reports, int messages);
  //InvalidGuid(int games, int matches, int actions, int invalid_games, int invalid_matches);
  //InvalidPacket(int games, int matches, int actions); //Wrong test, because should be tested from the GameSender::SendPacket(...)
  //GamePerTitle(int games, int matches, int actions);
  
  //GamePerTitle(2, 2, 100);
  //general_test();
  InvalidGuid(2, 2, 100, 0, 1);

  while (true) {
  };  // let SDK run in background separetly from the main thread, in order to
      // see the behavior

  return 0;
}