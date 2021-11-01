// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MainMenuWidget.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "MainGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ISLANDSURVIVAL_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UMainGameInstance(const FObjectInitializer& ObjectInitilize);
	virtual void Init(); //auto called when game starts

	UFUNCTION(BlueprintCallable)
	void LoadMenu(); //whenever mainmenu scene loads up call this function

	void HostSession();
	void JoinSession();
private:
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	class UMainMenuWidget* MainMenu;

	IOnlineSubsystem* Subsystem;
	//shorthand way of writting a TSharedPointer
	IOnlineSessionPtr SessionInterface; //note this is a shared pointer
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result); 
	void OnFindSessionComplete(bool bSuccess); //when searching and found sessions
	void OnCreateSessionComplete(FName SessionName, bool bSuccess); //when hosting and joined session
	void OnDestroySessionComplete(FName SessionName, bool bSuccess); //when deleted session

};
