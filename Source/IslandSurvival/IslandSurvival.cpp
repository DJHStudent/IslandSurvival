// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "IslandSurvival.h"
#include "Modules/ModuleManager.h"


#if WITH_EDITOR //only ever run if within the editor

#include "CustomDetails.h"
#include "PropertyEditorModule.h"

#endif

IMPLEMENT_PRIMARY_GAME_MODULE(FDetailsBindingModule, IslandSurvival, "IslandSurvival");

void FDetailsBindingModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("Module Starting Up"));

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	PropertyModule.RegisterCustomClassLayout("CustomDetailsTestActor", FOnGetDetailCustomizationInstance::CreateStatic(&FCustomDetails::MakeInstance));
}

void FDetailsBindingModule::ShutdownModule()
{
	UE_LOG(LogTemp, Warning, TEXT("Module Shutting Down"));
}
