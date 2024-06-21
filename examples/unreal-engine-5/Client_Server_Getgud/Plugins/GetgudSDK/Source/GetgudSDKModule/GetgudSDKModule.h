

#pragma once

#include "Engine.h"
#include "CoreMinimal.h"
#include "Helpers.h"

class GETGUDSDKMODULE_API UGetgudUtils
{
public:
	static uint64_t NowMs();
	static GetgudSDK::PositionF UnrealToGetGud(const FVector& Position);
	static GetgudSDK::RotationF UnrealToGetGud(const FRotator& Rotation);
};
