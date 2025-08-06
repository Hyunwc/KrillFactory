// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KFObjectInteractComponent.generated.h"

class AAInteractableBase;
class AKFPlayerController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRILLFACTORY_API UKFObjectInteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UKFObjectInteractComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(VisibleAnywhere)
	AAInteractableBase* Owner;

	UPROPERTY(VisibleAnywhere)
	AKFPlayerController* PC;


public:
	UFUNCTION(BlueprintCallable)
	void BeginFocus();

	UFUNCTION(BlueprintCallable)
	void EndFocus();

	UFUNCTION(BlueprintCallable)
	void Click();
		
};
