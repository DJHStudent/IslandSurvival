// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "Kismet/GameplayStatics.h"

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
	UE_LOG(LogTemp, Warning, TEXT("Host Button Pressed"))
	if (MainGameInstance)
		MainGameInstance->HostSession();
}

void UMainMenuWidget::OnJoinButtonPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Join Button Pressed"))
	if (MainGameInstance)
		MainGameInstance->JoinSession();
}

void UMainMenuWidget::OnQuitButtonPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Quit Button Pressed"))
}