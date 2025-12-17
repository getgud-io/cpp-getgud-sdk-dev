using UnrealBuildTool;
using System.IO;

public class GetgudSDKBlueprint : ModuleRules
{
	public GetgudSDKBlueprint(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Projects"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
			}
		);

		// GetGud SDK ThirdParty paths
		string ThirdPartyPath = Path.Combine(PluginDirectory, "ThirdParty", "GetgudSDK");
		string IncludePath = Path.Combine(ThirdPartyPath, "include");
		string BinPath = Path.Combine(ThirdPartyPath, "bin", "Win64");

		// Add include path
		PublicIncludePaths.Add(IncludePath);

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
			// DLL and LIB paths
			string DLLPath = Path.Combine(BinPath, "GetgudSDK.dll");
			string LIBPath = Path.Combine(BinPath, "GetgudSDK.lib");
			string ConfigPath = Path.Combine(BinPath, "config.json");

			// Link against the import library
			PublicAdditionalLibraries.Add(LIBPath);

			// Delay load the DLL
			PublicDelayLoadDLLs.Add("GetgudSDK.dll");

			// Copy DLL to output directory
			RuntimeDependencies.Add("$(TargetOutputDir)/GetgudSDK.dll", DLLPath);

			// Copy config.json to output directory (NonUFS keeps it as loose file, not in PAK)
			RuntimeDependencies.Add("$(TargetOutputDir)/config.json", ConfigPath, StagedFileType.NonUFS);
		}
	}
}
