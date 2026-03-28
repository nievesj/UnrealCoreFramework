#pragma once

#include "Modules/ModuleManager.h"

class FCoreSaveModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

