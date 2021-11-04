// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerGameHUD.h"
#include "UObject/ConstructorHelpers.h"

////////APlayerGameHUD::APlayerGameHUD()
////////{
////////	static ConstructorHelpers::FClassFinder<UUserWidget> PlayerHUDObject(TEXT("/Game/Widgets/PlayerHUDWidget")); //access the file location of the widget specified, so can access it in C++
////////	PlayerHUDClass = PlayerHUDObject.Class; //get the object referenced above and save it to a veriable
////////
////////	if (PlayerHUDClass != nullptr)
////////		CurrentPlayerHUDWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDClass); //spawn in a new widget
////////}

void UPlayerGameHUD::UpdateBiomeTextBlock(FString Text)
{
	TextBlockBiome->SetText(FText::FromString(Text));
}

void UPlayerGameHUD::UpdateSeedTextBlock(FString Text)
{
	TextBlockSeed->SetText(FText::FromString("Seed: " + Text));
}