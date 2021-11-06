// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerGameHUD.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/Engine.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

////////APlayerGameHUD::APlayerGameHUD()
////////{
////////	static ConstructorHelpers::FClassFinder<UUserWidget> PlayerHUDObject(TEXT("/Game/Widgets/PlayerHUDWidget")); //access the file location of the widget specified, so can access it in C++
////////	PlayerHUDClass = PlayerHUDObject.Class; //get the object referenced above and save it to a veriable
////////
////////	if (PlayerHUDClass != nullptr)
////////		CurrentPlayerHUDWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDClass); //spawn in a new widget
////////}
bool UPlayerGameHUD::Initialize()
{
	Super::Initialize();

	ButtonResume->OnClicked.AddDynamic(this, &UPlayerGameHUD::OnResumeButtonPressed);
	ButtonLeave->OnClicked.AddDynamic(this, &UPlayerGameHUD::OnLeaveButtonPressed);

	PauseMenu->SetVisibility(ESlateVisibility::Hidden);

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
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Escape Clicked");
	PauseMenu->SetVisibility(ESlateVisibility::Visible);
}

void UPlayerGameHUD::HidePauseMenu()
{
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Escape Clicked");
	PauseMenu->SetVisibility(ESlateVisibility::Hidden);
}

void UPlayerGameHUD::OnResumeButtonPressed()
{
	Cast<APlayerCharacter>(GetOwningPlayerPawn())->Resume();
}

void UPlayerGameHUD::OnLeaveButtonPressed()
{
	MainGameInstance->QuitLobby();
}