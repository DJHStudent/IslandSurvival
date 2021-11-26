// Fill out your copyright notice in the Description page of Project Settings.


#include "ZombieCharacter.h"
#include "PlayerCharacter.h"
#include "LobbyGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "MainGameMode.h"

// Sets default values
AZombieCharacter::AZombieCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	bUseControllerRotationYaw = false;
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception Component"));
	AISense_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	if (AISense_Sight)
	{
		AISense_Sight->SightRadius = 800 * 1.5f;
		AISense_Sight->LoseSightRadius = 900 * 1.5f;
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
		OnActorHit.AddDynamic(this, &AZombieCharacter::OnHit);
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

void AZombieCharacter::OnHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{ //when on server and zombie collides with a player, kill the player
	//get the Game Mode and call the function to reset player
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);

	if (Player)
	{ //find appropriate game mode based on level on and reset players position
		ALobbyGameMode* LobbyGame = Cast<ALobbyGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
		if (LobbyGame)
			LobbyGame->PlayerDeath(Player);
		else
		{
			AMainGameMode* MainGame = Cast<AMainGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
			if (MainGame)
				MainGame->PlayerDeath(Player);
		}
	}
}