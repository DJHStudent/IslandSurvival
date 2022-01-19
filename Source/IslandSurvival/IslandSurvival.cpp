// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "IslandSurvival.h"
#include "Modules/ModuleManager.h"


#if WITH_EDITOR //only ever run if within the editor

#include "CustomDetails.h"
#include "CustomProceduralTerrainDetails.h"
#include "CustomBiomeStatsDetails.h"
#include "PropertyEditorModule.h"

#endif

IMPLEMENT_PRIMARY_GAME_MODULE(FDetailsBindingModule, IslandSurvival, "IslandSurvival");

void FDetailsBindingModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("Module Starting Up"));
#if WITH_EDITOR
	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	//assign the class to use the custom property settings
	PropertyModule.RegisterCustomClassLayout("CustomDetailsTestActor", FOnGetDetailCustomizationInstance::CreateStatic(&FCustomDetails::MakeInstance));

	//the custom details for procedural terrain class
	PropertyModule.RegisterCustomClassLayout("ProcedurallyGeneratedTerrain", FOnGetDetailCustomizationInstance::CreateStatic(&FCustomProceduralTerrainDetails::MakeInstance));

	//the custom details for procedural terrain class
	PropertyModule.RegisterCustomClassLayout("BiomeStatsObject", FOnGetDetailCustomizationInstance::CreateStatic(&FCustomBiomeStatsDetails::MakeInstance));

#endif
}

void FDetailsBindingModule::ShutdownModule()
{
	UE_LOG(LogTemp, Warning, TEXT("Module Shutting Down"));
}
