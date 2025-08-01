// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainPower.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPowerStateChanged, bool, bIsPowerOn);

UCLASS()
class KRILLFACTORY_API AMainPower : public AActor
{
	GENERATED_BODY()
	
public:	
	AMainPower();

	//void TogglePower(bool bOn);

public:

	UPROPERTY(BlueprintAssignable, Category = "Power")
	FOnPowerStateChanged OnPowerStateChanged;

    UPROPERTY(BlueprintReadOnly)
	bool bIsPowerOn;

public:
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Power")
	void TogglePower();
	
};
