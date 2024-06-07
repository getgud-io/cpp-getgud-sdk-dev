# Getgud Unreal Engine 5 Integration Tutorial

## Project Setup

### Additional to point 2. Integrate GetGudSDK Files

1. Copy the include folder of the GetGudSDK repository to this folder.
2. Replace any existing files when prompted.


Client_Server_GetGud
+---Plugins
|   \---GetGudSDK
|       |   GetGudSDK.uplugin
|       |
|       \---Source
|           \---GetGudSDKModule
|               |   GetGudSDKModule.Build.cs
|               |   GetGudSDKModule.cpp
|               |   GetGudSDKModule.h
|               |
|               \---ThirdParty
|                   +---include
|                   |
|                   \---src
|
+---Source
|   |   Client_Server_GetGud.Target.cs
|   |   Client_Server_GetGudEditor.Target.cs
|   |   Client_Server_GetGudServer.Target.cs
|   |
|   \---Client_Server_GetGud
|           Client_Server_GetGud.Build.cs
|           Client_Server_GetGud.cpp
|           Client_Server_GetGud.h
|           Client_Server_GetGudCharacter.cpp
|           Client_Server_GetGudCharacter.h
|           Client_Server_GetGudGameMode.cpp
|           Client_Server_GetGudGameMode.h
|           Client_Server_GetGudPlayerController.cpp
|           Client_Server_GetGudPlayerController.h
|           Client_Server_GetGudProjectile.cpp
|           Client_Server_GetGudProjectile.h
|           GetGudInstance.cpp
|           GetGudInstance.h
|           TP_PickUpComponent.cpp
|           TP_PickUpComponent.h
|           TP_WeaponComponent.cpp
|           TP_WeaponComponent.h
|
\---ThirdParty
    \---GetGudSDK
        \---include <----------------