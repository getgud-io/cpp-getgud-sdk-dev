#include "../../include/GetGudSdk_C.h"
#include <Windows.h>
#include <stdio.h>

int main()
{
  char gameGuidCpy[36];
  int gameGuidSize = 0;
  init();
  struct StartGameInfo gameInfo;
  gameInfo.gameMode = "gameMode";
  gameInfo.gameModeSize = 8;
  gameInfo.privateKey = "4143253";
  gameInfo.privateKeySize = 7;
  gameInfo.serverGuid = "serverGuid";
  gameInfo.serverGuidSize = 10;
  gameInfo.titleId = 1;
  gameGuidSize = StartGame(gameInfo, &gameGuidCpy);
  printf("%i\n", gameGuidSize);
  Sleep(10000);
}