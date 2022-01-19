// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomDetails.h"
#include "Components/SceneComponent.h"
#include "Components/LightComponentBase.h"
#include "Components/LocalLightComponent.h"
#include "Misc/Attribute.h"
#include "Components/LightComponent.h"
#include "DetailLayoutBuilder.h"
#include "IDetailPropertyRow.h"
#include "DetailCategoryBuilder.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorDelegates.h"
#include "PropertyCustomizationHelpers.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Input/SComboBox.h"
#include "ClassViewerModule.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Engine/Selection.h"
#include "Editor.h"



TSharedRef<IDetailCustomization> FCustomDetails::MakeInstance()
{
	return MakeShareable(new FCustomDetails);
}
void FCustomDetails::CustomizeDetails(IDetailLayoutBuilder& DetailBuilder)
{
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
	IDetailCategoryBuilder& LightingCategory = DetailBuilder.EditCategory("Lighting", FText::FromString("Light Stuff"), ECategoryPriority::Important);

	if (CustomizedObjects.Num() == 1)
	{
		TSharedPtr<IPropertyHandle> ProjectionModeProperty = DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(ACustomDetailsTestActor, LightOptionsEnum));

		// Perspective-specific properties


		IDetailPropertyRow& bCastStaticShadowRow = LightingCategory.AddProperty(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(ACustomDetailsTestActor, bCastStaticShadow)));
		bCastStaticShadowRow.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FCustomDetails::ProjectionModeMatches, ProjectionModeProperty, ELightOptions::StaticShadow)));


		// Perspective-specific properties
		IDetailPropertyRow& bCastDynamicShadowRow = LightingCategory.AddProperty(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(ACustomDetailsTestActor, DynamicAmount)));
		TAttribute<EVisibility> OrthographicVisibility = TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FCustomDetails::ProjectionModeMatches, ProjectionModeProperty, ELightOptions::DynamicShadow));
		bCastDynamicShadowRow.Visibility(OrthographicVisibility);


		// Perspective-specific properties
		IDetailPropertyRow& bCastShadowRow = LightingCategory.AddProperty(DetailBuilder.GetProperty(GET_MEMBER_NAME_CHECKED(ACustomDetailsTestActor, VolumetricLightName)));
		bCastShadowRow.Visibility(TAttribute<EVisibility>::Create(TAttribute<EVisibility>::FGetter::CreateSP(this, &FCustomDetails::ProjectionModeMatches, ProjectionModeProperty, ELightOptions::VolumetricTranslucentShadow)));



		//basically here bind delegate to the appropriate class

		// Edit the lighting category(from other class), give it a new name to be displayed with and ECategoryPriotity(change where it appears in list)
		// Get a handle to the bOverrideLightmapRes property
//		TSharedPtr<IPropertyHandle> OverrideLightmapRes = DetailBuilder.GetProperty("LightOptionsEnum");//GetProperty("bOverrideLightmapRes");

		LightingCategory.AddCustomRow(FText::FromString("Magic"))
			.NameContent()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Light Types"))
			]
		.ValueContent()
			[
				SNew(SComboButton)
				.ContentPadding(2)
			.OnGetMenuContent(this, &FCustomDetails::OnGetComboContent)
			.ButtonContent() //the actual name and hover features of the button
			[
				SAssignNew(AspectTextBox, STextBlock)
				.Text(FText::FromString("Static"))
				.ToolTipText(FText::FromString("Enter a ratio in the form \'width x height\' or \'width:height\'"))
				//.Text_Lambda([this]()->void { //FText(AspectTextBox->GetText()); 
				//	UE_LOG(LogTemp, Warning, TEXT("Text is being updated so also update the editor with the new values"))
				//	})
			]
			];

		////////OverrideLightmapRes = AspectTextBox->GetText();


		//		.OnGetMenuContent(this, &FCustomDetails::OnGetComboContent)
		//	.ContentPadding(0.0f)
		//	.ButtonStyle(FEditorStyle::Get(), "ToggleButton")
		//	.ForegroundColor(FSlateColor::UseForeground())
		//	.VAlign(VAlign_Center)
		//	.ButtonContent()
		//	[
		//		SAssignNew(AspectTextBox, SEditableTextBox)
		//		.HintText(LOCTEXT("AspectTextHint", "width x height"))
		//	.ToolTipText(LOCTEXT("AspectTextTooltip", "Enter a ratio in the form \'width x height\' or \'width:height\'"))
		//	.Font(FontStyle)
		//	.OnTextCommitted(this, &FCameraDetails::OnCommitAspectRatioText)
		////		// Make a new SProperty
		//		//SNew(SComboBox<TSharedPtr<bool>>)
		//		//.OptionsSource("LightOptionsArray", A)
		////		///////*[
		////		//////	SNew(SComboBoxRow)
		////		//////	.Text(FText::FromString("one"))
		////		//////]
		////		//////[
		////		//////	SNew(STextBlock)
		////		//////	.Text(FText::FromString("two"))
		////		//////]*/

		// Add a property to the category.  The first param is the name of the property and the second is an optional display name override.
		LightingCategory.AddProperty("bCastStaticShadow", ACustomDetailsTestActor::StaticClass(), FName("Static"), EPropertyLocation::Advanced);
		LightingCategory.AddProperty("bCastDynamicShadow", ACustomDetailsTestActor::StaticClass(), FName("Dynamic"));
		LightingCategory.AddProperty("bCastVolumetricTranslucentShadow", ACustomDetailsTestActor::StaticClass(), FName("Volumetric"));


		//Get a list of the actual objects selected
	}
	else //can only customize 1 object ever
	{

		LightingCategory.AddCustomRow(FText::FromString("Magic"))
			.NameContent()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Warning"))
			]
		.ValueContent()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Only one can be edited at a time"))
			];
	}
}

//making a custom combo box list thing
TSharedRef<SWidget> FCustomDetails::OnGetComboContent() const //make the list of combo box items to choose from
{
	// Fill the combo menu with presets of common screen resolutions
	FMenuBuilder MenuBuilder(true, NULL);

	TArray<FText> Items;
	Items.Add(FText::FromString("Static"));
	Items.Add(FText::FromString("Dynamic"));
	Items.Add(FText::FromString("Volumetric"));

	for (auto ItemIter = Items.CreateConstIterator(); ItemIter; ++ItemIter)
	{
		FText ItemText = *ItemIter;
		FUIAction ItemAction(FExecuteAction::CreateSP(const_cast<FCustomDetails*>(this), &FCustomDetails::CommitAspectRatioText, ItemText));
		MenuBuilder.AddMenuEntry(ItemText, FText::FromString("Life is fun"), FSlateIcon(), ItemAction);
	}

	return MenuBuilder.MakeWidget();
}

void FCustomDetails::CommitAspectRatioText(FText ItemText)
{
	// placing new text into the box - so set the actual text then run the 'oncommit' handler
	AspectTextBox->SetText(ItemText);
	USelection* SelectedActors = GEditor->GetSelectedActors(); //get all actors currently selected and update appropriate script
	if (SelectedActors->Num() > 0)
	{
		ACustomDetailsTestActor* Selected = Cast<ACustomDetailsTestActor>(&SelectedActors[0]);
		if (Selected)
		{
			Selected->SetValues(ItemText);
			UE_LOG(LogTemp, Warning, TEXT("Text is being updated so also update the editor with the new values"))
		}
	}

	OnCommitAspectRatioText(ItemText, ETextCommit::Default);
}

void FCustomDetails::OnCommitAspectRatioText(const FText& ItemFText, ETextCommit::Type CommitInfo)
{




	// Parse the text assuming the following format
	// <INTEGER><optional whitespace><x or : or /><optional whitespace><INTEGER><optional extra info>
	FString ItemText = ItemFText.ToString();
	float ParsedRatio = -1.0f;
	int32 DelimIdx = INDEX_NONE;


	if (!ItemText.FindChar(TCHAR('x'), DelimIdx))
	{
		if (!ItemText.FindChar(TCHAR('X'), DelimIdx))
		{
			if (!ItemText.FindChar(TCHAR(':'), DelimIdx))
			{
				ItemText.FindChar(TCHAR('/'), DelimIdx);
			}
		}
	}

	if (DelimIdx != INDEX_NONE)
	{
		int32 Width;
		TTypeFromString<int32>::FromString(Width, *ItemText.Mid(0, DelimIdx).TrimStartAndEnd());
		if (Width > 0)
		{
			int32 WSIdx;
			FString RemainingText = ItemText.Mid(DelimIdx + 1).TrimStart();
			if (RemainingText.FindChar(TCHAR(' '), WSIdx))
			{
				RemainingText = RemainingText.Left(WSIdx);
			}
			int32 Height;
			TTypeFromString<int32>::FromString(Height, *RemainingText);
			if (Height > 0)
			{
				ParsedRatio = (float)Width / (float)Height;
			}
		}
	}

	//if (ParsedRatio < 0.0f)
	//{
	//	// invalid text - value couldn't be read
	//}
	//else if (ParsedRatio < MinAspectRatio)
	//{
	//	// invalid value - too small
	//}
	//else if (ParsedRatio > MaxAspectRatio)
	//{
	//	// invalid value - too large
	//}
	//else
	//{
	//	// valid ratio parsed from text
	//	LastParsedAspectRatioValue = ParsedRatio;
	//	AspectRatioProperty->SetValue(ParsedRatio);
	//}
}

EVisibility FCustomDetails::ProjectionModeMatches(TSharedPtr<IPropertyHandle> Property, ELightOptions::Type DesiredMode) const
{
	if (Property.IsValid())
	{
		uint8 ValueAsByte;
		FPropertyAccess::Result Result = Property->GetValue(/*out*/ ValueAsByte);

		if (Result == FPropertyAccess::Success)
		{
			return (((ELightOptions::Type)ValueAsByte) == DesiredMode) ? EVisibility::Visible : EVisibility::Collapsed;
		}
	}

	// If there are multiple values, show all properties
	return EVisibility::Visible;
}