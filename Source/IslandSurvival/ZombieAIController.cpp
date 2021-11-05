// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieAIController.h"
#include "ZombieCharacter.h"
#include "BehaviorTree/BehaviorTree.h"

AZombieAIController::AZombieAIController()
{
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behavior Comp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackbaord Comp"));
}

void AZombieAIController::OnPossess(APawn* InPawn)
{
	if (GetWorld()->IsServer())
	{
		Super::OnPossess(InPawn);

		auto character = Cast<AZombieCharacter>(InPawn);

		if (character && character->ZombieBehaviorTree)
		{
			BlackboardComp->InitializeBlackboard(*character->ZombieBehaviorTree->BlackboardAsset);
			BehaviorComp->StartTree(*character->ZombieBehaviorTree);
		}
	}
}