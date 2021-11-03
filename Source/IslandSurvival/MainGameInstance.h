// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Net/UnrealNetwork.h"
#include "MainMenuWidget.h"
#include "LobbyWidget.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "MainGameInstance.generated.h"

UENUM() //just a macro specifying its an enum class
enum class EGameState : uint8 //should be called EAgentState as an enum and Unreal convention
{
	LOBBY UMETA(DisplayName = "Lobby"),
	GAME UMETA(DisplayName = "Game"),
};
UCLASS()
class ISLANDSURVIVAL_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UMainGameInstance(const FObjectInitializer& ObjectInitilize);
	virtual void Init(); //auto called when game starts

	UFUNCTION(BlueprintCallable)
	void LoadMenu(); //whenever mainmenu scene loads up call this function
	UFUNCTION(BlueprintCallable)
	void LoadLobby(APawn* Player);

	void HostSession();
	void JoinSession();

	void StartGame();
	UFUNCTION(BlueprintCallable)
	void LoadGame();
	
	class ULobbyWidget* Lobby;

	//UPROPERTY(Replicated)
	EGameState CurrentGameState;

	//these variables will only actually be on the server version
	int32 TerrainWidth;
	int32 TerrainHeight;
	int32 Seed;
private:
	TSubclassOf<UUserWidget> MainMenuWidgetClass;
	class UMainMenuWidget* MainMenu;
	TSubclassOf<UUserWidget> LobbyWidgetClass;
	


	IOnlineSubsystem* Subsystem;
	//shorthand way of writting a TSharedPointer
	IOnlineSessionPtr SessionInterface; //note this is a shared pointer
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result); 
	void OnFindSessionComplete(bool bSuccess); //when searching and found sessions
	void OnCreateSessionComplete(FName SessionName, bool bSuccess); //when hosting and joined session
	void OnDestroySessionComplete(FName SessionName, bool bSuccess); //when deleted session

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override; //allow all the variables to be replicated
};
