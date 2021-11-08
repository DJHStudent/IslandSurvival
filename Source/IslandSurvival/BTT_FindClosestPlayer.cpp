// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_FindClosestPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine.h"
#include "ZombieCharacter.h"

UBTT_FindClosestPlayer::UBTT_FindClosestPlayer()
{
	ClosestDistance = 100000.0;

}

EBTNodeResult::Type UBTT_FindClosestPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (GetWorld()->IsServer())
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), FoundActors);
		ClosestPlayer = nullptr;
		if (FoundActors.Num() > 0)
		{
			float ClosestPlayerDistance = TNumericLimits<float>::Max();
			for (int i = 0; i < FoundActors.Num(); i++)
			{
				float ReturnDistance = FoundActors[i]->GetDistanceTo(OwnerComp.GetOwner());
				if (ReturnDistance < ClosestDistance && ReturnDistance < ClosestPlayerDistance)
				{
					APlayerCharacter* Character = Cast<APlayerCharacter>(FoundActors[i]);
					if (Character)
					{
						ClosestPlayer = Character;
						ClosestPlayerDistance = ReturnDistance;
					}

				}
			}
			if (ClosestPlayer)
			{
				UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
				if (Blackboard)
				{
					Blackboard->SetValueAsObject("SelfActor", ClosestPlayer);
					Blackboard->SetValueAsVector("TargetLocation", ClosestPlayer->GetActorLocation());
					return EBTNodeResult::Succeeded;
				}
			}
		}
	}
	return EBTNodeResult::Failed;
}