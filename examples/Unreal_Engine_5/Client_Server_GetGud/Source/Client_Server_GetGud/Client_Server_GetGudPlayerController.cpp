// Copyright Epic Games, Inc. All Rights Reserved.


#include "Client_Server_GetGudPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"

#include "GetGudSDK.h"

std::string g_gameGuid; //guid + \0
std::string g_matchGuid; //guid + \0

void AClient_Server_GetGudPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// add the mapping context so we get controls
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}

	if (GetWorld() && (GetWorld()->IsNetMode(NM_DedicatedServer) || GetWorld()->IsNetMode(NM_ListenServer)))
	{
		ServerRemoteGameAndMapStart();
		UE_LOG(LogTemp, Warning, TEXT("Player Controller is ending play, possibly due to disconnection."));
	}
}

void AClient_Server_GetGudPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    // Check if the end play is because of the player disconnecting
	if (GetWorld() && (GetWorld()->IsNetMode(NM_DedicatedServer) || GetWorld()->IsNetMode(NM_ListenServer)))
    {
		ServerRemoteGameEnd();
        UE_LOG(LogTemp, Warning, TEXT("Player Controller is ending play, possibly due to disconnection."));
    }
}

void AClient_Server_GetGudPlayerController::ServerRemoteGameAndMapStart_Implementation()
{

	std::string serverGuid = "us-west-1";
	std::string gameMode = "deathmatch";
	std::string serverLocation = "UK";

	// Start a Game:
	g_gameGuid = GetgudSDK::StartGame(your_title_id,
		your_private_key,
		serverGuid,  // serverGuid
		gameMode,     // gameMode
		serverLocation
	);

	g_matchGuid = GetgudSDK::StartMatch(g_gameGuid,
		"Knives-only",  // matchMode
		"de-dust"       // mapName
	);


	UE_LOG(LogTemp, Warning, TEXT("GetGud SDK Match started."));
}

bool AClient_Server_GetGudPlayerController::ServerRemoteGameAndMapStart_Validate()
{
	// Add any necessary validation here
	return true; // Assume validation passes
}


void AClient_Server_GetGudPlayerController::ServerRemoteGameEnd_Implementation()
{
	//int martResult = MarkEndGame(g_gameGuid, 36);
	GetgudSDK::MarkEndGame(g_gameGuid);

	UE_LOG(LogTemp, Warning, TEXT("GetGud SDK Game finish."));
}

bool AClient_Server_GetGudPlayerController::ServerRemoteGameEnd_Validate()
{
	// Add any necessary validation here
	return true; // Assume validation passes
}