// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "CurrentPlayerController.h"
#include "Materials/MaterialInstance.h"
#include "GameFramework/GameSession.h"
#include "Engine/Engine.h"
#include "Net/OnlineEngineInterface.h"

ALobbyGameMode::ALobbyGameMode()
{
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> RedObject(TEXT("/Game/Materials/PlayerColours/RedPlayer"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> BlackObject(TEXT("/Game/Materials/PlayerColours/BlackPlayer"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> BlueGreenObject(TEXT("/Game/Materials/PlayerColours/BlueGreenPlayer"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> BlueObject(TEXT("/Game/Materials/PlayerColours/BluePlayer"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> BrownObject(TEXT("/Game/Materials/PlayerColours/BrownPlayer"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> GrayObject(TEXT("/Game/Materials/PlayerColours/GrayPlayer"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> GreenObject(TEXT("/Game/Materials/PlayerColours/GreenPlayer"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> OrangeObject(TEXT("/Game/Materials/PlayerColours/OrangePlayer"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> PinkObject(TEXT("/Game/Materials/PlayerColours/PinkPlayer"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> PurpleObject(TEXT("/Game/Materials/PlayerColours/PurplePlayer"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> WhiteObject(TEXT("/Game/Materials/PlayerColours/WhitePlayer"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> YellowObject(TEXT("/Game/Materials/PlayerColours/YellowPlayer"));
	HostColour = Cast<UMaterialInterface>(RedObject.Object);
	PlayerColours.Add(Cast<UMaterialInterface>(BlackObject.Object));
	PlayerColours.Add(Cast<UMaterialInterface>(BlueGreenObject.Object));
	PlayerColours.Add(Cast<UMaterialInterface>(BlueObject.Object));
	PlayerColours.Add(Cast<UMaterialInterface>(BrownObject.Object));
	PlayerColours.Add(Cast<UMaterialInterface>(GrayObject.Object));
	PlayerColours.Add(Cast<UMaterialInterface>(GreenObject.Object));
	PlayerColours.Add(Cast<UMaterialInterface>(OrangeObject.Object));
	PlayerColours.Add(Cast<UMaterialInterface>(PinkObject.Object));
	PlayerColours.Add(Cast<UMaterialInterface>(PurpleObject.Object));
	PlayerColours.Add(Cast<UMaterialInterface>(WhiteObject.Object));
	PlayerColours.Add(Cast<UMaterialInterface>(YellowObject.Object));

	InactivePlayerStateLifeSpan = 1.0f;
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ACurrentPlayerController* PlayerController = Cast<ACurrentPlayerController>(NewPlayer);
	if (PlayerController)
	{
		if (NewPlayer->GetLocalRole() == ROLE_Authority && NewPlayer->IsLocalController())
			PlayerController->ServerUpdateColour(HostColour);
		else
		{
			int32 RandIndex = FMath::RandRange(0, PlayerColours.Num() - 1);
			PlayerController->ServerUpdateColour(PlayerColours[RandIndex]);
			UsedPlayerColours.Add(NewPlayer, PlayerColours[RandIndex]);
			PlayerColours.RemoveAt(RandIndex);
		}
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	APlayerController* ExitingPlayer = Cast<APlayerController>(Exiting);

	if (ExitingPlayer && UsedPlayerColours.Contains(ExitingPlayer))
	{
		PlayerColours.Add(UsedPlayerColours[ExitingPlayer]);
		UsedPlayerColours.Remove(ExitingPlayer);
	}
	Super::Logout(Exiting);
}

void ALobbyGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	const bool bUniqueIdCheckOk = (!UniqueId.IsValid() || (UniqueId.GetType() == UOnlineEngineInterface::Get()->GetDefaultOnlineSubsystemName()));
	if (PlayerColours.Num() > 0) //as long as a colour exists 
	{
		ErrorMessage = GameSession->ApproveLogin(Options);
	}
	else
	{
		ErrorMessage = TEXT("Failed to Login");
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Failed to Join Session " + NumPlayers);
	}

	FGameModeEvents::GameModePreLoginEvent.Broadcast(this, UniqueId, ErrorMessage);
}
