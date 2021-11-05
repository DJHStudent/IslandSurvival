// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDSURVIVAL_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()

private:
	ALobbyGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	UMaterialInterface* HostColour;
	TArray<UMaterialInterface*> PlayerColours;
};