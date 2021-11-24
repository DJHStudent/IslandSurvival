// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieCharacter.h"

// Sets default values
AZombieCharacter::AZombieCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	bUseControllerRotationYaw = false;
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component"));
	AISense_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	if (AISense_Sight)
	{
		AISense_Sight->SightRadius = 800;
		AISense_Sight->LoseSightRadius = 900;
		AISense_Sight->PeripheralVisionAngleDegrees = 180.0f;
		AISense_Sight->DetectionByAffiliation.bDetectEnemies = true;
		AISense_Sight->DetectionByAffiliation.bDetectNeutrals = true;
		AISense_Sight->DetectionByAffiliation.bDetectFriendlies = true;
	}
	AIPerceptionComponent->ConfigureSense(*AISense_Sight);
	AIPerceptionComponent->SetDominantSense(AISense_Sight->GetSenseImplementation());
}

// Called when the game starts or when spawned
void AZombieCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (GetWorld()->IsServer())
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AZombieCharacter::TargetPerceptionUpdated);
	}
}

// Called to bind functionality to input
void AZombieCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (GetWorld()->IsServer())
	{	
		Super::SetupPlayerInputComponent(PlayerInputComponent);
		//AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AZombieCharacter::TargetPerceptionUpdated);
	}
}

void AZombieCharacter::TargetPerceptionUpdated(AActor* actor, FAIStimulus stimulus)
{
	if (GetWorld()->IsServer())
	{
		if (stimulus.WasSuccessfullySensed())
		{
			ChangeEnum(1);
		}
		else
		{
			ChangeEnum(0);
		}
	}
}