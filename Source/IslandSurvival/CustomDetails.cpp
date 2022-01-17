// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomDetails.h"
#include "Components/SceneComponent.h"
#include "Components/LightComponentBase.h"
#include "Components/LocalLightComponent.h"
#include "Misc/Attribute.h"
#include "Components/LightComponent.h"
#include "PropertyHandle.h"
#include "DetailLayoutBuilder.h"
#include "IDetailPropertyRow.h"
#include "DetailCategoryBuilder.h"

#include "CustomDetailsTestActor.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorDelegates.h"


//#include "IDetailsView.h"
//#include "DetailLayoutBuilder.h"
//#include "DetailCategoryBuilder.h"
//#include "Widgets/Colors/SColorPicker.h"
//#include "Widgets/SBoxPanel.h"
//#include "DetailWidgetRow.h"

////CustomDetails::CustomDetails()
////{
////}
////
////CustomDetails::~CustomDetails()
////{
////}
////

TSharedRef<IDetailCustomization> FCustomDetails::MakeInstance()
{
	return MakeShareable(new FCustomDetails);
}
void FCustomDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder) 
{
	//basically here bind delegate to the appropriate class
	
	// Edit the lighting category(from other class), give it a new name to be displayed with and ECategoryPriotity(change where it appears in list)
	IDetailCategoryBuilder& LightingCategory = DetailBuilder.EditCategory("Lighting", FText::FromString("Light Stuff"), ECategoryPriority::Important);

	//Get a list of the actual objects selected
	ACustomDetailsTestActor* Actor = nullptr;
	TArray<TWeakObjectPtr<UObject>> CustomizedObjects;
	DetailBuilder.GetObjectsBeingCustomized(CustomizedObjects);

	for (auto Object : CustomizedObjects)
	{
		if (Object.IsValid()) //check to find the appropriate object has been selected
		{
			Actor = Cast<ACustomDetailsTestActor>(Object);
			if (Actor)
				break;
		}
	}

	check(Actor); //ensure that an appropriate actor has been selected so it can be customized and not crash the engine







	// Add a property to the category.  The first param is the name of the property and the second is an optional display name override.
	LightingCategory.AddProperty("bCastStaticShadow", ACustomDetailsTestActor::StaticClass(), TEXT("Static"));
	LightingCategory.AddProperty("bCastDynamicShadow", ACustomDetailsTestActor::StaticClass(), TEXT("Dynamic"));
	LightingCategory.AddProperty("bCastVolumetricTranslucentShadow", ACustomDetailsTestActor::StaticClass(), TEXT("Volumetric"));

}