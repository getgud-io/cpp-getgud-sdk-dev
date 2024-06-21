# Getgud Unreal Engine 5 Integration Tutorial

## Project Setup

### Additional to point 2. Integrate GetgudSDK Files

1. Copy the include and src folder of the GetgudSDK repository to this folder.
2. Replace any existing files when prompted.

```
Client_Server_Getgud
+---Plugins
|   \---GetgudSDK
|       |   GetgudSDK.uplugin
|       |
|       \---Source
|           \---GetgudSDKModule
|               |   GetgudSDKModule.Build.cs
|               |   GetgudSDKModule.cpp
|               |   GetgudSDKModule.h
|               |
|               \---ThirdParty
|                   +---include <----------------
|                   |
|                   \---src <----------------
|
+---Source
|   |   Client_Server_Getgud.Target.cs
|   |   Client_Server_GetgudEditor.Target.cs
|   |   Client_Server_GetgudServer.Target.cs
|   |
|   \---Client_Server_Getgud
|           Client_Server_Getgud.Build.cs
|           Client_Server_Getgud.cpp
|           Client_Server_Getgud.h
|           Client_Server_GetgudCharacter.cpp
|           Client_Server_GetgudCharacter.h
|           Client_Server_GetgudGameMode.cpp
|           Client_Server_GetgudGameMode.h
|           Client_Server_GetgudPlayerController.cpp
|           Client_Server_GetgudPlayerController.h
|           Client_Server_GetgudProjectile.cpp
|           Client_Server_GetgudProjectile.h
|           GetgudInstance.cpp
|           GetgudInstance.h
|           TP_PickUpComponent.cpp
|           TP_PickUpComponent.h
|           TP_WeaponComponent.cpp
|           TP_WeaponComponent.h
|
\---ThirdParty
    \---GetgudSDK
        \---include
```
