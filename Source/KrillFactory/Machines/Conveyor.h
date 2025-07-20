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

	// �޸� Ǯ�� ������ �ִ� ��� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Conveyor")
	int32 MaxBlockPoolSize;

private:
	// ----- Ǯ�� ���� ������ -----
	UPROPERTY()
	TArray<AKrillBlock*> BlockPool; // ��� ����� ���� Ǯ
	
	TQueue<AKrillBlock*> AvailableBlocks; // ���� ��� ������ (��Ȱ��ȭ��) ��� ť

	UPROPERTY()
	TArray<AKrillBlock*> ActiveBlocks; // ���� Ȱ��ȭ�Ǿ� �����̾� ���� �ִ� ��ϵ�

	TArray<float> ActiveBlockDistances; // ActiveBlocks�� �ش��ϴ� �� ����� ���� ���ö��� �Ÿ�

	// ----- Ÿ�̸� ���� ������ -----
	FTimerHandle BlockSpawnTimerHandle; // ��� ���� Ÿ�̸� �ڵ�
	int32 NumBlocksToSpawn; // ������ ����� �� ����
	int32 BlocksSpawnedCount; // ������� ���Ե� ��� ����

private:
	// ----- Ǯ�� �� ���� �Լ��� -----
	void InitializeBlockPool(); // Ǯ �ʱ�ȭ
	void TrySpawnNextBlock(); // Ÿ�̸ӿ� ���� ȣ��� �Լ� (���� ��� ���� �õ�)
	AKrillBlock* GetBlockFromPool(); // Ǯ���� ����� �������� �Լ�
	void ReturnBlockToPool(AKrillBlock* BlockToReturn); // ����� Ǯ�� ��ȯ�ϴ� �Լ�

	// ���ö��� ���� ������ ���� ��ġ �� ȸ��
	FVector SplineStartLocation;
	FRotator SplineStartRotation;
};
