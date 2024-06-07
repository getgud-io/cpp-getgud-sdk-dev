// Copyright Epic Games, Inc. All Rights Reserved.

#include "Client_Server_GetGudGameMode.h"
#include "Client_Server_GetGudCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GetgudSDK.h"
#include <random>
#include <chrono>

AClient_Server_GetGudGameMode::AClient_Server_GetGudGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
}