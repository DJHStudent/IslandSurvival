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
	MainGameState = Cast<AMainGameState>(UGameplayStatics::GetGameState(GetWorld())); //get a reference to the Game Instance using on each client
	return true;
}

void ULobbyWidget::SetEditability(AActor* Player) //come back to eventually once rest of code setup as no idea about this
{
	if (Player && Player->GetLocalRole() == ROLE_AutonomousProxy) //if controlled player not on server
	{
		if(ButtonStart)
			ButtonStart->SetVisibility(ESlateVisibility::HitTestInvisible);
		if(SpinBoxSeed)
			SpinBoxSeed->SetVisibility(ESlateVisibility::HitTestInvisible);
		if (SpinBoxWidth)
			SpinBoxWidth->SetVisibility(ESlateVisibility::HitTestInvisible);
		if (SpinBoxHeight)
			SpinBoxHeight->SetVisibility(ESlateVisibility::HitTestInvisible);
		UE_LOG(LogTemp, Error, TEXT("Successfully Stopped Editing Ability: %s"), *Player->GetName())
	}
	else if(Player)
		UE_LOG(LogTemp, Error, TEXT("Successfully Stopped Editing Ability failed as no owner exists ever"))

}

void ULobbyWidget::OnStartButtonPressed() //when called move all clients to the terrain gen map
{

}

void ULobbyWidget::OnLeaveButtonPressed() //when called remove this client from the server
{
}

void ULobbyWidget::SetHeight(int32 Value)
{
	if (SpinBoxHeight != nullptr)
		SpinBoxHeight->SetValue(Value);
}


void ULobbyWidget::OnSeedChanged(float InValue)
{
	int32 RoundValue = FMath::RoundToInt(InValue);
	//call game state to update all clients
	MainGameState->Seed = RoundValue;
}

void ULobbyWidget::OnWidthChanged(float InValue)
{
	int32 RoundValue = FMath::RoundToInt(InValue);
	//call game state to update all clients
	MainGameState->TerrainWidth = RoundValue;
}

void ULobbyWidget::OnHeightChanged(float InValue)
{
	int32 RoundValue = FMath::RoundToInt(InValue);
	//call game state to update all clients
	MainGameState->TerrainHeight = RoundValue;
}
