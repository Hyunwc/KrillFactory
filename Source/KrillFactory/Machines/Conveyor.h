// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h"
//#include "Products/KrillBlock.h"
#include "Conveyor.generated.h"

class AKrillBlock;

USTRUCT(BlueprintType)
struct FBlockPool
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<AKrillBlock*> Blocks;
};

USTRUCT(BlueprintType)
struct FActiveBlockInfo
{
	GENERATED_BODY()

	UPROPERTY()
	AKrillBlock* Block;

	UPROPERTY()
	float DistanceAlongSpline;
};

UCLASS()
class KRILLFACTORY_API AConveyor : public AActor
{
	GENERATED_BODY()
	
public:	
	AConveyor();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type Reason) override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USplineComponent* Spline; // 블록들 이동 경로

	UPROPERTY(VisibleAnywhere, Category = "Power")
	bool bPowerOn; // 기계의 전원

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor")
	float MoveSpeed; // 컨베이어 속도

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor")
	float BlockSpawnInterval; 	// 다음 블록을 투입할 시간 간격

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor")
	int32 NumBlocksToSpawn; // 투입할 블록의 총 개수

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor")
	int32 BlocksSpawnedCount; // 현재까지 투입된 블록 개수
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Conveyor")
	TArray<FActiveBlockInfo> ActiveBlocks; // 현재 활성화되어 컨베이어 위에 있는 블록들

	UPROPERTY(EditAnywhere, Category = "BlockPool")
	TMap<EBlockType, TSubclassOf<AKrillBlock>> BlockBlueprints; // 블록 풀링을 위한 맵
	
	UPROPERTY(EditAnywhere, Category = "BlockPool")
	TMap<EBlockType, int32> MaxBlockPoolSizes; // 타입별 최대 풀 크기

private:

	UPROPERTY()
	TMap<EBlockType, FBlockPool> BlockPool; // 실제 블록풀

	// ----- 타이머 관련 변수들 -----
	FTimerHandle BlockSpawnTimerHandle; // 블록 투입 타이머 핸들
	
	// 스플라인 시작 지점의 월드 위치 및 회전
	FVector SplineStartLocation;
	FRotator SplineStartRotation;

public:
	void InitializeBlockPool(); // 풀 초기화

	void TrySpawnNextBlock(); // 타이머에 의해 호출될 함수 (다음 블록 투입 시도)

	UFUNCTION(BlueprintCallable, Category = "Conveyor Pool")
	AKrillBlock* GetBlockFromPool(EBlockType Type); // 풀에서 블록을 가져오는 함수

	UFUNCTION(BlueprintCallable, Category = "Conveyor Pool")
	void ReturnBlockToPool(AKrillBlock* BlockToReturn); // 블록을 풀로 반환하는 함수

	// 특정 위치에 블록을 스플라인에 추가하는 함수(절삭기용)
	UFUNCTION(BlueprintCallable, Category = "Conveyor Management")
	void AddBlockToConveyor(AKrillBlock* Block, const FVector& WorldLocation, const FRotator& WorldRotation);
};
