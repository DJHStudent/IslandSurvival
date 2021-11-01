// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "PlayerCharacter.h"
#include "BTT_FindClosestPlayer.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDSURVIVAL_API UBTT_FindClosestPlayer : public UBTTask_BlueprintBase
{
	GENERATED_BODY()
	
public:
	UBTT_FindClosestPlayer();

	void ReceiveExecuteAI(AAIController* OwnerController, APawn* ControlledPawn);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

	UPROPERTY(EditAnywhere, Category = "Blackboard Keys")
	FBlackboardKeySelector SelfActor;

private:
	float ClosestDistance;

	APlayerCharacter* ClosestPlayer;

};
