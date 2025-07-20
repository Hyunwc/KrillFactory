// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h"
#include "Conveyor.generated.h"

class AKrillBlock;

UCLASS()
class KRILLFACTORY_API AConveyor : public AActor
{
	GENERATED_BODY()
	
public:	
	AConveyor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	// 블록들 이동 경로
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USplineComponent* Spline;

	// 기계의 전원
	UPROPERTY(VisibleAnywhere, Category = "Power")
	bool bPowerOn;

	// 컨베이어 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor")
	float MoveSpeed;

	// 다음 블록을 투입할 시간 간격
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor")
	float BlockSpawnInterval;

	// 메모리 풀에 생성할 최대 블록 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor")
	int32 MaxBlockPoolSize;

private:
	// ----- 풀링 관련 변수들 -----
	UPROPERTY()
	TArray<AKrillBlock*> BlockPool; // 모든 블록을 담을 풀
	
	TQueue<AKrillBlock*> AvailableBlocks; // 현재 사용 가능한 (비활성화된) 블록 큐

	UPROPERTY()
	TArray<AKrillBlock*> ActiveBlocks; // 현재 활성화되어 컨베이어 위에 있는 블록들

	TArray<float> ActiveBlockDistances; // ActiveBlocks에 해당하는 각 블록의 현재 스플라인 거리

	// ----- 타이머 관련 변수들 -----
	FTimerHandle BlockSpawnTimerHandle; // 블록 투입 타이머 핸들
	int32 NumBlocksToSpawn; // 투입할 블록의 총 개수
	int32 BlocksSpawnedCount; // 현재까지 투입된 블록 개수

private:
	// ----- 풀링 및 투입 함수들 -----
	void InitializeBlockPool(); // 풀 초기화
	void TrySpawnNextBlock(); // 타이머에 의해 호출될 함수 (다음 블록 투입 시도)
	AKrillBlock* GetBlockFromPool(); // 풀에서 블록을 가져오는 함수
	void ReturnBlockToPool(AKrillBlock* BlockToReturn); // 블록을 풀로 반환하는 함수

	// 스플라인 시작 지점의 월드 위치 및 회전
	FVector SplineStartLocation;
	FRotator SplineStartRotation;
};
