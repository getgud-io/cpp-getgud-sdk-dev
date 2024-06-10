using UnrealBuildTool;
using System.Collections.Generic;

public class Client_Server_GetGudServerTarget : TargetRules
{
    public Client_Server_GetGudServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        ExtraModuleNames.AddRange( new string[] { "Client_Server_GetGud" } );
    }
}