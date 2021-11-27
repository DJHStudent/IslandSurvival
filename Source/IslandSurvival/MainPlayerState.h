// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MainPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDSURVIVAL_API AMainPlayerState : public APlayerState
{ //empty class used to store any info related to a specific player
	GENERATED_BODY()

protected:
	AMainPlayerState();
};
