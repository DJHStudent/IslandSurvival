// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomBiomeStatsDetails.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorDelegates.h"
#include "PropertyCustomizationHelpers.h"
#include "ClassViewerModule.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Engine/Selection.h"
#include "Editor.h"

#include "DetailLayoutBuilder.h"
#include "IDetailPropertyRow.h"
#include "DetailCategoryBuilder.h"
#include "Misc/Attribute.h"

TSharedRef<IDetailCustomization> FCustomBiomeStatsDetails::MakeInstance()
{
	return MakeShareable(new FCustomBiomeStatsDetails);
}

void FCustomBiomeStatsDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	//perform a check to ensure at least one actor of right type has been selected

	IDetailCategoryBuilder& BiomeCategory = DetailBuilder.EditCategory("Biome Spawning", FText::FromString("Biome Spawning"), ECategoryPriority::Default);
	IDetailCategoryBuilder& TerrainNoiseCategory = DetailBuilder.EditCategory("Terrain Height", FText::FromString("Terrain Noise Settings"), ECategoryPriority::Default);

	
	TSharedPtr<IPropertyHandle> BiomeProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UBiomeStatsObject, BiomeSpawningEnum));
	BiomeProperty->CreatePropertyNameWidget(FText::FromString("Spawn Condition"));

	//get the properties for the height based biomes
	TAttribute<EVisibility> HeightStatsVisibility = TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FCustomBiomeStatsDetails::PropertyMatchesEnum, BiomeProperty, EBiomeStats::HeightBased));
	IDetailPropertyRow& MinSpawnHeight = BiomeCategory.AddProperty(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UBiomeStatsObject, MinSpawnHeight)));
	IDetailPropertyRow& MaxSpawnHeight = BiomeCategory.AddProperty(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UBiomeStatsObject, MaxSpawnHeight)));

	//determine the height biomes visability
	MinSpawnHeight.Visibility(HeightStatsVisibility);
	MaxSpawnHeight.Visibility(HeightStatsVisibility);


	//get the properties for the land/water based biomes
	TSharedPtr<IPropertyHandle> OnlySingleProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UBiomeStatsObject, bOnlySingle));
	TAttribute<EVisibility> LandWaterStatsVisibility = TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FCustomBiomeStatsDetails::PropertiesLandWater, BiomeProperty));
	TAttribute<EVisibility> OnlySingleVisibility = TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FCustomBiomeStatsDetails::PropertiesSingleLandWater, BiomeProperty, OnlySingleProperty));
	
	IDetailPropertyRow& bOnlySingle = BiomeCategory.AddProperty(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UBiomeStatsObject, bOnlySingle)));
	IDetailPropertyRow& MinSpawnArea = BiomeCategory.AddProperty(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UBiomeStatsObject, MinSpawnArea)));
	IDetailPropertyRow& MaxSpawnArea = BiomeCategory.AddProperty(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UBiomeStatsObject, MaxSpawnArea)));

	//determine the land visability
	bOnlySingle.Visibility(LandWaterStatsVisibility);
	MinSpawnArea.Visibility(OnlySingleVisibility);
	MaxSpawnArea.Visibility(OnlySingleVisibility);

	//determine the water visability
//	bOnlySingle.Visibility(WaterStatsVisibility);
//	MinSpawnArea.Visibility(WaterStatsVisibility);
//	MaxSpawnArea.Visibility(WaterStatsVisibility);






	IDetailPropertyRow& TerrainNoise = TerrainNoiseCategory.AddProperty(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UBiomeStatsObject, TerrainHeight)));
	//IDetailPropertyRow& DoCustomTerrain = TerrainNoiseCategory.AddProperty(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UBiomeStatsObject, bCustomTerrain)));
	TSharedPtr<IPropertyHandle> bCustomTerrainProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UBiomeStatsObject, bCustomTerrain));
	TerrainNoise.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FCustomDetails::PropertyMatchesBool, bCustomTerrainProperty)));


}

EVisibility FCustomBiomeStatsDetails::PropertyMatchesEnum(TSharedPtr<IPropertyHandle> Property, EBiomeStats::Type DesiredMode) const
{
	if (Property.IsValid())
	{
		uint8 ValueAsByte;
		FPropertyAccess::Result Result = Property->GetValue(/*out*/ ValueAsByte);

		if (Result == FPropertyAccess::Success)
		{
			return (((EBiomeStats::Type)ValueAsByte) == DesiredMode) ? EVisibility::Visible : EVisibility::Collapsed;
		}
	}

	// If there are multiple values, show all properties
	return EVisibility::Visible;
}

EVisibility FCustomBiomeStatsDetails::PropertiesLandWater(TSharedPtr<IPropertyHandle> Property) const
{

	if (PropertyMatchesEnum(Property, EBiomeStats::LandBased) == EVisibility::Visible
		|| PropertyMatchesEnum(Property, EBiomeStats::WaterBased) == EVisibility::Visible)
	{
		// If there are multiple values, show all properties
		return EVisibility::Visible;
	}

	return EVisibility::Collapsed;
}

EVisibility FCustomBiomeStatsDetails::PropertiesSingleLandWater(TSharedPtr<IPropertyHandle> PropertyEnum, TSharedPtr<IPropertyHandle> PropertyBool) const
{

	if (PropertiesLandWater(PropertyEnum) == EVisibility::Visible
		&& PropertyMatchesBool(PropertyBool) == EVisibility::Visible)
	{
		// If there are multiple values, show all properties
		return EVisibility::Visible;
	}

	return EVisibility::Collapsed;
}