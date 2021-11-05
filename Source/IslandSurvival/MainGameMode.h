// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MainGameState.h"
#include "CurrentPlayerController.h"
#include "MainGameState.h"
#include "MainGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDSURVIVAL_API AMainGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	void UpdateTerrainValues(int32 Seed, int32 Width, int32 Height);

private:
	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessages) override;
	//AMainGameState* GameState;

	virtual void PostSeamlessTravel() override;
	void PlayerTerrainUpdate();

	AMainGameState* MainGameState;
	ACurrentPlayerController* HostController;

	//virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
};
