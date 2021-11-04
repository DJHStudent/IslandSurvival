// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_FindClosestPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine.h"

UBTT_FindClosestPlayer::UBTT_FindClosestPlayer()
{
	ClosestDistance = 100000.0;

}

EBTNodeResult::Type UBTT_FindClosestPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerCharacter::StaticClass(), FoundActors);
	for (int i = 0; i < FoundActors.Num(); i++)
	{
		float ReturnDistance = FoundActors[i]->GetDistanceTo(OwnerComp.GetOwner());
		if (ReturnDistance < ClosestDistance)
		{
			APlayerCharacter* Character = Cast<APlayerCharacter>(FoundActors[i]);
			ClosestPlayer = Character;
			ClosestDistance = ReturnDistance;

		}
	}
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	Blackboard->SetValueAsObject("SelfActor", ClosestPlayer);
	Blackboard->SetValueAsVector("TargetLocation", ClosestPlayer->GetActorLocation());
	return EBTNodeResult::Succeeded;
}