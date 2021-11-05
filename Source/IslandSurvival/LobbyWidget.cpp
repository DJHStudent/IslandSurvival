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
	LobbyGameState = Cast<ALobbyGameState>(UGameplayStatics::GetGameState(GetWorld())); //get a reference to the Game Instance using on each client

	if (LobbyGameState) //for each widget variant set the default values
	{
		SetSeed(LobbyGameState->Seed);
		SetWidth(LobbyGameState->TerrainWidth);
		SetHeight(LobbyGameState->TerrainHeight);
	}
	return true;
}

void ULobbyWidget::SetEditability(APawn* Player) //come back to eventually once rest of code setup as no idea about this
{
	if (Player && Player->GetLocalRole() == ROLE_AutonomousProxy) //if controlled player not on server
	{
		if (ButtonStart)
		{
			ButtonStart->SetVisibility(ESlateVisibility::HitTestInvisible);
			ButtonStart->SetRenderOpacity(0.5f);
		}
		if (SpinBoxSeed)
		{
			SpinBoxSeed->SetVisibility(ESlateVisibility::HitTestInvisible);
			SpinBoxSeed->SetRenderOpacity(0.5f);
		}
		if (SpinBoxWidth)
		{
			SpinBoxWidth->SetVisibility(ESlateVisibility::HitTestInvisible);
			SpinBoxWidth->SetRenderOpacity(0.5f);
		}
		if (SpinBoxHeight)
		{
			SpinBoxHeight->SetVisibility(ESlateVisibility::HitTestInvisible);
			SpinBoxHeight->SetRenderOpacity(0.5f);
		}
		UE_LOG(LogTemp, Error, TEXT("Successfully Stopped Editing Ability: %s"), *Player->GetName())
	}
}

void ULobbyWidget::OnStartButtonPressed() //when called move all clients to the terrain gen map, can only actually be called on server 
{
	if (MainGameInstance)
	{
		if (LobbyGameState) //copy the variables to the host version
		{
			MainGameInstance->Seed = LobbyGameState->Seed;
			MainGameInstance->TerrainHeight = LobbyGameState->TerrainHeight;
			MainGameInstance->TerrainWidth = LobbyGameState->TerrainWidth;
		}
		MainGameInstance->StartGame();
	}
}

void ULobbyWidget::OnLeaveButtonPressed() //when called remove this client from the server
{
	if (MainGameInstance)
	{
		MainGameInstance->QuitLobby();
	}
}

void ULobbyWidget::SetSeed(int32 Value)
{
	SpinBoxSeed->SetValue(Value);
}
void ULobbyWidget::SetWidth(int32 Value)
{
	SpinBoxWidth->SetValue(Value);
}
void ULobbyWidget::SetHeight(int32 Value)
{
	SpinBoxHeight->SetValue(Value);
}


void ULobbyWidget::OnSeedChanged(float InValue)
{
	int32 RoundValue = FMath::RoundToInt(InValue);
	//call game state to update all clients
	LobbyGameState->Seed = RoundValue;
}

void ULobbyWidget::OnWidthChanged(float InValue)
{
	int32 RoundValue = FMath::RoundToInt(InValue);
	//call game state to update all clients
	LobbyGameState->TerrainWidth = RoundValue;
}

void ULobbyWidget::OnHeightChanged(float InValue)
{
	int32 RoundValue = FMath::RoundToInt(InValue);
	//call game state to update all clients
	LobbyGameState->TerrainHeight = RoundValue;
}
