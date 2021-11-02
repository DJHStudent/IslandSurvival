// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameState.h"
#include "Net/UnrealNetwork.h"

void AMainGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMainGameState, TerrainWidth);	
	DOREPLIFETIME(AMainGameState, TerrainHeight);
	DOREPLIFETIME(AMainGameState, Seed);
}