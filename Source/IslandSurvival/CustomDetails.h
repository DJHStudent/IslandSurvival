// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IDetailCustomization.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "PropertyHandle.h"
#include "CustomDetailsTestActor.h"

//#include "DetailLayoutBuilder.h"
//#include "IPropertyTypeCustomization.h"
//#include "ProcedurallyGeneratedTerrain.h" //the class customizing the details of
/**
 * 
 */

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
	//TSharedRef<SWidget> OnGetComboContent() const;

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
private:
	/** Retrieves the content for the Convert combo button */
	//TSharedRef<SWidget> OnGetConvertContent();

	/**
	 * Callback from the Class Picker when a class is picked.
	 *
	 * @param ChosenClass		The class chosen to convert to
	 */
	TSharedRef<SWidget> OnGetComboContent() const;
	TSharedPtr<STextBlock> AspectTextBox; //the name of the button, changeable whenever property changes
	void CommitAspectRatioText(FText ItemText);
	void OnCommitAspectRatioText(const FText& ItemFText, ETextCommit::Type CommitInfo);

	EVisibility ProjectionModeMatches(TSharedPtr<IPropertyHandle> Property, ELightOptions::Type DesiredMode) const;


};