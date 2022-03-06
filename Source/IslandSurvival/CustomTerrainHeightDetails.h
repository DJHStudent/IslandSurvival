// Fill out your copyright notice in the Description page of Project Settings.
#if WITH_EDITOR
#pragma once

#include "CoreMinimal.h"
#include "CustomDetails.h"

/**
 * 
 */
class FCustomTerrainHeightDetails : public FCustomDetails
{
public:
	void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;//main method running when custom details occur

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();
};

#endif