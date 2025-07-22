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
	UStaticMeshComponent* BladeMesh; // 칼날

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* CuttingZone; // 컷팅 이벤트 발생 영역

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CuttingMachine")
	int32 NumberOfBlades; // 칼날 개수

private:
	// 실제 칼날 메시 인스턴스를 저장할 배열
	UPROPERTY()
	TArray<UStaticMeshComponent*> BladeInstance;

	// 풀 기능을 사용하기 위해 컨베이어 인스턴스를 월드에서
	UPROPERTY()
	AConveyor* FoundConveyor;

	// 현재 컷팅 존에 있는 블록들을 추적
	UPROPERTY()
	TSet<AKrillBlock*> BlocksInCuttingZone;

	// 잘린 블록이 다시 들어오는 것을 막기 위한 쿨다운(옵션)
	FTimerHandle CuttingCooldownTimerHandle;
	void ClearCuttingCooldown();
	bool bIncuttingCooldown;
	void SpawnBlades(); // 칼날 메시 여러 개 생성 함수 (블루프린트에서 설정??)
};
