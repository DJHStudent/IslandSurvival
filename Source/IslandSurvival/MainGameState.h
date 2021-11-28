// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "MainGameState.generated.h"

/**
 the state of the current level / game in
 */
UCLASS()
class ISLANDSURVIVAL_API AMainGameState : public AGameState
{ //empty state, but used when in the Terrain map
	GENERATED_BODY()
protected:
	AMainGameState();

public:
	void ServerAddFuel();
	int32 MaxFuelAmount;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
private:
	int32 CurrentFuelAmount;
	UPROPERTY(ReplicatedUsing = UpdateFuelUI)
	float FuelPercentage;

	UFUNCTION()
	void UpdateFuelUI();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastWonGame();
};
