// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/NavLinkProxy.h"
#include "GameFramework/Character.h"
#include "Engine/Engine.h"
#include "NavLinkClass.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDSURVIVAL_API ANavLinkClass : public ANavLinkProxy
{
	GENERATED_BODY()

public:
	ANavLinkClass();

	UFUNCTION(BlueprintCallable)
	void FindCharacter(AActor* Agent, const FVector& Destination);

	FVector CalculateVelocity(FVector FinishVector, FVector StartingVector, float Duration);

	UPROPERTY(EditAnywhere)
	float JumpDuration;
};
