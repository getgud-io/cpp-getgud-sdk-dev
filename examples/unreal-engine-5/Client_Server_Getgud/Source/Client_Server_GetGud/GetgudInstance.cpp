// Fill out your copyright notice in the Description page of Project Settings.


#include "GetgudInstance.h"
#include "GetgudSDK.h"

void UGetgudInstance::Init()
{
    Super::Init();

    if (GetWorld()->IsNetMode(NM_DedicatedServer) || GetWorld()->IsNetMode(NM_ListenServer))
    {
        UE_LOG(LogTemp, Warning, TEXT("Getgud SDK initialized."));
        GetgudSDK::Init();
    }
}

UGetgudInstance::~UGetgudInstance() noexcept
{
    if (GetWorld() && (GetWorld()->IsNetMode(NM_DedicatedServer) || GetWorld()->IsNetMode(NM_ListenServer)))
    {
        UE_LOG(LogTemp, Warning, TEXT("Getgud SDK disposed."));
        GetgudSDK::Dispose();
    }
}