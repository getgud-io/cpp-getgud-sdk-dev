# GetGud SDK Blueprint Plugin for Unreal Engine 5

Blueprint wrapper for GetGud SDK. Windows (Win64) only.

## Setup Instructions

Before compiling, copy the following files from the SDK:

### 1. Include Files

Copy from `cpp-getgud-sdk-dev/include/` to `ThirdParty/GetgudSDK/include/`:

```
ThirdParty/GetgudSDK/include/
├── GetgudSDK.h
└── actions/
    ├── AffectAction.h
    ├── AttackAction.h
    ├── BaseActionData.h
    ├── DamageAction.h
    ├── DeathAction.h
    ├── HealAction.h
    ├── Helpers.h
    ├── PositionAction.h
    └── SpawnAction.h
```

### 2. DLL and Config

Copy to `ThirdParty/GetgudSDK/bin/Win64/`:

- `GetgudSDK.dll` - Compiled SDK DLL
- `config.json` - SDK configuration file (from `cpp-getgud-sdk-dev/config.json`)

## Usage

1. Copy the `GetgudSDKBlueprint` folder to your UE5 project's `Plugins/` directory
2. Build/package your project on Windows
3. The DLL and config.json will be automatically copied to the executable directory

## Blueprint Functions

All SDK functions are exposed under the following categories:

- **GetGud|Init** - Init, InitWithPath, InitWithContent, Dispose
- **GetGud|Game** - StartGame, StartMatch, MarkEndGame, SetMatchWinTeam
- **GetGud|Actions** - SendSpawnAction, SendPositionAction, SendAttackAction, SendDamageAction, SendDeathAction, SendHealAction, SendAffectAction
- **GetGud|Reports** - SendInMatchReport, SendReports
- **GetGud|Chat** - SendChatMessage
- **GetGud|Players** - UpdatePlayers
