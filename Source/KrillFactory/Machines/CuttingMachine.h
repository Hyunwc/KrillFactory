// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CuttingMachine.generated.h"

class AConveyor;
class AKrillBlock;
class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class KRILLFACTORY_API ACuttingMachine : public AActor
{
	GENERATED_BODY()
	
public:	
	ACuttingMachine();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnCuttingZoneOverlapBegin(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCuttingZoneOverlapEnd(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:	
	virtual void Tick(float DeltaTime) override;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BladeMesh; // 칼날

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* CuttingZone; // 컷팅 이벤트 발생 영역

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cutting")
	float EightSpawnInterval;

	// 스폰 대기 중인 Eighth 블록들을 저장할 배열
	UPROPERTY(VisibleAnywhere)
	TArray<AKrillBlock*> PendingSpawnEighthBlocks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cutting")
	FVector CuttingZoneExitLocation;

public:
	UPROPERTY()
	class AMainPower* MainPower; // 델리게이트 사용을 위한 참조

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Power")
	bool bIsPowerOn; // 기계의 전원

	// 델리게이트에 등록할 함수
	UFUNCTION()
	void OnMainPowerStateChanged(bool bPowerOn);

public:
	UFUNCTION()
	void SpawnNextEighthBlock();
private:
	// 풀 기능을 사용하기 위해 컨베이어 인스턴스를 월드에서
	UPROPERTY()
	AConveyor* FoundConveyor;

	FTimerHandle EighthTimer;
};
