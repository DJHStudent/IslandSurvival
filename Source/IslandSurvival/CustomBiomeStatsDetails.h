// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomDetails.h"
#include "BiomeStats.h"

/**
 the custom details for the biome blueprint class
 */
class FCustomBiomeStatsDetails : public FCustomDetails
{
public:
	void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;//main method running when custom details occur
	
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();
private:
	EVisibility PropertyMatchesEnum(TSharedPtr<IPropertyHandle> Property, EBiomeStats::Type DesiredMode) const;
	EVisibility PropertiesLandWater(TSharedPtr<IPropertyHandle> Property) const;
	EVisibility PropertiesSingleLandWater(TSharedPtr<IPropertyHandle> PropertyEnum, TSharedPtr<IPropertyHandle> PropertyBool) const;
};
