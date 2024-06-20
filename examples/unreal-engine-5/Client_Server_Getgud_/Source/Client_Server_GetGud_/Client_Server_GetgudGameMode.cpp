// Copyright Epic Games, Inc. All Rights Reserved.

#include "Client_Server_GetgudGameMode.h"
#include "Client_Server_GetgudCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "GetgudSDK.h"
#include <random>
#include <chrono>

AClient_Server_GetgudGameMode::AClient_Server_GetgudGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;
}