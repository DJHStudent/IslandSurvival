// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerCharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDSURVIVAL_API UPlayerCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	

public:
	// Sets default values for this character's properties
	UPlayerCharacterAnimInstance();


public:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
	bool bIsSprinting;

};
