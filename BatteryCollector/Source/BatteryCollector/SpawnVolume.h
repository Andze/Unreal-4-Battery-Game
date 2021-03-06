// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

UCLASS()
class BATTERYCOLLECTOR_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawnVolume();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	//returns the where to spawn sub object
	FORCEINLINE UBoxComponent* GetWhereToSpawn() const { return WhereToSpawn; }

	//Find point within box component
	UFUNCTION(BlueprintPure, Category = "Spawning")
	FVector GetRandomPointInVolume();

	//this function toggles spawn volumes
	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SetSpawningActive(bool bShouldSpawn);

	void LogStringToFile(FString data, FString filename);

	bool CreateDirectory(const FString& TestDir);


protected:
	//The pickup to spawn
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class APickup> WhatToSpawn;

	FString DateTime;

	//Timer 
	FTimerHandle SpawnTimer;

	//min spawn delay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	float SpawnDelayRangeLow;
	//max spawn delay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category =  "Spawning")
	float SpawnDelayRangeHigh;




private:
	//Box Component to specify where pickups should be spawned
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* WhereToSpawn;

	//Handle spawning a new pcikup
	void SpawnPickup();

	//Current Spawn delay
	float SpawnDelay;

	
	
};
