// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Client_Server_GetgudGameMode.generated.h"

UCLASS(minimalapi)
class AClient_Server_GetgudGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AClient_Server_GetgudGameMode();
	virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;


};



