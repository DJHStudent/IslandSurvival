// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomTerrainHeightDetails.h"
#include "DetailLayoutBuilder.h"
#include "IDetailPropertyRow.h"
#include "DetailCategoryBuilder.h"
#include "PropertyEditorDelegates.h"
#include "PropertyCustomizationHelpers.h"
#include "ClassViewerModule.h"
#include "Engine/Selection.h"
#include "Editor.h"


#include "TerrainHeight.h"

TSharedRef<IDetailCustomization> FCustomTerrainHeightDetails::MakeInstance()
{
	return MakeShareable(new FCustomTerrainHeightDetails);
}

void FCustomTerrainHeightDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
	IDetailCategoryBuilder& FBMCategory = DetailBuilder.EditCategory("FBM", FText::FromString("Noise Layers"), ECategoryPriority::Default);


	IDetailCategoryBuilder& WarpingCategory = DetailBuilder.EditCategory("Domain Warping", FText::FromString("Domain Warping"), ECategoryPriority::Default);
	//setup elements for when a value changes within the editor
	TSharedPtr<IPropertyHandle> bDoWarpingProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UTerrainHeight, bDoWarping));
	IDetailPropertyRow& DomainAmount = WarpingCategory.AddProperty(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UTerrainHeight, DomainAmount)));
	DomainAmount.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FCustomDetails::PropertyMatchesBool, bDoWarpingProperty)));

	IDetailCategoryBuilder& TerraceCategory = DetailBuilder.EditCategory("Terraces", FText::FromString("Biome Terracing"), ECategoryPriority::Default);
	TSharedPtr<IPropertyHandle> bDoTerracingProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UTerrainHeight, bDoTerrace));
	IDetailPropertyRow& TerraceSize = TerraceCategory.AddProperty(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UTerrainHeight, TerraceSize)));
	TerraceSize.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FCustomDetails::PropertyMatchesBool, bDoTerracingProperty)));


	IDetailCategoryBuilder& FalloffCategory = DetailBuilder.EditCategory("FallOff", FText::FromString("Island Settings"), ECategoryPriority::Default);
	TSharedPtr<IPropertyHandle> bDoFalloffProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UTerrainHeight, bDoFalloff));
	TAttribute<EVisibility> FalloffStatsVisibility = TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FCustomDetails::PropertyMatchesBool, bDoFalloffProperty));
	//the different falloff stats visability setting
	FalloffCategory.AddProperty(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UTerrainHeight, Steepness))).Visibility(FalloffStatsVisibility);
	FalloffCategory.AddProperty(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UTerrainHeight, Size))).Visibility(FalloffStatsVisibility);
	FalloffCategory.AddProperty(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(UTerrainHeight, AboveWater))).Visibility(FalloffStatsVisibility);


}