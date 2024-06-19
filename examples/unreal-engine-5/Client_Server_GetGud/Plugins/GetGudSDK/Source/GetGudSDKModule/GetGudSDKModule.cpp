

#include "GetGudSDKModule.h"

#include <GetgudSDK.h>


DEFINE_LOG_CATEGORY_STATIC(LogGetGudSDK, Log, All);

int GetGudFilterLogs = 1;
static FAutoConsoleVariableRef CVar_GetGudFilterLogs(
	TEXT("getgud.filterLogs"),
	GetGudFilterLogs,
	TEXT("Filter spam log"),
	ECVF_Cheat);

class FGetGudSDKModule : public IModuleInterface
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
		
		auto DllPath = FPaths::ProjectPluginsDir() / TEXT("GetGudSDK") / TEXT("ThirdParty") / TEXT("bin") / ConfDir / TEXT("GetgudSDK.dll");
		DllHandle = FPlatformProcess::GetDllHandle(*DllPath);
	}
	
	void ShutdownModule() override
	{
		FPlatformProcess::FreeDllHandle(DllHandle);
		DllHandle = nullptr;
	}

	void* DllHandle = nullptr;

};

IMPLEMENT_MODULE(FGetGudSDKModule, GetGudSDKModule);

uint64_t UGetGudUtils::NowMs()
{
	auto Now = FDateTime::UtcNow();
	auto Ticks = Now.GetTicks() - FDateTime(1970, 1, 1).GetTicks();
	return Ticks / ETimespan::TicksPerMillisecond;
}

GetgudSDK::PositionF UGetGudUtils::UnrealToGetGud(const FVector& Position)
{
	return { static_cast<float>(Position.X) / 100.0f, -static_cast<float>(Position.Y) / 100.0f, static_cast<float>(Position.Z) / 100.0f };
}

GetgudSDK::RotationF UGetGudUtils::UnrealToGetGud(const FRotator& Rotation)
{
	long YawLong = static_cast<long>(Rotation.Yaw);
	long LongYaw;

	// convert -360..360
	if(YawLong > 0)
	{
		LongYaw = (YawLong + 360) % 360;
	}
	else
	{
		LongYaw = -((-YawLong + 360) % 360);
	}

	// convert -180..180
	if(LongYaw > 180)
	{
		LongYaw = -180 + (LongYaw - 180);
	}
	else if(LongYaw < -180)
	{
		LongYaw = 180 - (LongYaw + 180);
	}
	

	auto Yaw = -static_cast<double>(LongYaw);
	auto Pitch = -static_cast<double>(Rotation.Pitch);
	
	return { static_cast<float>(Pitch), static_cast<float>(Yaw) };
}
