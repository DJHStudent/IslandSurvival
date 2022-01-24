// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CustomDetails.h"

/**
 the custom details for the procedural terrain actor class
 */
class FCustomProceduralTerrainDetails : public FCustomDetails
{
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();

	void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;//main method running when custom details occur
};
