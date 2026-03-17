

#include "GetgudSDKModule.h"

#include <GetgudSDK.h>


DEFINE_LOG_CATEGORY_STATIC(LogGetgudSDK, Log, All);

int GetgudFilterLogs = 1;
static FAutoConsoleVariableRef CVar_GetgudFilterLogs(
	TEXT("getgud.filterLogs"),
	GetgudFilterLogs,
	TEXT("Filter spam log"),
	ECVF_Cheat);

class FGetgudSDKModule : public IModuleInterface
{
public:
	void StartupModule() override
	{
#if UE_BUILD_TEST || UE_BUILD_SHIPPING
		FString Configuration(TEXT("release"));
#else
		FString Configuration(TEXT("debug"));
#endif
		
		FString ConfDir = TEXT("win64_") + Configuration;
		
		auto DllPath = FPaths::ProjectPluginsDir() / TEXT("GetgudSDK") / TEXT("ThirdParty") / TEXT("bin") / ConfDir / TEXT("GetgudSDK.dll");
		DllHandle = FPlatformProcess::GetDllHandle(*DllPath);
	}
	
	void ShutdownModule() override
	{
		if (DllHandle != nullptr)
		{
			FPlatformProcess::FreeDllHandle(DllHandle);
			DllHandle = nullptr;
		}
	}

	void* DllHandle = nullptr;

};

IMPLEMENT_MODULE(FGetgudSDKModule, GetgudSDKModule);

uint64_t UGetgudUtils::NowMs()
{
	auto Now = FDateTime::UtcNow();
	auto Ticks = Now.GetTicks() - FDateTime(1970, 1, 1).GetTicks();
	return Ticks / ETimespan::TicksPerMillisecond;
}

GetgudSDK::PositionF UGetgudUtils::UnrealToGetGud(const FVector& Position)
{
	return { static_cast<float>(Position.X) / 100.0f, -static_cast<float>(Position.Y) / 100.0f, static_cast<float>(Position.Z) / 100.0f };
}

GetgudSDK::RotationF UGetgudUtils::UnrealToGetGud(const FRotator& Rotation)
{
	// Normalize to -180..+180 — GetControlRotation() can return accumulated
	// values outside this range (e.g. pitch of -355° instead of 5°)
	FRotator NormalizedRotation = Rotation;
	NormalizedRotation.Normalize();

	auto Yaw = -static_cast<double>(NormalizedRotation.Yaw);
	auto Pitch = -static_cast<double>(NormalizedRotation.Pitch);

	return { static_cast<float>(Yaw), static_cast<float>(Pitch) };
}
