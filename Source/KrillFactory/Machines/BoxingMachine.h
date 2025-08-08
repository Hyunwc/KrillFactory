// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KFMachineBase.h"
#include "BoxingMachine.generated.h"


UCLASS()
class KRILLFACTORY_API ABoxingMachine : public AKFMachineBase
{
	GENERATED_BODY()
	
public:	
	ABoxingMachine();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxingZoneOverlapBegin(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxingZoneOverlapEnd(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* BoxingZone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	int32 PackCount; // 박스에 들어간 포장된 블럭의 개수

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	int32 MaxCount = 16; // 박스에 들어갈 최대 블럭 개수

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	bool bBoxingComplete; // 박싱이 완료됨을 알릴 불리언값


	// 카운트 증가되는게 확인되면 박스를 담을 배열도 필요할지 싶은데

private:
	UPROPERTY()
	class AConveyor* FoundConveyor;

public:
	UPROPERTY()
	class AMainPower* MainPower; // 델리게이트 사용을 위한 참조

	// 델리게이트에 등록할 함수
	UFUNCTION()
	void OnMainPowerStateChanged(bool bPowerOn);

	

};
