// Fill out your copyright notice in the Description page of Project Settings.


#include "GetGudInstance.h"
#include "GetgudSDK.h"

void UGetGudInstance::Init()
{
    Super::Init();

    if (GetWorld()->IsNetMode(NM_DedicatedServer) || GetWorld()->IsNetMode(NM_ListenServer))
    {
        UE_LOG(LogTemp, Warning, TEXT("GetGud SDK initialized."));
        GetgudSDK::Init();
    }
}

UGetGudInstance::~UGetGudInstance()
{
    if (GetWorld() && (GetWorld()->IsNetMode(NM_DedicatedServer) || GetWorld()->IsNetMode(NM_ListenServer)))
    {
        UE_LOG(LogTemp, Warning, TEXT("GetGud SDK disposed."));
        GetgudSDK::Dispose();
    }
}