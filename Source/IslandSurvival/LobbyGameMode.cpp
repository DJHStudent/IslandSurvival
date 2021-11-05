// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "CurrentPlayerController.h"
#include "Materials/MaterialInstance.h"

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

//	PlayerColours.Add(FLinearColor::Blue);
//	PlayerColours.Add(FLinearColor::Gray);
//	PlayerColours.Add(FLinearColor::Green);
//	PlayerColours.Add(FLinearColor::White);
//	PlayerColours.Add(FLinearColor::Yellow);
//	PlayerColours.Add(FLinearColor(1, 0.63f, 0.06f));
//	PlayerColours.Add(FLinearColor(1, 0.38f, 0.82f)); //purple
//	PlayerColours.Add(FLinearColor(0.63f, 0.13f, 0.1f));
//	PlayerColours.Add(FLinearColor(0.63f, 0.5f, 0.38f));
//	PlayerColours.Add(FLinearColor(0.38f, 1, 0.5f));
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
			PlayerColours.RemoveAt(RandIndex);
		}
	}
}