// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerGameHUD.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

bool UPlayerGameHUD::Initialize()
{
	Super::Initialize();

	//setup delegates to be called when button clicked
	ButtonResume->OnClicked.AddDynamic(this, &UPlayerGameHUD::OnResumeButtonPressed);
	ButtonLeave->OnClicked.AddDynamic(this, &UPlayerGameHUD::OnLeaveButtonPressed);

	if (PauseMenu)
		PauseMenu->SetVisibility(ESlateVisibility::Hidden); //ensure hidden when widget loads in
	MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())); //get game instance for client on
	return true;
}
void UPlayerGameHUD::UpdateBiomeTextBlock(FString Text) //each tick when called, will update the text displaying the biome a player is currently in
{
	TextBlockBiome->SetText(FText::FromString(Text));
}

void UPlayerGameHUD::UpdateFuelValue(float Percentage)
{ 
	//update the fuels UI values
	FuelProgressBar->SetPercent(Percentage / 100);
	FString Text = FString::FromInt(FMath::CeilToInt(Percentage));
	TextBlockFuel->SetText(FText::FromString("Fuel: " + Text + "%"));
}

void UPlayerGameHUD::UpdateSeedTextBlock(FString Text) //when seed gotten from the server, update it so player can see what the seed was
{
	TextBlockSeed->SetText(FText::FromString("Seed: " + Text));
}

void UPlayerGameHUD::ShowPauseMenu()
{
	if(PauseMenu)
		PauseMenu->SetVisibility(ESlateVisibility::Visible); //show the pause menu
}

void UPlayerGameHUD::HidePauseMenu() //hide the pause menu
{
	if (PauseMenu)
		PauseMenu->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerGameHUD::HideLoading() //hide the pause menu
{
	if (Loading)
		Loading->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerGameHUD::OnResumeButtonPressed() //on playercharacter, allow it to resume playing as pause completed
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwningPlayerPawn());
	if (Player)
		Player->Resume();
}

void UPlayerGameHUD::OnLeaveButtonPressed() //if leaving the session
{
	if (MainGameInstance)
		MainGameInstance->QuitLobby(); //call appropriate function on client to leave session
}

void UPlayerGameHUD::WonGame() //Update UI when all fuel collected
{
	ShowPauseMenu();
	if (ButtonResume)
		ButtonResume->SetVisibility(ESlateVisibility::Collapsed);
	FText Message = FText::FromString("Ship Refueled, Match Over");
	if (TextBlockPause)
		TextBlockPause->SetText(Message);
}