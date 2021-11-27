// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameState.h"
#include "Kismet/GameplayStatics.h"
#include "MainGameInstance.h"

AMainGameState::AMainGameState()
{
	NetUpdateFrequency = 50;
	MaxFuelAmount = 5;
	CurrentFuelAmount = 0;
}

void AMainGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{ //setup these values to be replicated
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMainGameState, CurrentFuelAmount);
}

void AMainGameState::ServerAddFuel()
{ //update the fuel on the server
	CurrentFuelAmount++;
	UpdateFuelUI();
}

void AMainGameState::UpdateFuelUI()
{
	//call to update fuel UI from game instance
	UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	if (MainGameInstance)
	{
		float Percentage = CurrentFuelAmount / MaxFuelAmount * 100;
		MainGameInstance->UpdateFuel(Percentage);
	}


}
