// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameState.h"
#include "LobbyWidget.h"
#include "MainGameInstance.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

void AMainGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMainGameState, TerrainWidth);	
	DOREPLIFETIME(AMainGameState, TerrainHeight);
	DOREPLIFETIME(AMainGameState, Seed);
}

void AMainGameState::UpdateHeight()
{
	UMainGameInstance* PlayerInstance = Cast<UMainGameInstance>(UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetGameInstance());
	if (PlayerInstance && PlayerInstance->Lobby)
	{
		PlayerInstance->Lobby->SetHeight(TerrainHeight);
		UE_LOG(LogTemp, Warning, TEXT("Updated Terrain Height: %i"), TerrainHeight)
	}
}