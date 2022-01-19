// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "PropertyHandle.h"
#include "BiomeStats.h"

/**
 * 
 */
class FCustomBiomeStatsDetails : public IDetailCustomization
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;//main method running when custom details occur

private:
	EVisibility IsHeightBiome(TSharedPtr<IPropertyHandle> Property, EBiomeStats::Type DesiredMode) const;

};
