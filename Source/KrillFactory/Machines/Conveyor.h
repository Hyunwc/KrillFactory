// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Containers/Queue.h"
//#include "Products/KrillBlock.h"
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

	virtual void EndPlay(const EEndPlayReason::Type Reason) override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	// ��ϵ� �̵� ���
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USplineComponent* Spline;

	// ����� ����
	UPROPERTY(VisibleAnywhere, Category = "Power")
	bool bPowerOn;

	// �����̾� �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor")
	float MoveSpeed;

	// ���� ����� ������ �ð� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor")
	float BlockSpawnInterval;

	// Ÿ�Ժ� �ִ� Ǯ ũ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor Pool")
	TMap<EBlockType, int32> MaxBlockPoolSizes;

private:
	// Ÿ�Ժ��� ��� ������ ��� ť�� �����ϴ� TMap
	TMap<EBlockType, TQueue<AKrillBlock*>*> AvailableBlockPools;

	// ���� Ȱ��ȭ�Ǿ� �����̾� ���� �ִ� ��ϵ�(��ϰ� �� ���ö��� �Ÿ��� ����)
	struct FActiveBlockInfo
	{
		UPROPERTY()
		AKrillBlock* Block;
		float DistanceAlongSpline;
	};
	TArray<FActiveBlockInfo> ActiveBlocks;

	// ----- Ÿ�̸� ���� ������ -----
	FTimerHandle BlockSpawnTimerHandle; // ��� ���� Ÿ�̸� �ڵ�
	int32 NumBlocksToSpawn; // ������ ����� �� ����
	int32 BlocksSpawnedCount; // ������� ���Ե� ��� ����

	// ���ö��� ���� ������ ���� ��ġ �� ȸ��
	FVector SplineStartLocation;
	FRotator SplineStartRotation;

	// ----- Ǯ�� �� ���� �Լ��� -----
	void InitializeBlockPool(); // Ǯ �ʱ�ȭ
	void TrySpawnNextBlock(); // Ÿ�̸ӿ� ���� ȣ��� �Լ� (���� ��� ���� �õ�)

public:
	// �ܺο��� ȣ���� �Լ���
	UFUNCTION(BlueprintCallable, Category = "Conveyor Pool")
	AKrillBlock* GetBlockFromPool(EBlockType Type); // Ǯ���� ����� �������� �Լ�

	UFUNCTION(BlueprintCallable, Category = "Conveyor Pool")
	void ReturnBlockToPool(AKrillBlock* BlockToReturn); // ����� Ǯ�� ��ȯ�ϴ� �Լ�

	// Ư�� ��ġ�� ����� ���ö��ο� �߰��ϴ� �Լ�(������)
	UFUNCTION(BlueprintCallable, Category = "Conveyor Management")
	void AddBlockToConveyorAtWorldLocation(AKrillBlock* Block, const FVector& WorldLocation, const FRotator& WorldRotation);
};
