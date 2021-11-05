// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MoveToPlayer.h"
#include "ZombieAIController.h"
EBTNodeResult::Type UBTT_MoveToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController && GetWorld()->IsServer())
	{
		UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
		if (Blackboard)
		{
			AActor* Goal = Cast<AActor>(Blackboard->GetValueAsObject("SelfActor"));
			if (Goal)
			{
				FVector Location = Blackboard->GetValueAsVector("TargetLocation");
				AIController->MoveToLocation(Location);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}