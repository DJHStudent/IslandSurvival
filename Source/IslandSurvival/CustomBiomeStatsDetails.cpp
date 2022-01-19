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
	
	
	TSharedPtr<IPropertyHandle> BiomeProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UBiomeStatsObject, BiomeSpawningEnum));
	BiomeProperty->CreatePropertyNameWidget(FText::FromString("Spawn Condition"));


	TAttribute<EVisibility> HeightStatsVisibility = TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FCustomBiomeStatsDetails::IsHeightBiome, BiomeProperty, EBiomeStats::HeightBased));
	IDetailPropertyRow& MaxSpawnHeight = BiomeCategory.AddProperty(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UBiomeStatsObject, MaxSpawnHeight)));
	IDetailPropertyRow& MinSpawnHeight = BiomeCategory.AddProperty(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UBiomeStatsObject, MinSpawnHeight)));

	MaxSpawnHeight.Visibility(HeightStatsVisibility);
	MinSpawnHeight.Visibility(HeightStatsVisibility);

}

EVisibility FCustomBiomeStatsDetails::IsHeightBiome(TSharedPtr<IPropertyHandle> Property, EBiomeStats::Type DesiredMode) const
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
