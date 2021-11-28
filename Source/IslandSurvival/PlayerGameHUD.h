// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "Components/CanvasPanel.h"
#include "PlayerGameHUD.generated.h"

/**
 *
 */
UCLASS()
class ISLANDSURVIVAL_API UPlayerGameHUD : public UUserWidget //widget for player when on the Terrain map
{
	GENERATED_BODY()
	
public:

	void UpdateSeedTextBlock(FString Text);
	void UpdateBiomeTextBlock(FString Text);
	void UpdateFuelValue(float Percentage);
	void WonGame();

	void ShowPauseMenu();
	void HidePauseMenu();

	void HideLoading();

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* DeathMessage;
private:
	virtual bool Initialize() override;

	UPROPERTY(meta = (BindWidget)) //bind this to the actual widget element related to it
	UTextBlock* TextBlockSeed;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlockBiome;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlockFuel;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlockPause;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* PauseMenu;
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* Loading;

	UPROPERTY(meta = (BindWidget))
	UButton* ButtonResume;
	UPROPERTY(meta = (BindWidget))
	UButton* ButtonLeave;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* FuelProgressBar;

	UFUNCTION()
	void OnResumeButtonPressed();
	UFUNCTION()
	void OnLeaveButtonPressed();

	class UMainGameInstance* MainGameInstance;
};
