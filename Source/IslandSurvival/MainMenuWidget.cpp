// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"

bool UMainMenuWidget::Initialize() //run when the widget gets created
{
	Super::Initialize();

	ButtonHost->OnClicked.AddDynamic(this, &UMainMenuWidget::OnHostButtonPressed);
	ButtonJoin->OnClicked.AddDynamic(this, &UMainMenuWidget::OnJoinButtonPressed);
	ButtonQuit->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitButtonPressed);

	ButtonCancel->OnClicked.AddDynamic(this, &UMainMenuWidget::OnCancelButtonPressed);

	ButtonOK->OnClicked.AddDynamic(this, &UMainMenuWidget::OnOKButtonPressed);

	MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())); //get a reference to the Game Instance using on each client
	Loading->SetVisibility(ESlateVisibility::Hidden);
	//ErrorMenu->SetVisibility(ESlateVisibility::Hidden);

	return true;
}

void UMainMenuWidget::OnHostButtonPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Host Button Pressed"));
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Host Button Pressed");
	if (MainGameInstance)
		MainGameInstance->HostSession();
}

void UMainMenuWidget::OnJoinButtonPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Join Button Pressed"));
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Join Button Pressed");
	UpdateJoiningText("Finding Nearest Session");
	if (MainGameInstance)
	{
		Loading->SetVisibility(ESlateVisibility::Visible);
		MainGameInstance->JoinSession();
	}
}

void UMainMenuWidget::OnQuitButtonPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Quit Button Pressed"));
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Quit Button Pressed");
	GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
}

void UMainMenuWidget::OnCancelButtonPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Cancel Button Pressed"));
	ButtonCancel->SetVisibility(ESlateVisibility::Hidden);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Cancel Button Pressed");
	UpdateJoiningText("Cancelling Session Joining");
	if (MainGameInstance)
	{
		MainGameInstance->CancelFindSession();
		FTimerHandle CancellingTimer;
		GetWorld()->GetTimerManager().SetTimer(CancellingTimer, this, &UMainMenuWidget::HideLoadingMenu, 2.0f, false);
	}
}

void UMainMenuWidget::UpdateJoiningText(FString Text)
{
	TextBlockJoining->SetText(FText::FromString(Text));

}

void UMainMenuWidget::OnOKButtonPressed()
{
	ErrorMenu->SetVisibility(ESlateVisibility::Hidden);
	MainGameInstance->bCrashed = false;
}

void UMainMenuWidget::ShowErrorMenu()
{
	ErrorMenu->SetVisibility(ESlateVisibility::Visible);
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Show Error Menu");
}

void UMainMenuWidget::HideLoadingMenu()
{
	Loading->SetVisibility(ESlateVisibility::Hidden);
	ButtonCancel->SetVisibility(ESlateVisibility::Visible);
}