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


AActor* AClient_Server_GetgudGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
    // First, try to find a PlayerStart actor
    AActor* BestStart = Super::FindPlayerStart_Implementation(Player, IncomingName);

    if (BestStart)
    {
        // If a PlayerStart is found, use its location
        return BestStart;
    }
    else
    {
        // If no PlayerStart is found, use a default location or handle the error
        UE_LOG(LogTemp, Warning, TEXT("No PlayerStart found. Using default spawn location."));
        return nullptr;
    }
}