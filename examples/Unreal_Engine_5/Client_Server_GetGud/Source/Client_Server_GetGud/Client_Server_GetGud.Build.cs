// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class Client_Server_GetGud : ModuleRules
{
	public Client_Server_GetGud(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

		PrivateDependencyModuleNames.Add("GetGudSDKModule");

        // Setup the path to the directory containing the .lib and .dll files
        string ThirdPartyPath = Path.Combine(ModuleDirectory, "../../ThirdParty/GetGudSDK");

        PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "include"));
    }
}
