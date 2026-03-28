#pragma once

#include "Modules/ModuleManager.h"

class FCoreInteractionModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

