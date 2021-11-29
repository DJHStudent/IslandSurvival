// Fill out your copyright notice in the Description page of Project Settings.


#include "NavLinkClass.h"

ANavLinkClass::ANavLinkClass()
{
	bSmartLinkIsRelevant = true;
	JumpDuration = 2.0f;
}

void ANavLinkClass::FindCharacter(AActor* Agent, const FVector& Destination)
{
	ACharacter* AgentCharacter = Cast<ACharacter>(Agent);
	FVector velocity = CalculateVelocity(Destination, AgentCharacter->GetActorLocation(), JumpDuration);
	AgentCharacter->LaunchCharacter(velocity, true, true);
}

FVector ANavLinkClass::CalculateVelocity(FVector FinishVector, FVector StartingVector, float Duration)
{
	float InDuration = Duration;
	float x = (FinishVector.X - StartingVector.X) / InDuration; //Distance / Time = Speed or velocity
	float y = (FinishVector.Y - StartingVector.Y) / InDuration;
	//Unreal use centimetre, 9.82m/s^2 = 982cm/s^2
	float z = (FinishVector.Z - (StartingVector.Z + FMath::Pow(InDuration, 2.0f) * -0.5f * 982.0f)) / InDuration;
	return FVector(x, y, z);
}