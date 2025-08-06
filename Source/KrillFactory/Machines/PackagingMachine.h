// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KFMachineBase.h"
#include "PackagingMachine.generated.h"

UCLASS()
class KRILLFACTORY_API APackagingMachine : public AKFMachineBase
{
	GENERATED_BODY()
	
public:	
	APackagingMachine();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnPackagingZoneOverlapBegin(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnPackagingZoneOverlapEnd(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* PackagingZone;

public:
	UPROPERTY()
	class AMainPower* MainPower; // 델리게이트 사용을 위한 참조
	// 델리게이트에 등록할 함수
	UFUNCTION()
	void OnMainPowerStateChanged(bool bPowerOn);

private:
	UPROPERTY()
	class AConveyor* FoundConveyor;
};
