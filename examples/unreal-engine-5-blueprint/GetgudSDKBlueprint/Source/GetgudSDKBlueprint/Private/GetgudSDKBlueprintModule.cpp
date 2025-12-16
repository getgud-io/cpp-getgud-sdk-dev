#include "GetgudSDKBlueprintModule.h"
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "HAL/PlatformProcess.h"

#define LOCTEXT_NAMESPACE "FGetgudSDKBlueprintModule"

void FGetgudSDKBlueprintModule::StartupModule()
{
#if PLATFORM_WINDOWS
	// Get the path to the executable directory where RuntimeDependencies copies the DLL
	FString ExeDir = FPaths::GetPath(FPlatformProcess::ExecutablePath());
	FString DLLPath = FPaths::Combine(ExeDir, TEXT("GetgudSDK.dll"));

	if (FPaths::FileExists(DLLPath))
	{
		GetgudSDKDLLHandle = FPlatformProcess::GetDllHandle(*DLLPath);
		if (GetgudSDKDLLHandle == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("GetgudSDKBlueprint: Failed to load GetgudSDK.dll from %s"), *DLLPath);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("GetgudSDKBlueprint: Successfully loaded GetgudSDK.dll from %s"), *DLLPath);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GetgudSDKBlueprint: GetgudSDK.dll not found at %s. Make sure to copy the DLL to the plugin's ThirdParty/GetgudSDK/bin/Win64/ folder."), *DLLPath);
	}
#endif
}

void FGetgudSDKBlueprintModule::ShutdownModule()
{
#if PLATFORM_WINDOWS
	if (GetgudSDKDLLHandle != nullptr)
	{
		FPlatformProcess::FreeDllHandle(GetgudSDKDLLHandle);
		GetgudSDKDLLHandle = nullptr;
		UE_LOG(LogTemp, Log, TEXT("GetgudSDKBlueprint: Unloaded GetgudSDK.dll"));
	}
#endif
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FGetgudSDKBlueprintModule, GetgudSDKBlueprint)
