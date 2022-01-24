// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomProceduralTerrainDetails.h"
#include "ProcedurallyGeneratedTerrain.h"

#include "DetailLayoutBuilder.h"

TSharedRef<IDetailCustomization> FCustomProceduralTerrainDetails::MakeInstance()
{
	return MakeShareable(new FCustomProceduralTerrainDetails);
}

void FCustomProceduralTerrainDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	//perform a check to ensure at least one actor of right type has been selected
	AProcedurallyGeneratedTerrain* Actor = nullptr;
	TArray<TWeakObjectPtr<UObject>> CustomizedObjects;
	DetailBuilder.GetObjectsBeingCustomized(CustomizedObjects);

	for (auto Object : CustomizedObjects)
	{
		if (Object.IsValid()) //check to find the appropriate object has been selected
		{
			Actor = Cast<AProcedurallyGeneratedTerrain>(Object);
			if (Actor)
				break;
		}
	}
	check(Actor);
	DetailBuilder.EditCategory("Generate Terrain", FText::FromString("Generate Terrain"), ECategoryPriority::Transform);
	DetailBuilder.EditCategory("Terrain Size", FText::FromString("Terrain Size"), ECategoryPriority::Important);
	DetailBuilder.EditCategory("Smooth Terrain", FText::FromString("Smooth Terrain"), ECategoryPriority::Important);
	DetailBuilder.EditCategory("General Settings", FText::FromString("General Settings"), ECategoryPriority::Important);

	DetailBuilder.EditCategory("Seed", FText::FromString("Terrain Seed"), ECategoryPriority::Important);
	TSharedPtr<IPropertyHandle> CurrentSeed = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(AProcedurallyGeneratedTerrain, Seed));
	CurrentSeed->SetPropertyDisplayName(FText::FromString("Current Seed"));


	DetailBuilder.EditCategory("Smooth Terrain", FText::FromString("Smooth Terrain"), ECategoryPriority::Important);
	TSharedPtr<IPropertyHandle> bSmoothTerrain = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(AProcedurallyGeneratedTerrain, bSmoothTerrain));
	bSmoothTerrain->SetPropertyDisplayName(FText::FromString("Override Terracing"));


	DetailBuilder.EditCategory("Terrain Height", FText::FromString("Terrain Noise Settings"), ECategoryPriority::Important);
	DetailBuilder.EditCategory("Other", FText::FromString("Other"), ECategoryPriority::Important);
}