// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "BatteryCollector.h"
#include "BatteryCollectorGameMode.h"
#include "BatteryCollectorCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "SpawnVolume.h"
#include "Global_Log.h"

DEFINE_LOG_CATEGORY_STATIC(MyGameModeLog, All, All)

ABatteryCollectorGameMode::ABatteryCollectorGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	//base decay rate
	DecayRate = 0.075f;
}

void ABatteryCollectorGameMode::BeginPlay()
{
	Super::BeginPlay();
	LOG( MyGameModeLog, "Game Start" );
	//Set time for logs

	DateTime = FDateTime::Now().ToString();

	//find all spawn volume actors
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundActors);

	for (auto Actor : FoundActors)
	{
		ASpawnVolume* SpawnVolumeActor = Cast<ASpawnVolume>(Actor);
		if (SpawnVolumeActor)
		{
			SpawnVolumeActors.AddUnique(SpawnVolumeActor);
		}
	}

	SetCurrentState(EBatteryPlayState::EPlaying);

	//set score to beat
	ABatteryCollectorCharacter* MyCharacter = Cast<ABatteryCollectorCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (MyCharacter)
	{
		PowerToWin = (MyCharacter->GetInitialPower())*1.25f;
	
	}

	if (HUDWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}

}

void ABatteryCollectorGameMode::LogStringToFile(FString data, FString filename)
{
	FString dir = FPaths::GameDir() + "Logs/" + "Gameplay-" + DateTime + "/";
	CreateDirectory(dir);

	FString AbsoluteFilePath = dir + "/" + (filename + " - " + DateTime + ".txt");
	//Save data to file

	FFileHelper::SaveStringToFile(data, *AbsoluteFilePath, FFileHelper::EEncodingOptions::ForceUTF8,
		&IFileManager::Get(), FILEWRITE_Append);

}

bool ABatteryCollectorGameMode::CreateDirectory(const FString& TestDir)
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// Directory Exists?
	if (!PlatformFile.DirectoryExists(*TestDir))
	{
		PlatformFile.CreateDirectory(*TestDir);

		if (!PlatformFile.DirectoryExists(*TestDir))
		{
			return false;
			//~~~~~~~~~~~~~~
		}
	}
	return true;
}

void ABatteryCollectorGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ABatteryCollectorCharacter* MyCharacter = Cast<ABatteryCollectorCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (MyCharacter)
	{
		if (MyCharacter->GetCurrentPower() > PowerToWin)
		{
			SetCurrentState(EBatteryPlayState::EWon);
		}
		//if power more than 0 take it away
		else if (MyCharacter->GetCurrentPower() > 0)
		{
			//decrease power using decay rate
			MyCharacter->UpdatePower(-DeltaTime*DecayRate*(MyCharacter->GetInitialPower()));
		}
		else
		{
			SetCurrentState(EBatteryPlayState::EGameOver);
		}
		
	}
	//Every Tick append data to a string

	//Add player positon log string
	LogStringToFile("Position:" + MyCharacter->GetActorLocation().ToString() + ";" "\n", "Player Position");

	//Log Player Current Power
	FString CurrentPower = FString::FromInt(MyCharacter->GetCurrentPower());
	LogStringToFile("Power:" + CurrentPower + ";" "\n", "Player Power");

	//Add player positon log string
	LogStringToFile("Forward Vector:" + MyCharacter->GetActorForwardVector().ToString() + ";" "\n", "Player Forward");

	
	LOG(MyGameModeLog, CurrentPower);

	//Log Player Position	
	//FString PlayerPos = (MyCharacter->GetActorLocation()).ToString();
	LOG(MyGameModeLog, *MyCharacter->GetActorLocation().ToString());

	//Log Player Forward Vector	
	//FString PlayerDirection = (MyCharacter->GetActorForwardVector()).ToString();
	LOG(MyGameModeLog, *MyCharacter->GetActorForwardVector().ToString());
}

float ABatteryCollectorGameMode::GetPowerToWin() const
{
	return PowerToWin;
}

EBatteryPlayState ABatteryCollectorGameMode::GetCurrentState() const
{
	return CurrentState;
}

void ABatteryCollectorGameMode::SetCurrentState(EBatteryPlayState NewState)
{
	//Set current state
	CurrentState = NewState;
	//Handle current state
	HandleNewState(CurrentState);
}

void ABatteryCollectorGameMode::HandleNewState(EBatteryPlayState NewState)
{
	switch (NewState)
	{
		// if game is playing
		case EBatteryPlayState::EPlaying:
		{
			//spawn volumes to be active
			for (ASpawnVolume* Volume : SpawnVolumeActors)
			{
				Volume->SetSpawningActive(true);
			}
		}
		break;
			//if game is over
		case EBatteryPlayState::EGameOver:
		{
			//End log so that sessions are seperated easier
			LOG(MyGameModeLog, "Game Lost");

			//spawn volumes inactive 
			for (ASpawnVolume* Volume : SpawnVolumeActors)
			{
				Volume->SetSpawningActive(false);
			}
			//disable player inputs
			APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
			if (PlayerController)
			{
				PlayerController->SetCinematicMode(true, false, false, true, true);
			}
			//Ragdoll the player
			ACharacter* MyCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
			if (MyCharacter)
			{
				MyCharacter->GetMesh()->SetSimulatePhysics(true);
				MyCharacter->GetMovementComponent()->MovementState.bCanJump = false;
			}
		}
		break;
		//if game is won
		case EBatteryPlayState::EWon:
		{
			LOG(MyGameModeLog, "Game Won");
			//spawn volumes inactive
			for (ASpawnVolume* Volume : SpawnVolumeActors)
			{
				Volume->SetSpawningActive(false);
			}
		}
		break;
		//if game is unknown
		case EBatteryPlayState::EUnknown:
		{
			//do nothing
		}
		break;
		}
}