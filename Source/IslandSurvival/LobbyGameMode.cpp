// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "CurrentPlayerController.h"
#include "Materials/MaterialInstance.h"
#include "GameFramework/GameSession.h"
#include "Engine/Engine.h"
#include "Net/OnlineEngineInterface.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

ALobbyGameMode::ALobbyGameMode()
{
	//get all the colour materials and assign them to the list of player colours
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

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer) //called after a player controller has sucessfully joined session and PostLogin
{
	Super::PostLogin(NewPlayer); //once player pawn created also do this code

	ACurrentPlayerController* PlayerController = Cast<ACurrentPlayerController>(NewPlayer);
	if (PlayerController) //if the player controller's class actually exists
	{
		APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PlayerController->GetPawn());
		if(PlayerCharacter)
			UE_LOG(LogTemp, Warning, TEXT("Player Has Been Found"))

		if (NewPlayer->GetLocalRole() == ROLE_Authority && NewPlayer->IsLocalController()) //if the host
			PlayerController->ServerUpdateColour(HostColour); //assign the host the same colour each time
		else
		{
			int32 RandIndex = FMath::RandRange(0, PlayerColours.Num() - 1); 
			PlayerController->ServerUpdateColour(PlayerColours[RandIndex]); //assign the player a radom colour from the list
			UsedPlayerColours.Add(NewPlayer, PlayerColours[RandIndex]); //add to list of used colours with key as controller used
			PlayerColours.RemoveAt(RandIndex); //remove colour as no new player can become it
			PlayerController->GetPawn();
		}
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{ //called when a controller has left the game
	APlayerController* ExitingPlayer = Cast<APlayerController>(Exiting);

	if (ExitingPlayer && UsedPlayerColours.Contains(ExitingPlayer)) //ensure that the controller used is a PlayerController and it has been assigned a colour
	{
		PlayerColours.Add(UsedPlayerColours[ExitingPlayer]); //get colour a player was using and readd it to the list of possible ones to use
		UsedPlayerColours.Remove(ExitingPlayer); //remove this from the list of used colours
	}
	Super::Logout(Exiting);
}

void ALobbyGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{ //called when user first connects to game, before anysetup is done
	const bool bUniqueIdCheckOk = (!UniqueId.IsValid() || (UniqueId.GetType() == UOnlineEngineInterface::Get()->GetDefaultOnlineSubsystemName())); //checks player ID is actually unique
	if (PlayerColours.Num() > 0 && bUniqueIdCheckOk) //only allow into session if not full, i.e a colour still exists to choose from 
		ErrorMessage = GameSession->ApproveLogin(Options);
	else
		ErrorMessage = TEXT("Failed to Login");

	FGameModeEvents::GameModePreLoginEvent.Broadcast(this, UniqueId, ErrorMessage); //broadcast message back to client, causing them to fail to login if ErrorMessage not empty
}

void ALobbyGameMode::PlayerDeath(APlayerCharacter* Player)
{
	if (Player) //update dead players location and show UI
	{
		Player->GetCharacterMovement()->Velocity = FVector::ZeroVector;
		Player->OnDeathServer();
	}
}