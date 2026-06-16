#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FGetgudSDKBlueprintModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	/** Handle to the DLL */
	void* GetgudSDKDLLHandle = nullptr;
};
