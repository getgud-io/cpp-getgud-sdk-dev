#include "../../include/GetGudSdk_C.h"
#include <Windows.h>
#include <stdio.h>

int main()
{
  init();
  char gameGuid[36];
  int gameGuidSize = 0;
  struct StartGameInfo gameInfo;
  gameInfo.gameMode = "gameMode";
  gameInfo.gameModeSize = 8;
  gameInfo.privateKey = "4143253";
  gameInfo.privateKeySize = 7;
  gameInfo.serverGuid = "serverGuid";
  gameInfo.serverGuidSize = 10;
  gameInfo.titleId = 1;
  gameGuidSize = StartGame(gameInfo, gameGuid);

  char matchGuid[36];
  int matchGuidSize = 0;
  struct StartMatchInfo matchInfo;
  /*
  *   char* gameGuid;
  int gameGuidSize;
  char* matchMode;
  int matchModeSize;
  char* mapName;
  int mapNameSize;
  */
  matchInfo.gameGuid = gameGuid;
  matchInfo.gameGuidSize = 36;
  matchInfo.matchMode = "4143253";
  matchInfo.matchModeSize = 7;
  matchInfo.mapName = "serverGuid";
  matchInfo.mapNameSize = 10;
  matchGuidSize = StartMatch(matchInfo, matchGuid);


  Sleep(2000);
}