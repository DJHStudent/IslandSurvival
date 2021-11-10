// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameState.h"

#include "LobbyWidget.h"
#include "MainGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

ALobbyGameState::ALobbyGameState()
{
	//assign the default starting values
	TerrainWidth = 300;
	TerrainHeight = 300;
	Seed = 0;
	bSmoothTerrain = false;
}

void ALobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{ //setup these values to be replicated
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ALobbyGameState, TerrainWidth);
	DOREPLIFETIME(ALobbyGameState, TerrainHeight);
	DOREPLIFETIME(ALobbyGameState, Seed);
	DOREPLIFETIME(ALobbyGameState, bSmoothTerrain);
}

void ALobbyGameState::UpdateHeight()
{	//as server height changed, on this client update the Lobby widget UI to reflect this change
	UMainGameInstance* PlayerInstance = Cast<UMainGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
	if (PlayerInstance && PlayerInstance->Lobby) //if the lobby UI exists in the player GameInstance class
		PlayerInstance->Lobby->SetHeight(TerrainHeight);
}

void ALobbyGameState::UpdateWidth()
{ //as server width changed, on this client update the Lobby widget UI to reflect this change
	UMainGameInstance* PlayerInstance = Cast<UMainGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
	if (PlayerInstance && PlayerInstance->Lobby) //if the lobby UI exists in the player GameInstance class
		PlayerInstance->Lobby->SetWidth(TerrainWidth);
}

void ALobbyGameState::UpdateSeed()
{ //as server seed changed, on this client update the Lobby widget UI to reflect this change
	UMainGameInstance* PlayerInstance = Cast<UMainGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
	if (PlayerInstance && PlayerInstance->Lobby)//if the lobby UI exists in the player GameInstance class
		PlayerInstance->Lobby->SetSeed(Seed);
}

void ALobbyGameState::UpdateSmooth()
{ //as server smooth changed, on this client update the Lobby widget UI to reflect this change
	UMainGameInstance* PlayerInstance = Cast<UMainGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
	if (PlayerInstance && PlayerInstance->Lobby) //if the lobby UI exists in the player GameInstance class
		PlayerInstance->Lobby->SetSmooth(bSmoothTerrain);
}