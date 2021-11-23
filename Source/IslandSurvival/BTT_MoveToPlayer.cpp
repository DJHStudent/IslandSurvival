// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_MoveToPlayer.h"
#include "ZombieAIController.h"
#include "Engine/Engine.h"

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
				//if (AIController->GetPathFollowingComponent()->GetStatus() != EPathFollowingStatus::Moving)
				{
					AIController->MoveToLocation(Location, -1, false);//, true, false, true); //instead use character movement, add movement input
					//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 10.0f, FColor::Green, TEXT("Client Moving Location Updates: " + AIController->GetName() + "Location: " + Location.ToString()));
				}
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}

void UBTT_MoveToPlayer::ClientMoveToLocation(FVector Location)
{
	
}
