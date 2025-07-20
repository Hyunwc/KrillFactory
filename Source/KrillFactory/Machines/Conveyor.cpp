// Fill out your copyright notice in the Description page of Project Settings.


#include "Machines/Conveyor.h"
#include "Components/SplineComponent.h"
#include "Products/KrillBlock.h"
#include "Kismet/GameplayStatics.h"

AConveyor::AConveyor()
{
	PrimaryActorTick.bCanEverTick = true;

	Spline = CreateDefaultSubobject< USplineComponent>(TEXT("Spline"));
	SetRootComponent(Spline);

	bPowerOn = false;
	MoveSpeed = 0.0f;

	MoveSpeed = 100.0f;
	BlockSpawnInterval = 2.0f;  // 2�� �������� ��� ����
	MaxBlockPoolSize = 50;
	NumBlocksToSpawn = 10;
	BlocksSpawnedCount = 0;
}

void AConveyor::BeginPlay()
{
	Super::BeginPlay();
	
	if (Spline->GetNumberOfSplinePoints() < 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("Conveyor : Spline needs at least 2 points for movement!"));
	}
	// ���ö��� ���� ������ ��ġ�� ȸ�� ĳ��
	SplineStartLocation = Spline->GetLocationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World);
	SplineStartRotation = Spline->GetRotationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World);

	// ��� Ǯ �ʱ�ȭ
	InitializeBlockPool();

	// ù ��� ���� Ÿ�̸� ����(ó���� �ٷ� ����)
	if (NumBlocksToSpawn > 0)
	{
		GetWorldTimerManager().SetTimer(BlockSpawnTimerHandle, this, &AConveyor::TrySpawnNextBlock, BlockSpawnInterval, true, 0.0f);
	}
}

void AConveyor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float SplineLength = Spline->GetSplineLength();

	// ���� Ȱ��ȭ�� ��ϵ��� ��ġ ������Ʈ
	for (int32 i = ActiveBlocks.Num() - 1; i >= 0; i--)
	{
		AKrillBlock* CurrentKrill = ActiveBlocks[i];
		if (IsValid(CurrentKrill))
		{
			ActiveBlockDistances[i] += MoveSpeed * DeltaTime;

			// ���ö��� ���� �����ߴ��� Ȯ��
			if (ActiveBlockDistances[i] >= SplineLength)
			{
				// Ǯ�� ��ȯ
				ReturnBlockToPool(CurrentKrill);

				// ActiveBlocks�� ActiveBlockDistances �迭���� ����
				ActiveBlocks.RemoveAt(i);
				ActiveBlockDistances.RemoveAt(i);
			}
			else
			{
				// ��� ��ġ ������Ʈ
				FVector NewLocation = Spline->GetLocationAtDistanceAlongSpline(ActiveBlockDistances[i], ESplineCoordinateSpace::World);
				FRotator NewRotation = Spline->GetRotationAtDistanceAlongSpline(ActiveBlockDistances[i], ESplineCoordinateSpace::World);
				CurrentKrill->SetActorLocation(NewLocation);
				CurrentKrill->SetActorRotation(NewRotation);
			}
		}
		else
		{
			ActiveBlocks.RemoveAt(i);
			ActiveBlockDistances.RemoveAt(i);
		}
	}
}

void AConveyor::InitializeBlockPool()
{
	BlockPool.Empty();
	AvailableBlocks.Empty();

	// ��� �Ҵ�
	for (int32 i = 0; i < MaxBlockPoolSize; i++)
	{
		AKrillBlock* NewKrill = GetWorld()->SpawnActor<AKrillBlock>(GetActorLocation(), FRotator::ZeroRotator);
		if (NewKrill)
		{
			BlockPool.Add(NewKrill);
			ReturnBlockToPool(NewKrill); // Ǯ�� ��ȯ�ϸ鼭 ��Ȱ��ȭ
		}
	}
	UE_LOG(LogTemp, Log, TEXT("Conveyor : Initialized block pool with %d blocks."), BlockPool.Num());
}

void AConveyor::TrySpawnNextBlock()
{
	// ���� < �� ����
	if (BlocksSpawnedCount < NumBlocksToSpawn)
	{
		AKrillBlock* NewActiveBlock = GetBlockFromPool();
		if (NewActiveBlock)
		{
			// ���ö��� ���� ������ ��� ��ġ
			NewActiveBlock->SetActorLocation(SplineStartLocation);
			NewActiveBlock->SetActorRotation(SplineStartRotation);

			ActiveBlocks.Add(NewActiveBlock);
			ActiveBlockDistances.Add(0.0f); // �� ����� ���ö��� ���� ����(0.0f)���� ����
			BlocksSpawnedCount++;
			UE_LOG(LogTemp, Log, TEXT("Conveyor : Spawned block %d / %d."), BlocksSpawnedCount, NumBlocksToSpawn);
		}
	}
	else
	{
		// ��� ����� �����ߴٸ� Ÿ�̸� ����
		GetWorldTimerManager().ClearTimer(BlockSpawnTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("Conveyor : All %d blocks Spawned."), NumBlocksToSpawn);
	}
}

AKrillBlock* AConveyor::GetBlockFromPool()
{
	AKrillBlock* Krill = nullptr;
	if (AvailableBlocks.Dequeue(Krill))
	{
		if (IsValid(Krill))
		{
			Krill->SetActorHiddenInGame(false); // 
			Krill->SetActorEnableCollision(true); // 
			//Krill->SetActorTickEnabled(false); // ����� ƽ�� ������ ���߿� ����� �� ������ �ּ�
			return Krill;
		}
	}

	return nullptr;
}

void AConveyor::ReturnBlockToPool(AKrillBlock* BlockToReturn)
{
	if (IsValid(BlockToReturn))
	{
		BlockToReturn->SetActorHiddenInGame(true); // �Ⱥ��̰�
		BlockToReturn->SetActorEnableCollision(false); // �ݸ��� ����
		//BlockToReturn->SetActorTickEnabled(false); // ����� ƽ�� ������ ���߿� ����� �� ������ �ּ�
		AvailableBlocks.Enqueue(BlockToReturn); // ť�� ��ȯ
	}
}

