// Fill out your copyright notice in the Description page of Project Settings.

//all includes need to be above the .h file of this class
#include "PlayerCharacter.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerCharacterAnimInstance.h"
#include "Net/UnrealNetwork.h"
#include "Engine/EngineTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bAllowTickBeforeBeginPlay = false; //while this would work in editor, might fail when actually shipped so need to always ensure checking if object null or not first
	//ensures this character will be automatically possesed when starting
	//AutoPossessPlayer = EAutoReceiveInput::Player0;

	LookSensitivity = 1.0f;
	SprintMultiplier = 5.5f;

	SprintMovementSpeed = GetCharacterMovement()->MaxWalkSpeed * SprintMultiplier;
	NormalMovementSpeed = GetCharacterMovement()->MaxWalkSpeed;
	SwimMovementSpeed = 600;
	SprintSwimMovementSpeed = SwimMovementSpeed * SprintMultiplier;
	CurrentBiomeText = TEXT("");

	MainGameInstance = Cast<UMainGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));

	bPaused = false;
	PrimaryActorTick.bTickEvenWhenPaused = true;
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//gets the camera component added to the AActor
	if (this->GetLocalRole() == ROLE_AutonomousProxy || this->IsLocallyControlled()) //only do if controlled
	{
		Camera = FindComponentByClass<UCameraComponent>();

		//get the skeletal mesh from the sub object arms of this object
		USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(GetDefaultSubobjectByName(TEXT("Arms")));
		if (SkeletalMesh)//ensures no null pointer and will only work if it exists
			AnimInstance = Cast<UPlayerCharacterAnimInstance>(SkeletalMesh->GetAnimInstance()); //get the anim instance class from the skeletal mesh defined

		UISetup(); //setup the appropriate widget
	}
}
void APlayerCharacter::UISetup() //based on game state do some updates to the visuals
{
	if (MainGameInstance) //timmer so if failed try it again
	{
		if (MainGameInstance->CurrentGameState == EGameState::LOBBY) //if player has spawn into the lobby
			MainGameInstance->LoadLobby(this); //setup the lobbies UI
		else if (MainGameInstance->CurrentGameState == EGameState::GAME) //if player spawned into the terrain map
		{
			BiomeList = Cast<AProcedurallyGeneratedTerrain>(UGameplayStatics::GetActorOfClass(GetWorld(), AProcedurallyGeneratedTerrain::StaticClass())); //Get a reference to the terrain
			PlayerWidget = Cast<UPlayerGameHUD>(MainGameInstance->CurrentPlayerHUDWidget); //setup reference to player's widget
		}
		USkeletalMeshComponent* BodyMesh = Cast<USkeletalMeshComponent>(GetDefaultSubobjectByName(TEXT("InvisShadowBody"))); //get player component to update colour
		if (BodyMesh && MainGameInstance->PlayerColour) //update players colour so it matches what 
		{
			BodyMesh->SetMaterial(0, MainGameInstance->PlayerColour); //set this player's colour to the value saved
			ServerUpdatePlayerColour(MainGameInstance->PlayerColour); //call on the server so that it can update all other clients, with the right colour
			GetController();
		}
	}
}
void APlayerCharacter::ServerUpdatePlayerColour_Implementation(UMaterialInterface* Colour) //on the server update player's colur
{
	USkeletalMeshComponent* BodyMesh = Cast<USkeletalMeshComponent>(GetDefaultSubobjectByName(TEXT("InvisShadowBody"))); //get player component to update colour
	if (BodyMesh && Colour)
	{
		BodyMesh->SetMaterial(0, Colour); //update colour with new material
		PlayersColour = Colour; //update replicated variables colour
	}
}
void APlayerCharacter::ReplicatedColourUpdate() //when this players mesh colour changed on server, update it on each client
{
	USkeletalMeshComponent* BodyMesh = Cast<USkeletalMeshComponent>(GetDefaultSubobjectByName(TEXT("InvisShadowBody"))); //get player component to update colour
	if (BodyMesh && PlayersColour)
	{
		BodyMesh->SetMaterial(0, PlayersColour); //update colour with new material
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(BiomeList && BiomeList->BiomeGeneration && (GetLocalRole() == ROLE_AutonomousProxy || IsLocallyControlled())) //i.e if clients player and on game map
		DisplayPointBiome(); //update the text for the biome currently in

	////////if (IsLocallyControlled() && GetCharacterMovement()->IsInWater() && AnimInstance && AnimInstance->bIsSprinting) //deactivate sprinting whenever inside the water
	////////	SprintEnd();

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//on the specified axis, in this class when that key is pressed play the specified function
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayerCharacter::MoveForward); //also already handles diagonal mvoement so that speed the same
	PlayerInputComponent->BindAxis(TEXT("Strafe"), this, &APlayerCharacter::Strafe);

	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APlayerCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APlayerCharacter::Turn);

	//jumping already default and built in so just need to setup the right input for it to work
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &APlayerCharacter::Jump);

	//sprinting start and end
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &APlayerCharacter::SprintStart);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &APlayerCharacter::SprintEnd);

	PlayerInputComponent->BindAction(TEXT("Pause"), IE_Pressed, this, &APlayerCharacter::Paused);

	////PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &APlayerCharacter::Reload);
}

void APlayerCharacter::MoveForward(float Value)
{
	if (!bPaused)
	{
		//move the character forward by the value of the input axis already given
		FRotator ForwardRotator = GetControlRotation();
		ForwardRotator.Roll = 0;
		ForwardRotator.Pitch = 0;
		AddMovementInput(ForwardRotator.Vector(), Value);
	}
}

void APlayerCharacter::Strafe(float Value)
{
	if (!bPaused)
		AddMovementInput(GetActorRightVector(), Value);
}

void APlayerCharacter::LookUp(float Value)
{
	if (!bPaused)
	{
		FRotator LookUpRotation = FRotator::ZeroRotator;//FRotator(0.0f, 0.0f, 0.0f); both sets an FRotator to zero
		LookUpRotation.Pitch = Value * LookSensitivity;

		if (Camera && Camera->RelativeRotation.Pitch + LookUpRotation.Pitch < 90
			&& Camera->RelativeRotation.Pitch + LookUpRotation.Pitch > -90)//stops juttering if the camera moved beyond these values
		{
			Camera->AddRelativeRotation(LookUpRotation);//applies the rotation to the camera and allows only the pitch 
			Camera->RelativeRotation.Yaw = 0.0f;
			Camera->RelativeRotation.Roll = 0.0f;
		}
	}
}

void APlayerCharacter::Turn(float Value)
{
	if (!bPaused)
		AddControllerYawInput(Value * LookSensitivity);
}

void APlayerCharacter::SprintStart()
{
	if (!bPaused)// && GetCharacterMovement()->MovementMode != MOVE_Swimming) //can only sprint when not in water
	{
		//when sprinting starts change the walk speed and tell the animation to play the sprint
		GetCharacterMovement()->MaxWalkSpeed = SprintMovementSpeed; //issue here if it does not register release of shift it will just increase the run speed further
		GetCharacterMovement()->MaxSwimSpeed = SprintSwimMovementSpeed;

		ServerSprintStart();
		if (AnimInstance)
			AnimInstance->bIsSprinting = true;
	}
}

void APlayerCharacter::SprintEnd()
{
	if (!bPaused)
	{
		//reset the walk speed back to the walk value
		GetCharacterMovement()->MaxWalkSpeed = NormalMovementSpeed;
		GetCharacterMovement()->MaxSwimSpeed = SwimMovementSpeed;
		ServerSprintEnd();
		//set the animation is sprinting value to be false to stop sprinting and goe back to walk
		if (AnimInstance)
			AnimInstance->bIsSprinting = false;
	}
}
void APlayerCharacter::Jump()
{
	if(!bPaused)
		Super::Jump();
}
void APlayerCharacter::ServerSprintStart_Implementation() //update sprint on the server
{
	GetCharacterMovement()->MaxWalkSpeed = SprintMovementSpeed;
	GetCharacterMovement()->MaxSwimSpeed = SprintSwimMovementSpeed;
}

void APlayerCharacter::ServerSprintEnd_Implementation()
{
	GetCharacterMovement()->MaxWalkSpeed = NormalMovementSpeed;
	GetCharacterMovement()->MaxSwimSpeed = SwimMovementSpeed;
}
void APlayerCharacter::Reload()
{
	BlueprintReload();
}

void APlayerCharacter::DisplayPointBiome()
{
	//based on the players actual position determine its position within the Biome at each point array
	int32 XPosition = FMath::RoundToInt(GetActorLocation().X / BiomeList->GridSize);
	int32 YPosition = FMath::RoundToInt(GetActorLocation().Y / BiomeList->GridSize);
	//clamp the values so they don't fall outside of the size of the array of biome points
	XPosition = FMath::Clamp(XPosition, 0, BiomeList->Width - 1);
	YPosition = FMath::Clamp(YPosition, 0, BiomeList->Height - 1);

	int32 BiomeIndex = YPosition * BiomeList->Width + XPosition; //get the 1D index of the position of the point 
	if (BiomeIndex < BiomeList->BiomeGeneration->BiomeAtEachPoint.Num()) //check to ensure the index doesn't fall outside the bounds of the array
	{
		int32 BiomeOfPoint = BiomeList->BiomeGeneration->BiomeAtEachPoint[BiomeIndex]; //biome at the specific point on map

		if (BiomeList->BiomeGeneration->BiomeStatsMap.Contains(BiomeOfPoint)) //if the biome choosen actually does exist i.e an error occured when adding it's value to the list
		{
			CurrentBiomeText = BiomeList->BiomeGeneration->BiomeStatsMap[BiomeOfPoint].GetDefaultObject()->BiomeName; //for specific biome get its name and display it to the UI
		}
		else
			CurrentBiomeText = "Lookup Error";
	}
	else
		CurrentBiomeText = "Lookup Error"; //display an error as one occured when trying to access a biome as it didn't exist

	PlayerWidget->UpdateBiomeTextBlock(CurrentBiomeText);
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlayerCharacter, PlayersColour); //setup this player to be replicated
}

void APlayerCharacter::Paused() //called if pause initiated on a client
{
	if (IsLocallyControlled()) //only call if currently being controlled
	{
		if (!bPaused)
		{
			bPaused = true; //set player to be paused

			FInputModeGameAndUI InputMode; //gets the mouse to appear on screen and unlock cursor from menu widget
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			if (MainGameInstance && MainGameInstance->CurrentGameState == EGameState::LOBBY) //if in the lobby
				MainGameInstance->Lobby->SetVisibility(ESlateVisibility::Visible); //make lobby pause screen visable
			else if (PlayerWidget) //if this widget exists must be in the terrain map
				PlayerWidget->ShowPauseMenu(); //upate players HUD widget to show pause screen

			APlayerController* PlayerController = Cast<APlayerController>(GetController());
			if (PlayerController) //update input to work for the UI on the specified player controller
			{
				PlayerController->SetInputMode(InputMode);
				PlayerController->bShowMouseCursor = true;
				PlayerController->bEnableClickEvents = true;
				PlayerController->bEnableMouseOverEvents = true;
			}
		}
		else //as was paused, resume playing
			Resume();
	}
}

void APlayerCharacter::Resume() //called when player finished being paused
{
	if (MainGameInstance && MainGameInstance->CurrentGameState == EGameState::LOBBY) //if in lobby game
		MainGameInstance->Lobby->SetVisibility(ESlateVisibility::Hidden); //hide the lobby pause widget
	else if (PlayerWidget) //if this widget exists must be in the terrain map
		PlayerWidget->HidePauseMenu(); //upate players HUD widget to hide pause screen

	FInputModeGameOnly InputMode; //gets the mouse to appear on screen and unlock cursor from menu widget

	bPaused = false;
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController) //update input to stop working for UI and only game on the specified player controller
	{
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = false;
		PlayerController->bEnableClickEvents = false;
		PlayerController->bEnableMouseOverEvents = false;
	}
}

void APlayerCharacter::OnDeathServer_Implementation()
{
	if (MainGameInstance)
	{
		GetCharacterMovement()->Velocity = FVector::ZeroVector;
		MainGameInstance->PlayerDeathStart(this);
	}
}
