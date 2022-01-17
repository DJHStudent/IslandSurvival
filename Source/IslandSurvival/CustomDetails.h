// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
//#include "DetailLayoutBuilder.h"
//#include "IPropertyTypeCustomization.h"
//#include "ProcedurallyGeneratedTerrain.h" //the class customizing the details of
/**
 * 
 */

class IDetailLayoutBuilder;
class IPropertyHandle;
class ULightComponent;

class FCustomDetails : public IDetailCustomization
{
////public:
////////	CustomDetails();
////////	~CustomDetails();
////////
////////	static TSharedRef<IDetailCustomization> MakeInstance();
////////
public:
	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance();


	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;//main method running when custom details occur
////////
////////private:
////////	TArray<TWeakObjectPtr<UObject>> ObjectsToEdit; //all the objects selected which are being edited
////
////public:
////    //virtual void CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) override;
////    //void ColorPicked(FLinearColor SelectedColor);
////    ////static TSharedRef<IDetailCustomization> FCustomDetails::MakeInstance()
////    ////{
////    ////    return MakeShareable(new FCustomDetails);
////    ////}
////    //TWeakObjectPtr<class UMyCustomAsset> MyAsset;
////
};