// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class Client_Server_Getgud : ModuleRules
{
	public Client_Server_Getgud(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

		PrivateDependencyModuleNames.Add("GetgudSDKModule");

        // Setup the path to the directory containing the .lib and .dll files
        string ThirdPartyPath = Path.Combine(ModuleDirectory, "../../ThirdParty/GetgudSDK");

        PublicIncludePaths.Add(Path.Combine(ThirdPartyPath, "include"));
    }
}
