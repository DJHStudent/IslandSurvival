// Fill out your copyright notice in the Description page of Project Settings.

//all includes need to be above the .h file of this class
#include "PlayerCharacter.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerCharacterAnimInstance.h"
#include "Kismet/GameplayStatics.h"
// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bAllowTickBeforeBeginPlay = false; //while this would work in editor, might fail when actually shipped so need to always ensure checking if object null or not first
	//ensures this character will be automatically possesed when starting
	//AutoPossessPlayer = EAutoReceiveInput::Player0;

	LookSensitivity = 1.0f;
	SprintMultiplier = 5;

	SprintMovementSpeed = GetCharacterMovement()->MaxWalkSpeed * SprintMultiplier;
	NormalMovementSpeed = GetCharacterMovement()->MaxWalkSpeed;
	CurrentBiomeText = TEXT("");
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	//gets the camera component added to the AActor
	Camera = FindComponentByClass<UCameraComponent>();

	//get the skeletal mesh from the sub object arms of this object
	USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(GetDefaultSubobjectByName(TEXT("Arms")));
	if (SkeletalMesh)//ensures no null pointer and will only work if it exists
		AnimInstance = Cast<UPlayerCharacterAnimInstance>(SkeletalMesh->GetAnimInstance()); //get the anim instance class from the skeletal mesh defined

	////BiomeList = FindComponentByClass<UBiomeGenerationComponent>();//Cast<APCMapV2>(UGameplayStatics::GetActorOfClass(GetWorld(), APCMapV2::StaticClass()));//FindActorOfClass<APCMapV2>();
	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DisplayPointBiome();
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
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);

	//sprinting start and end
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Pressed, this, &APlayerCharacter::SprintStart);
	PlayerInputComponent->BindAction(TEXT("Sprint"), IE_Released, this, &APlayerCharacter::SprintEnd);

	////PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &APlayerCharacter::Reload);
}

void APlayerCharacter::MoveForward(float Value)
{
	//move the character forward by the value of the input axis already given
	FRotator ForwardRotator = GetControlRotation();
	ForwardRotator.Roll = 0;
	ForwardRotator.Pitch = 0;
	AddMovementInput(ForwardRotator.Vector(), Value);
}

void APlayerCharacter::Strafe(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}

void APlayerCharacter::LookUp(float Value)
{
	//AddControllerPitchInput(Value * LookSensitivity); bad as rotates the entire character not just the camera so when looking straight down will slide as not toughing ground anymore
	FRotator LookUpRotation = FRotator::ZeroRotator;//FRotator(0.0f, 0.0f, 0.0f); both sets an FRotator to zero
	LookUpRotation.Pitch = Value * LookSensitivity;

	if (Camera != nullptr && Camera->RelativeRotation.Pitch + LookUpRotation.Pitch < 90 
		&& Camera->RelativeRotation.Pitch + LookUpRotation.Pitch > -90)//stops juttering if the camera moved beyond these values
	{
		Camera->AddRelativeRotation(LookUpRotation);//applies the rotation to the camera and allows only the pitch 
		Camera->RelativeRotation.Yaw = 0.0f;
		Camera->RelativeRotation.Roll = 0.0f;
	}
}

void APlayerCharacter::Turn(float Value)
{
	AddControllerYawInput(Value * LookSensitivity);
}

/*
	frame rate dependance is when the speed of a character etc changes based on the frame rate of the game as the tick function gets called once every frame
	frame rate independence is when the speed of a character etc remains the same regardless of the frame rate of the game

	why no delta time with movement
	2. The value from the keyboard already includes delta time within it so it would work independently of the frame rate automatically

	why no delta time for mouse input
	3. The value comming in from the mouse is a delta value of how much it has changed since the previous frame so * by detlaTime is not needed
	this link should help https://answers.unrealengine.com/questions/1000423/how-to-get-framerate-independent-pawn-rotation.html?sort=oldest
	it is polling/ sampling every frame and the values are determined by the difference in position of the mouse per frame
	controllers however are not frame rate independent so require it to be added
*/

void APlayerCharacter::SprintStart()
{
	//when sprinting starts change the walk speed and tell the animation to play the sprint
	GetCharacterMovement()->MaxWalkSpeed = SprintMovementSpeed; //issue here if it does not register release of shift it will just increase the run speed further
	if(AnimInstance)
		AnimInstance->bIsSprinting = true;
}

void APlayerCharacter::SprintEnd()
{
	//reset the walk speed back to the walk value
	GetCharacterMovement()->MaxWalkSpeed = NormalMovementSpeed;
	//set the animation is sprinting value to be false to stop sprinting and goe back to walk
	if (AnimInstance)
		AnimInstance->bIsSprinting = false;
}

void APlayerCharacter::Reload()
{
	BlueprintReload();
}

void APlayerCharacter::DisplayPointBiome()
{
	int32 XPosition = FMath::RoundToInt(GetActorLocation().X / BiomeList->GridSize);
	int32 YPosition = FMath::RoundToInt(GetActorLocation().Y / BiomeList->GridSize);
	XPosition = FMath::Clamp(XPosition, 0, BiomeList->Width - 1);
	YPosition = FMath::Clamp(YPosition, 0, BiomeList->Height - 1);
	//UE_LOG(LogTemp, Error, TEXT("Check each point Biome junk"), XPosition, YPosition, BiomeList->VerticeColours.Num())//BiomeList->BiomeGeneration->BiomeAtEachPoint.Num())

		//if (YPosition * BiomeList->Width + XPosition < BiomeList->BiomeGeneration->BiomeAtEachPoint.Num())
		{
			int32 BiomeOfPoint = BiomeList->BiomeGeneration->BiomeAtEachPoint[YPosition * BiomeList->Width + XPosition]; //biome at the specific point on map
			//(LogTemp, Error, TEXT("Different Biomes: %i, %f, %f, %i, %i"), YPosition * BiomeList->Width + XPosition, GetActorLocation().X, GetActorLocation().Y, BiomeList->BiomeGeneration->DifferentBiomesMap.Num(), BiomeOfPoint);//BiomeList->BiomeGeneration->BiomeAtEachPoint.Num())
			
			//FString Name = BiomeList->BiomeGeneration->DifferentBiomesMap[BiomeOfPoint];//.BiomeName;
			if(BiomeList->BiomeGeneration->DifferentBiomesMap.Contains(BiomeOfPoint))
			CurrentBiomeText = BiomeList->BiomeGeneration->DifferentBiomesMap[BiomeOfPoint].BiomeName; //for specific biome get its name
			else
				CurrentBiomeText = "Biome Does Not Exist";
		}
	//	else
			//CurrentBiomeText = "Error";

	//print this out as a string
}
