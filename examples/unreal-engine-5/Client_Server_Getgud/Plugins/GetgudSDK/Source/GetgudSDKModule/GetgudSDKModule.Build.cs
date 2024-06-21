using System.IO;
using UnrealBuildTool;

using System;

public class GetgudSDKModule : ModuleRules
{
	public GetgudSDKModule(ReadOnlyTargetRules Target) : base(Target)
	{
		bUseUnity = false;
		
		bEnableExceptions = true;
		
		PrivateDefinitions.Add("_CRT_SECURE_NO_WARNINGS=1");
		PublicDefinitions.Add("WITH_GETGUD=1");
		
		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Core",
				"Engine",
				
				"libcurl",
				"zlib",
				"nghttp2",
				"OpenSSL"
			}
		);
		
		var ThirdPartyRoot = Path.GetFullPath(Path.Combine(ModuleDirectory, "ThirdParty"));
		
		PublicIncludePaths.AddRange(
			new string[] {
				Path.Combine(ThirdPartyRoot, "include"),
				Path.Combine(ThirdPartyRoot, "include", "actions"),
			}
		);
	}
}