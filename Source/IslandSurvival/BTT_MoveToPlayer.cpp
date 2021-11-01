// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MoveToPlayer.h"
#include "ZombieAIController.h"

EBTNodeResult::Type UBTT_MoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	AActor* Goal = Cast<AActor>(Blackboard->GetValueAsObject("SelfActor"));
	FVector Location = Blackboard->GetValueAsVector("TargetLocation");
	AIController->MoveToLocation(Location);
	return EBTNodeResult::Succeeded;
}