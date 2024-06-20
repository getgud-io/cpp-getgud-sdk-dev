// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Client_Server_GetgudPlayerController.generated.h"

class UInputMappingContext;

/**
 *
 */
UCLASS()
class CLIENT_SERVER_GETGUD_API AClient_Server_GetgudPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Input Mapping Context to be used for player input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

	// Server RPC for handling movement
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRemoteGameEnd();

	// Server RPC for handling movement
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRemoteGameAndMapStart();

	// Begin Actor interface
protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// End Actor interface
};
