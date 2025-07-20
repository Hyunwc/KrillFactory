// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MainPower.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FPowerOn, bool);

UCLASS()
class KRILLFACTORY_API AMainPower : public AActor
{
	GENERATED_BODY()
	
public:	
	AMainPower();

	void TogglePower(bool bOn);

public:
	UPROPERTY(BlueprintReadOnly)
	bool bPowerOn;

	//FPowerOn PowerOn; // �ٸ� ������ ���ε� �ؾ���
};
