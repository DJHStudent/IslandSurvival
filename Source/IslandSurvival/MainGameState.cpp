// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameState.h"
#include "Kismet/GameplayStatics.h"
#include "MainGameInstance.h"
#include "Engine/Engine.h"

AMainGameState::AMainGameState()
{
	NetUpdateFrequency = 50;
	MaxFuelAmount = 5;
	CurrentFuelAmount = 0;
}

void AMainGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{ //setup these values to be replicated
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMainGameState, FuelPercentage);
}

void AMainGameState::ServerAddFuel()
{ //update the fuel on the server
	CurrentFuelAmount++;
	//remember as dividing ints will need to be made into floats first
	FuelPercentage = (CurrentFuelAmount / (float)MaxFuelAmount) * 100; //determine new fuel percentage
	UpdateFuelUI();

	if (CurrentFuelAmount >= MaxFuelAmount)
		MulticastWonGame();
}

void AMainGameState::UpdateFuelUI()
{
	//call to update fuel UI from game instance
	UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (MainGameInstance) //update UI with new percentage
		MainGameInstance->UpdateFuel(FuelPercentage);
}

void AMainGameState::MulticastWonGame_Implementation()
{
	UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (MainGameInstance) //update UI with won message
		MainGameInstance->WonGame();
}
