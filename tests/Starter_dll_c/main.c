#include <GetGudSdk_C.h>
#ifdef _WIN32
#include <Windows.h>
#include <stdio.h>
#endif

int main() {
  init();
  //Fill the Game info
  char gameGuid[37] = { 0 }; //guid + \0
  int gameGuidSize = 0;
  struct StartGameInfo gameInfo;
  gameInfo.gameMode = "gameMode";
  gameInfo.gameModeSize = 8;
  gameInfo.privateKey = "4143253";
  gameInfo.privateKeySize = 7;
  gameInfo.serverGuid = "serverGuid";
  gameInfo.serverGuidSize = 10;
  gameInfo.titleId = 1;
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

  struct BaseActionData baseData;
  baseData.actionTimeEpoch = 1684059337532;
  baseData.matchGuid = matchGuid;
  baseData.matchGuidSize = 36;
  baseData.playerGuid = "player_guid";
  baseData.playerGuidSize = 11;

  SendAttackAction(baseData, "W_G", 3);

  SendDeathAction(baseData);

  MarkEndGame(gameGuid, 36);

  Sleep(6000);

  dispose();

  system("pause");

  return 0;
}