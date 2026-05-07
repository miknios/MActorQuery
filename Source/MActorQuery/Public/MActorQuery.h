// Copyright (c) Miknios. All rights reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FMActorQueryModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
