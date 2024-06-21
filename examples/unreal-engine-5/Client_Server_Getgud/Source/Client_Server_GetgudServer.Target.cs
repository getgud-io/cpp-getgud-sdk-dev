using UnrealBuildTool;
using System.Collections.Generic;

public class Client_Server_GetgudServerTarget : TargetRules
{
    public Client_Server_GetgudServerTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Server;
        DefaultBuildSettings = BuildSettingsVersion.V2;
        ExtraModuleNames.AddRange( new string[] { "Client_Server_Getgud" } );
    }
}