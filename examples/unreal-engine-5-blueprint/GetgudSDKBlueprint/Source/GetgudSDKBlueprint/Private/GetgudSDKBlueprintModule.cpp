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
	FString DLLPath;
	TArray<FString> SearchPaths;

	// Path 1 (RECOMMENDED): Use IPluginManager to get exact plugin location
	// This works regardless of where plugin is installed (project, engine, marketplace)
	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("GetgudSDKBlueprint"));
	if (Plugin.IsValid())
	{
		FString PluginBaseDir = Plugin->GetBaseDir();
		SearchPaths.Add(FPaths::Combine(
			PluginBaseDir,
			TEXT("ThirdParty"),
			TEXT("GetgudSDK"),
			TEXT("bin"),
			TEXT("Win64"),
			TEXT("GetgudSDK.dll")
		));
		UE_LOG(LogTemp, Log, TEXT("GetgudSDKBlueprint: Plugin base directory: %s"), *PluginBaseDir);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GetgudSDKBlueprint: Could not find plugin via IPluginManager"));
	}

	// Path 2: Executable directory (for packaged games - RuntimeDependencies copies here)
	SearchPaths.Add(FPaths::Combine(
		FPaths::GetPath(FPlatformProcess::ExecutablePath()),
		TEXT("GetgudSDK.dll")
	));

	// Path 3: Project Binaries folder (fallback)
	SearchPaths.Add(FPaths::Combine(
		FPaths::ProjectDir(),
		TEXT("Binaries"),
		TEXT("Win64"),
		TEXT("GetgudSDK.dll")
	));

	// Path 4: Project Plugins folder (fallback for editor mode)
	SearchPaths.Add(FPaths::Combine(
		FPaths::ProjectPluginsDir(),
		TEXT("GetgudSDKBlueprint"),
		TEXT("ThirdParty"),
		TEXT("GetgudSDK"),
		TEXT("bin"),
		TEXT("Win64"),
		TEXT("GetgudSDK.dll")
	));

	// Try each path until we find and load the DLL
	for (const FString& Path : SearchPaths)
	{
		FString NormalizedPath = FPaths::ConvertRelativePathToFull(Path);
		UE_LOG(LogTemp, Log, TEXT("GetgudSDKBlueprint: Checking for DLL at: %s"), *NormalizedPath);

		if (FPaths::FileExists(NormalizedPath))
		{
			GetgudSDKDLLHandle = FPlatformProcess::GetDllHandle(*NormalizedPath);
			if (GetgudSDKDLLHandle != nullptr)
			{
				UE_LOG(LogTemp, Log, TEXT("GetgudSDKBlueprint: Successfully loaded GetgudSDK.dll from %s"), *NormalizedPath);
				DLLPath = NormalizedPath;
				break;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("GetgudSDKBlueprint: DLL found but failed to load from %s (check dependencies with -dllerrors flag)"), *NormalizedPath);
			}
		}
	}

	if (GetgudSDKDLLHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("GetgudSDKBlueprint: Failed to load GetgudSDK.dll! Searched paths:"));
		for (const FString& Path : SearchPaths)
		{
			UE_LOG(LogTemp, Error, TEXT("  - %s"), *FPaths::ConvertRelativePathToFull(Path));
		}
		UE_LOG(LogTemp, Error, TEXT("GetgudSDKBlueprint: TIP - Run editor with -dllerrors flag to see detailed DLL loading errors"));
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
