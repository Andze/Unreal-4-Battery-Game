// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Pickup.h"
#include "BatteryPickup.generated.h"

/**
 * 
 */
UCLASS()
class BATTERYCOLLECTOR_API ABatteryPickup : public APickup
{
	GENERATED_BODY()
	
public:
	//Set default values for the actors properties
	ABatteryPickup();
	
	//Overide was collected function
	void WasCollected_Implementation() override;

	//Public way to access the battery power level
	float GetPower();

protected:
	//Set the amount of power the battery gives
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
	float BatteryPower;
	
};
