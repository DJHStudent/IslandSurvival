// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

bool ULobbyWidget::Initialize() //run when the widget gets created
{
	Super::Initialize();

	ButtonLeave->OnClicked.AddDynamic(this, &ULobbyWidget::OnLeaveButtonPressed);
	ButtonStart->OnClicked.AddDynamic(this, &ULobbyWidget::OnStartButtonPressed);

	SpinBoxWidth->OnValueChanged.AddDynamic(this, &ULobbyWidget::OnWidthChanged);
	SpinBoxHeight->OnValueChanged.AddDynamic(this, &ULobbyWidget::OnHeightChanged);
	SpinBoxSeed->OnValueChanged.AddDynamic(this, &ULobbyWidget::OnSeedChanged);

	MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())); //get a reference to the Game Instance using on each client
	return true;
}

void ULobbyWidget::SetEditability(APlayerController* PlayerController)
{
	AActor* PlayerOwner = PlayerController->GetOwner();
	if (PlayerOwner && PlayerOwner->GetLocalRole() == ROLE_AutonomousProxy) //if controlled player not on server
	{
		ButtonStart->Visibility = ESlateVisibility::HitTestInvisible;
		SpinBoxSeed->Visibility = ESlateVisibility::HitTestInvisible;
		SpinBoxWidth->Visibility = ESlateVisibility::HitTestInvisible;
		SpinBoxHeight->Visibility = ESlateVisibility::HitTestInvisible;
		UE_LOG(LogTemp, Error, TEXT("Successfully Stopped Editing Ability"))
	}
	else if(PlayerOwner)
		UE_LOG(LogTemp, Error, TEXT("Successfully Stopped Editing Ability failed as no owner exists ever"))

}

void ULobbyWidget::OnStartButtonPressed() //when called move all clients to the terrain gen map
{

}

void ULobbyWidget::OnLeaveButtonPressed() //when called remove this client from the server
{
}


void ULobbyWidget::OnSeedChanged(float InValue)
{
	int32 RoundValue = FMath::RoundToInt(InValue);
}

void ULobbyWidget::OnWidthChanged(float InValue)
{
	int32 RoundValue = FMath::RoundToInt(InValue);
}

void ULobbyWidget::OnHeightChanged(float InValue)
{
	int32 RoundValue = FMath::RoundToInt(InValue);
}
