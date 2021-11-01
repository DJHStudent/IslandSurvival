// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ZombieAIController.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDSURVIVAL_API AZombieAIController : public AAIController
{
	GENERATED_BODY()
	
private:
	UBehaviorTreeComponent* BehaviorComp;
	UBlackboardComponent* BlackboardComp;

public:
	AZombieAIController();

	virtual void OnPossess(APawn* InPawn) override;

};
