// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

bool ULobbyWidget::Initialize() //run when the widget gets created to setup nessesary values
{
	Super::Initialize();

	ButtonLeave->OnClicked.AddDynamic(this, &ULobbyWidget::OnLeaveButtonPressed);
	ButtonStart->OnClicked.AddDynamic(this, &ULobbyWidget::OnStartButtonPressed);

	SpinBoxWidth->OnValueChanged.AddDynamic(this, &ULobbyWidget::OnWidthChanged);
	SpinBoxHeight->OnValueChanged.AddDynamic(this, &ULobbyWidget::OnHeightChanged);
	SpinBoxSeed->OnValueChanged.AddDynamic(this, &ULobbyWidget::OnSeedChanged);

	CheckBoxSmooth->OnCheckStateChanged.AddDynamic(this, &ULobbyWidget::OnSmoothChanged);

	MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())); //get a reference to the Game Instance using on each client
	LobbyGameState = Cast<ALobbyGameState>(UGameplayStatics::GetGameState(GetWorld())); //get a reference to the Game Instance using on each client

	if (LobbyGameState) //for each widget variant set the current values to be displayed when loading into lobby
	{
		SetSeed(LobbyGameState->Seed);
		SetWidth(LobbyGameState->TerrainWidth);
		SetHeight(LobbyGameState->TerrainHeight);
		SetSmooth(LobbyGameState->bSmoothTerrain);
	}

	SetKeyboardFocus();
	bIsFocusable = true;
	return true;
}

void ULobbyWidget::SetEditability(APawn* Player) //on clients which are not the host, disable the editability of the terrain settings
{
	if (Player && Player->GetLocalRole() == ROLE_AutonomousProxy) //if controlled player not on server
	{
		if (ButtonStart)
		{
			ButtonStart->SetVisibility(ESlateVisibility::HitTestInvisible); //stop button being interacted with
			ButtonStart->SetRenderOpacity(0.5f); //make it clear to client cannot actually edit it
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
		if (CheckBoxSmooth)
		{
			CheckBoxSmooth->SetVisibility(ESlateVisibility::HitTestInvisible);
			CheckBoxSmooth->SetRenderOpacity(0.5f);
		}
	}
}

void ULobbyWidget::OnStartButtonPressed() //when called move all clients to the terrain gen map, can only actually be called on server 
{
	if (MainGameInstance)
	{
		if (LobbyGameState) //copy the variables to the host's GameInstance so not deleted when server travelling
		{
			MainGameInstance->Seed = LobbyGameState->Seed;
			MainGameInstance->TerrainHeight = LobbyGameState->TerrainHeight;
			MainGameInstance->TerrainWidth = LobbyGameState->TerrainWidth;
			MainGameInstance->bSmoothTerrain = LobbyGameState->bSmoothTerrain;
		}
		MainGameInstance->StartGame(); //begin process to setup the terrain map correctly
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
	if (SpinBoxSeed) //when seed changes on Lobby Game State update its value on this client
		SpinBoxSeed->SetValue(Value);
}
void ULobbyWidget::SetWidth(int32 Value)
{
	if (SpinBoxWidth) //when width changes on Lobby Game State update its value on this client
		SpinBoxWidth->SetValue(Value);
}
void ULobbyWidget::SetHeight(int32 Value)
{
	if (SpinBoxHeight) //when height changes on Lobby Game State update its value on this client
		SpinBoxHeight->SetValue(Value);
}
void ULobbyWidget::SetSmooth(bool Value)
{
	if (CheckBoxSmooth) //when smooth changes on Lobby Game State update its value on this client
	{
		if (Value)
			CheckBoxSmooth->SetCheckedState(ECheckBoxState::Checked);
		else
			CheckBoxSmooth->SetCheckedState(ECheckBoxState::Unchecked);
	}
}


void ULobbyWidget::OnSeedChanged(float InValue)
{ //if the host has actually changed the value of the seed, update the LobbyGameState with the new value
	int32 RoundValue = FMath::RoundToInt(InValue); //as spin box is a float, convert it to an int
	if (LobbyGameState)
		LobbyGameState->Seed = RoundValue;
}

void ULobbyWidget::OnWidthChanged(float InValue)
{ //if the host has actually changed the value of the width, update the LobbyGameState with the new value
	int32 RoundValue = FMath::RoundToInt(InValue); //as spin box is a float, convert it to an int
	if (LobbyGameState)
		LobbyGameState->TerrainWidth = RoundValue;
}

void ULobbyWidget::OnHeightChanged(float InValue)
{ //if the host has actually changed the value of the height, update the LobbyGameState with the new value
	int32 RoundValue = FMath::RoundToInt(InValue); //as spin box is a float, convert it to an int
	if (LobbyGameState)
		LobbyGameState->TerrainHeight = RoundValue;
}

void ULobbyWidget::OnSmoothChanged(bool bIsChecked)
{
	//if the host has actually changed the value of the smooth, update the LobbyGameState with the new value
	if (LobbyGameState)
		LobbyGameState->bSmoothTerrain = bIsChecked;
}