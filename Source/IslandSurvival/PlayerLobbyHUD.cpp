// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerLobbyHUD.h"
#include "UObject/ConstructorHelpers.h"

APlayerLobbyHUD::APlayerLobbyHUD()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> LobbyWidgetObject(TEXT("/Game/Widgets/LobbyWidget"));
	LobbyWidgetClass = LobbyWidgetObject.Class; //get the file location of the widget blueprint class and store it in this variable
}

void APlayerLobbyHUD::SetEditability()
{
	AActor* PlayerOwner = GetOwner();
	if (PlayerOwner && PlayerOwner->GetLocalRole() == ROLE_AutonomousProxy) //if controlled player not on server
	{
		ButtonStart->Visibility = ESlateVisibility::HitTestInvisible;
		SpinBoxSeed->Visibility = ESlateVisibility::HitTestInvisible;
		SpinBoxWidth->Visibility = ESlateVisibility::HitTestInvisible;
		SpinBoxHeight->Visibility = ESlateVisibility::HitTestInvisible;
		UE_LOG(LogTemp, Error, TEXT("Successfully Stopped Editing Ability"))
	}
	else if (PlayerOwner)
		UE_LOG(LogTemp, Error, TEXT("Successfully Stopped Editing Ability failed as no owner exists ever"))

}

void APlayerLobbyHUD::OnStartButtonPressed() //when called move all clients to the terrain gen map
{

}

void APlayerLobbyHUD::OnLeaveButtonPressed() //when called remove this client from the server
{
}


void APlayerLobbyHUD::OnSeedChanged(float InValue)
{
	int32 RoundValue = FMath::RoundToInt(InValue);
}

void APlayerLobbyHUD::OnWidthChanged(float InValue)
{
	int32 RoundValue = FMath::RoundToInt(InValue);
}

void APlayerLobbyHUD::OnHeightChanged(float InValue)
{
	int32 RoundValue = FMath::RoundToInt(InValue);
}