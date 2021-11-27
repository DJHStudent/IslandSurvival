// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MoveToPlayer.h"
#include "ZombieAIController.h"
#include "Engine/Engine.h"

EBTNodeResult::Type UBTT_MoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AZombieAIController* AIController = Cast<AZombieAIController>(OwnerComp.GetAIOwner());
	if (AIController && GetWorld()->IsServer())
	{
		UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
		if (Blackboard)
		{
			AActor* Goal = Cast<AActor>(Blackboard->GetValueAsObject("SelfActor"));
			if (Goal)
			{
				FVector Location = Blackboard->GetValueAsVector("TargetLocation");
				AIController->MoveToLocation(Location);//move character to the desired location, following nav mesh path
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}