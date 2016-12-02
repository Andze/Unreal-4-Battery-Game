// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/GameMode.h"
#include "BatteryCollectorGameMode.generated.h"

UENUM(BlueprintType)
enum class EBatteryPlayState
{
	EPlaying,
	EGameOver,
	EWon,
	EUnknown
};

UCLASS(minimalapi)
class ABatteryCollectorGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABatteryCollectorGameMode();

	virtual void Tick(float DeltaTime) override;

	//retunrs power to win for HUD
	UFUNCTION(BlueprintPure, Category = "power")
	float GetPowerToWin() const;

	virtual void BeginPlay() override;

	//Returns current playing state
	UFUNCTION(BlueprintPure, Category = "power")
	EBatteryPlayState GetCurrentState() const;

	
	void SetCurrentState(EBatteryPlayState NewState);

	void LogStringToFile(FString data, FString filename);

	bool CreateDirectory(const FString& TestDir);

protected:

	FString DateTime;

	//Decay Rate of character
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float DecayRate;

	//power to win the game
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float PowerToWin;

	//The widget class to use for our HUD screen
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	TSubclassOf<class UUserWidget> HUDWidgetClass;

	//the instance of the HUD
	UPROPERTY()
	class UUserWidget* CurrentWidget;


private:
	EBatteryPlayState CurrentState;

	TArray<class ASpawnVolume*> SpawnVolumeActors;

	//handle an function calls that rely on play state
	void HandleNewState(EBatteryPlayState NewState);

};



