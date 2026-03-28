#pragma once

#include "Modules/ModuleManager.h"

class FCoreTweenModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

