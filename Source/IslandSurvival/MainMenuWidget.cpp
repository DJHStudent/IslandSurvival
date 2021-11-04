// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

bool UMainMenuWidget::Initialize() //run when the widget gets created
{
	Super::Initialize();

	ButtonHost->OnClicked.AddDynamic(this, &UMainMenuWidget::OnHostButtonPressed);
	ButtonJoin->OnClicked.AddDynamic(this, &UMainMenuWidget::OnJoinButtonPressed);
	ButtonQuit->OnClicked.AddDynamic(this, &UMainMenuWidget::OnQuitButtonPressed);

	MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())); //get a reference to the Game Instance using on each client
	return true;
}

void UMainMenuWidget::OnHostButtonPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Host Button Pressed"));
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, "Host Button Pressed");
	if (MainGameInstance)
		MainGameInstance->HostSession();
}

void UMainMenuWidget::OnJoinButtonPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Join Button Pressed"));
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, "Join Button Pressed");
	if (MainGameInstance)
		MainGameInstance->JoinSession();
}

void UMainMenuWidget::OnQuitButtonPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Quit Button Pressed"));
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Quit Button Pressed");
	GetWorld()->GetFirstPlayerController()->ConsoleCommand("quit");
}