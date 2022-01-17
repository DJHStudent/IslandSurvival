// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Modules/ModuleManager.h"
/**
 * added into the default class for the entire project, ensuring will run 
 class used for assigning the custom editors to the various classes
 */
class FDetailsBindingModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;

	virtual void ShutdownModule() override;

	virtual bool IsGameModule() const override
	{
		return true;
	}
};