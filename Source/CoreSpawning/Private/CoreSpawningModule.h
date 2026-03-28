#pragma once

#include "Modules/ModuleManager.h"

class FCoreSpawningModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

