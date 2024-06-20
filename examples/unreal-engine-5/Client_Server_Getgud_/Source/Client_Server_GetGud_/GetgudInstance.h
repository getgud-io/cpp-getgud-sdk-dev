// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GetgudInstance.generated.h"

/**
 * 
 */
UCLASS()
class CLIENT_SERVER_GETGUD_API UGetgudInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;
	virtual ~UGetgudInstance() noexcept;
};
