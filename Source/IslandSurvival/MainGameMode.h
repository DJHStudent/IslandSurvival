// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MainGameState.h"
#include "MainGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDSURVIVAL_API AMainGameMode : public AGameMode
{
	GENERATED_BODY()

private:
	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessages) override;

	AMainGameState* GameState;
};
