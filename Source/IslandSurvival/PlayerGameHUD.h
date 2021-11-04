// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "PlayerGameHUD.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDSURVIVAL_API UPlayerGameHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateSeedTextBlock(FString Text);
	void UpdateBiomeTextBlock(FString Text);
private:
	//APlayerGameHUD();

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlockSeed;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlockBiome;

};
