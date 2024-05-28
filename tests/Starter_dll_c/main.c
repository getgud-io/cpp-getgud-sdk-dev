#include <GetgudSDK_C.h>
#ifdef _WIN32
#include <Windows.h>
#include <stdio.h>
#endif

int main() {
  //system("pause");
  init();
  //Fill the Game info
  char gameGuid[37] = { 0 }; //guid + \0
  int gameGuidSize = 0;
  struct StartGameInfo gameInfo;
  gameInfo.gameMode = "gameMode";
  gameInfo.gameModeSize = 8;
  gameInfo.privateKey = "a6effeb0-b3b6-11ee-a523-25e8feeae2d8";
  gameInfo.privateKeySize = 36;
  gameInfo.serverGuid = "serverGuid";
  gameInfo.serverGuidSize = 10;
  gameInfo.titleId = 111;
  gameInfo.serverLocation = "UK";
  gameInfo.serverLocationSize = 2;
  //Start the Game
  gameGuidSize = StartGame(gameInfo, gameGuid);

  //Fill the Match info
  char matchGuid[37] = { 0 }; //guid + \0
  int matchGuidSize = 0;
  struct StartMatchInfo matchInfo;
  matchInfo.gameGuid = gameGuid;
  matchInfo.gameGuidSize = 36;
  matchInfo.matchMode = "4143253";
  matchInfo.matchModeSize = 7;
  matchInfo.mapName = "serverGuid";
  matchInfo.mapNameSize = 10;
  //Start the Match
  matchGuidSize = StartMatch(matchInfo, matchGuid);

  printf("matchSize: %i\n", matchGuidSize);

  struct BaseActionData baseData;
  baseData.actionTimeEpoch = 1684059337532;
  baseData.matchGuid = matchGuid;
  baseData.matchGuidSize = 36;
  baseData.playerGuid = "player_guid";
  baseData.playerGuidSize = 11;

  SendAttackAction(baseData, "W_G", 3);

  SendDeathAction(baseData, "attacker_guid", 13);

  int martResult = MarkEndGame(gameGuid, 36);

  printf("End Game Result: %i\n", martResult);

  Sleep(6000);

  dispose();

  system("pause");

  return 0;
}