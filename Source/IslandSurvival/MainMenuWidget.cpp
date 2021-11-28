// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"

bool UMainMenuWidget::Initialize() //run when the widget gets created
{
	Super::Initialize();

	//setup appropriate delegates when buttons clicked
	ButtonHost->OnClicked.AddDynamic(this, &UMainMenuWidget::OnHostButtonPressed);
	ButtonJoin->OnClicked.AddDynamic(this, &UMainMenuWidget::OnJoinButtonPressed);
	ButtonQuit->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitButtonPressed);
	ButtonCancel->OnClicked.AddDynamic(this, &UMainMenuWidget::OnCancelButtonPressed);
	ButtonOK->OnClicked.AddDynamic(this, &UMainMenuWidget::OnOKButtonPressed);

	MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())); //get a reference to the Game Instance using on each client
	Loading->SetVisibility(ESlateVisibility::Hidden); //as no loading anything, ensure it is hidden

	return true;
}

void UMainMenuWidget::OnHostButtonPressed() //called when host button pressed
{
	if (MainGameInstance)
		MainGameInstance->HostSession(); //start a new session
}

void UMainMenuWidget::OnJoinButtonPressed() //when join button pressed
{
	UpdateJoiningText("Finding Nearest Session"); //tell user that code looking for session
	if (MainGameInstance)
	{
		Loading->SetVisibility(ESlateVisibility::Visible); //show message that currently looking for a session
		MainGameInstance->JoinSession(); //go about searching for and joining the session
	}
}

void UMainMenuWidget::OnQuitButtonPressed() //when quit button pressed
{
	GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit"); //close the game
}

void UMainMenuWidget::OnCancelButtonPressed() //when searching for a session and cancel button pressed
{
	ButtonCancel->SetVisibility(ESlateVisibility::Hidden); //disable buttin so cannot be pressed again
	UpdateJoiningText("Cancelling Session Joining"); //notify user cancelling search
	if (MainGameInstance)
	{
		MainGameInstance->CancelFindSession(); //actually cancel the search
		FTimerHandle CancellingTimer;
		GetWorld()->GetTimerManager().SetTimer(CancellingTimer, this, &UMainMenuWidget::HideLoadingMenu, 2.0f, false); //in 2 seconds hide cancel message
	}
}

void UMainMenuWidget::UpdateJoiningText(FString Text) //update text telling user that a session was found and they are now in process of joinning it
{
	TextBlockJoining->SetText(FText::FromString(Text));
}

void UMainMenuWidget::OnOKButtonPressed() //called when Ok button clicked after onnection crashed
{
	ErrorMenu->SetVisibility(ESlateVisibility::Hidden); //hide this error menu
	MainGameInstance->bCrashed = false; //update Player to no longer be crashed
}

void UMainMenuWidget::ShowErrorMenu() //update UI to show error message if connection failed
{
	if (ErrorMenu)
		ErrorMenu->SetVisibility(ESlateVisibility::Visible);
}

void UMainMenuWidget::HideLoadingMenu() //if searching for session and cancelled update UI
{
	Loading->SetVisibility(ESlateVisibility::Hidden);
	ButtonCancel->SetVisibility(ESlateVisibility::Visible);
}