// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "MainGameState.h"
#include "Kismet/GameplayStatics.h"

UMainGameInstance::UMainGameInstance(const FObjectInitializer& ObjectInitilize)
{
	static ConstructorHelpers::FClassFinder<UUserWidget> MainMenuWidgetObject(TEXT("/Game/Widgets/MainMenuWidget"));
	MainMenuWidgetClass = MainMenuWidgetObject.Class; //get the file location of the widget blueprint class and store it in this variable

	static ConstructorHelpers::FClassFinder<UUserWidget> LobbyWidgetObject(TEXT("/Game/Widgets/LobbyWidget"));
	LobbyWidgetClass = LobbyWidgetObject.Class; //get the file location of the widget blueprint class and store it in this variable

	CurrentGameState = EGameState::LOBBY;
}

void UMainGameInstance::Init() //setup connection to specific online system
{
	Subsystem = IOnlineSubsystem::Get(); //gets the subsystem for a service, in this case null as in editor
	if (Subsystem)
	{
		SessionInterface = Subsystem->GetSessionInterface();

		if (SessionInterface.IsValid())//add all the other delegates here
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMainGameInstance::OnCreateSessionComplete); //makes delegate so when session created calls this function
			//note must have this added
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMainGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMainGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMainGameInstance::OnJoinSessionComplete);
		}

		UE_LOG(LogTemp, Warning, TEXT("Online Subsystem found: %s"), *Subsystem->GetSubsystemName().ToString())
	}
	else
		UE_LOG(LogTemp, Error, TEXT("Unable to find Online Subsystem"))
}

void UMainGameInstance::LoadMenu()
{
	if (MainMenuWidgetClass != nullptr)
		MainMenu = CreateWidget<UMainMenuWidget>(GetWorld(), MainMenuWidgetClass); //spawn in a new widget

	if (MainMenu) //as exists now, add it to the viewport
	{
		MainMenu->AddToViewport();

		FInputModeUIOnly InputMode; //gets the mouse to appear on screen and unlock cursor from menu widget
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetWidgetToFocus(MainMenu->TakeWidget());

		APlayerController* PlayerController;
		PlayerController = GetFirstLocalPlayerController();
		if (PlayerController)
		{
			PlayerController->SetInputMode(InputMode); //tell current controller of game to use these input settings
			PlayerController->bShowMouseCursor = true; //don't hide cursor on mouse down
		}
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("Falied to actually find the widget"))
}

void UMainGameInstance::LoadLobby(APawn* Player)
{

	if (Player && Player->IsLocallyControlled()) //only make a new widget if the player is local
	{
		if (LobbyWidgetClass != nullptr)
			Lobby = CreateWidget<ULobbyWidget>(GetWorld(), LobbyWidgetClass); //spawn in a new widget

		if (Lobby) //as exists now, add it to the viewport
		{
			Lobby->AddToViewport();

			FInputModeUIOnly InputMode; //gets the mouse to appear on screen and unlock cursor from menu widget
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			InputMode.SetWidgetToFocus(Lobby->TakeWidget());

			APlayerController* PlayerController;
			PlayerController = Cast<APlayerController>(Player->GetController());
			if (PlayerController)
			{
				PlayerController->SetInputMode(InputMode); //tell current controller of game to use these input settings
				PlayerController->bShowMouseCursor = true; //don't hide cursor on mouse down

				UE_LOG(LogTemp, Warning, TEXT("A Pawn does actually exist for this player"))
				Lobby->SetEditability(Player);
				CurrentGameState = EGameState::GAME; //as in the lobby now when do next loading will be in game
			}
		}
		else
			UE_LOG(LogTemp, Warning, TEXT("Falied to actually find the widget"))
	}
}

void UMainGameInstance::HostSession()
{
	FName SessionName = TEXT("PLayerChoosenName"); //have actual input to choose this, from a UI element
	if (Subsystem && SessionInterface.IsValid()) //make a new session with current player as host
	{
		FOnlineSessionSettings SessionSettings;
		SessionSettings.bIsLANMatch = true; //only joinable on LAN networks
		SessionSettings.NumPublicConnections = 4;
		SessionSettings.bShouldAdvertise = true; //allows session to be public and joinable

		SessionInterface->CreateSession(0, SessionName, SessionSettings); //will now call the session complete delegate regardless of success or failure
	}
}

void UMainGameInstance::JoinSession() //look up sessions to find one looking for, and then print list of them to UI
{
	SessionSearch = MakeShareable<FOnlineSessionSearch>(new FOnlineSessionSearch());
	if (SessionSearch.IsValid() && SessionInterface.IsValid())
	{
		SessionSearch->bIsLanQuery = true;
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void UMainGameInstance::OnCreateSessionComplete(FName SessionName, bool bSuccess)
{
	if (bSuccess)
	{
		//session sucessful so client travel to the correct map
		APlayerController* PlayerController;
		PlayerController = GetPrimaryPlayerController(); //for this machine get primary, likly only player controller
		if (PlayerController) //travel the player to a different map, while keeping the server active
		{
			//FInputModeGameOnly InputMode; //gets the mouse to appear on screen and unlock cursor from menu widget
			//PlayerController->bShowMouseCursor = false;
			//PlayerController->SetInputMode(InputMode);

			GetWorld()->ServerTravel(TEXT("/Game/Maps/Terrain?listen")); //make a new server, but still allow it to listen so others can join it
		}
	}
	else
	{ //need to display warning to user as likly session with name already in use
		UE_LOG(LogTemp, Warning, TEXT("Session was not Created"))
		if (SessionInterface.IsValid())
			SessionInterface->DestroySession(SessionName);
	}
}

void UMainGameInstance::OnDestroySessionComplete(FName SessionName, bool bSuccess)
{
	if (bSuccess)
		HostSession(); //as session did exist before make a new one
	else
		UE_LOG(LogTemp, Error, TEXT("Unable to destroy session"))
}

void UMainGameInstance::OnFindSessionComplete(bool bSuccess) //here actually print all sessions avaliable to a list
{
	if (bSuccess && SessionSearch.IsValid())
	{
		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;
		FName SessionFoundName;
		for (const FOnlineSessionSearchResult& SearchResult : SearchResults) //loop through all possible sessions setting name to last one found
		{
			SessionFoundName = FName(*SearchResult.GetSessionIdStr());
			UE_LOG(LogTemp, Error, TEXT("Found Session %i"), SearchResult.Session.SessionSettings.NumPublicConnections)
		}
		if (SessionInterface.IsValid() && SessionSearch.IsValid() && SessionSearch->SearchResults.Num() > 0)
		{
			SessionInterface->JoinSession(0, SessionFoundName, SessionSearch->SearchResults[0]);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Find Sessions was not successful"))
	}
}

void UMainGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result == EOnJoinSessionCompleteResult::Success && SessionInterface.IsValid())
	{
		APlayerController* PlayerController;
		FString Address;
		SessionInterface->GetResolvedConnectString(SessionName, Address); //get platform specific info for joining match
		PlayerController = GetPrimaryPlayerController();
		if (PlayerController) //travel the player to a different map, while keeping the server active
		{
			//FInputModeGameOnly InputMode; //gets the mouse to appear on screen and unlock cursor from menu widget
			//PlayerController->bShowMouseCursor = false;
			//PlayerController->SetInputMode(InputMode);
			CurrentGameState = EGameState::LOBBY;
			PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
		}
	}
}


void UMainGameInstance::StartGame() //call on server only here
{
	CurrentGameState = EGameState::GAME;

	//GetWorld()->ServerTravel(TEXT("/Game/Maps/Terrain?listen"));
}

void UMainGameInstance::LoadGame() //called on all players when loading the MainGame level
{
	APlayerController* PlayerController = GetPrimaryPlayerController();
	if (PlayerController)
	{
		APawn* Player = Cast<APawn>(PlayerController->GetOwner());
		if (Player && Player->IsLocallyControlled()) //travel the player to a different map, while keeping the server active
		{
			APlayerController* PlayerController;
			PlayerController = Cast<APlayerController>(Player->GetController());
			if (PlayerController)
			{
				FInputModeGameOnly InputMode; //gets the mouse to appear on screen and unlock cursor from menu widget
				PlayerController->bShowMouseCursor = false;
				PlayerController->SetInputMode(InputMode);
			}

			UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());

			//now can go about adding in the player HUD widget

			///////*if (Lobby)
			//////	Lobby->RemoveFromViewport();*/

			//if the player is on the server and is controlled, don't forget to tell the world to use the main game state now
			if (Player->GetLocalRole() == ROLE_Authority)
			{
				AMainGameState* MainGame = Cast<AMainGameState>(UGameplayStatics::GetGameState(GetWorld()));
				if (MainGame)
				{
					MainGame->GenerateTerrain(Seed, TerrainWidth, TerrainHeight); //generate in the terrain for the game
					//UE_LOG(LogTemp, Warning, TEXT("Terrain is Being Updated"))
				}
			}
		}
	}
}


void UMainGameInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UMainGameInstance, CurrentGameState);
}
