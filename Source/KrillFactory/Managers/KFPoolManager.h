// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Products/KrillBlock.h"
#include "KFPoolManager.generated.h"

class AConveyor;

USTRUCT(BlueprintType)
struct FPoolMapWrapper
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<AKrillBlock*> PoolMapArray;
};

UCLASS()
class KRILLFACTORY_API AKFPoolManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AKFPoolManager();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	// 액터를 저장할 풀
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pool")
	TMap<EBlockType, FPoolMapWrapper> PoolMap;

	// 풀사이즈
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pool")
	TMap<EBlockType, int32> PoolSize;

	// 크릴 블루프린트를 담을 Map
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pool")
	TMap<EBlockType, TSubclassOf<AKrillBlock>> BlockClasses;

	UPROPERTY(VisibleAnywhere)
	int32 Size;

	UPROPERTY(VisibleAnywhere, Category = "Conveyor")
	AConveyor* MainConveyor;

public:
	UFUNCTION()
	void InitPooling();

	UFUNCTION()
	void SpawnPool();

	UFUNCTION()
	AKrillBlock* GetPooling(EBlockType Type);

	UFUNCTION()
	void ReturnPooling(AKrillBlock* Block, EBlockType Type);

	// 특정 위치에 블록을 스플라인에 추가하는 함수
	UFUNCTION(BlueprintCallable, Category = "Conveyor Management")
	void AddBlockToConveyor(AKrillBlock* Block, const FVector& WorldLocation);
};
