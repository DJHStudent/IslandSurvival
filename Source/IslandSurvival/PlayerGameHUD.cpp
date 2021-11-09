// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerGameHUD.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

bool UPlayerGameHUD::Initialize()
{
	Super::Initialize();

	ButtonResume->OnClicked.AddDynamic(this, &UPlayerGameHUD::OnResumeButtonPressed);
	ButtonLeave->OnClicked.AddDynamic(this, &UPlayerGameHUD::OnLeaveButtonPressed);
	if (PauseMenu)
	{
		PauseMenu->SetVisibility(ESlateVisibility::Hidden);
	}
	MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	return true;
}
void UPlayerGameHUD::UpdateBiomeTextBlock(FString Text)
{
	TextBlockBiome->SetText(FText::FromString(Text));
}

void UPlayerGameHUD::UpdateSeedTextBlock(FString Text)
{
	TextBlockSeed->SetText(FText::FromString("Seed: " + Text));
}

void UPlayerGameHUD::ShowPauseMenu()
{
	/*if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Escape Clicked");*/
	if(PauseMenu)
		PauseMenu->SetVisibility(ESlateVisibility::Visible);
	/*else
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Escape Clicked");*/
}

void UPlayerGameHUD::HidePauseMenu()
{
	/*if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Escape Clicked");*/
	if (PauseMenu)
		PauseMenu->SetVisibility(ESlateVisibility::Hidden);
	/*else
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Escape Clicked");*/
}

void UPlayerGameHUD::OnResumeButtonPressed()
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(GetOwningPlayerPawn());
	if (Player)
		Player->Resume();
}

void UPlayerGameHUD::OnLeaveButtonPressed()
{
	MainGameInstance->QuitLobby();
}