// Fill out your copyright notice in the Description page of Project Settings.


#include "Machines/Conveyor.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Products/KrillBlock.h"

AConveyor::AConveyor()
{
	PrimaryActorTick.bCanEverTick = true;

	Spline = CreateDefaultSubobject< USplineComponent>(TEXT("Spline"));
	SetRootComponent(Spline);

	bPowerOn = false;

	MoveSpeed = 100.0f;
	BlockSpawnInterval = 2.0f;  // 2�� �������� ��� ����
	//MaxBlockPoolSize = 50;
	NumBlocksToSpawn = 50;
	BlocksSpawnedCount = 0;

	// Ÿ�Ժ� �⺻ Ǯ ũ�� ����(�����Ϳ��� �������̵� ����)
	MaxBlockPoolSizes.Add(EBlockType::EBT_Full, 50);
	MaxBlockPoolSizes.Add(EBlockType::EBT_Quarter, 200);
	MaxBlockPoolSizes.Add(EBlockType::EBT_Eighth, 400);
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

void AConveyor::EndPlay(const EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);

	// TMap�� ����� ��� TQueue �����͵��� ��ȸ�ϸ� �޸� ����
	for (auto& Elem : AvailableBlockPools)
	{
		delete Elem.Value;
		Elem.Value = nullptr;
	}
	AvailableBlockPools.Empty();
}

void AConveyor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float SplineLength = Spline->GetSplineLength();

	// ���� Ȱ��ȭ�� ��ϵ��� ��ġ ������Ʈ
	for (int32 i = ActiveBlocks.Num() - 1; i >= 0; i--)
	{
		FActiveBlockInfo& CurrentInfo = ActiveBlocks[i];
	
		if (IsValid(CurrentInfo.Block))
		{
			CurrentInfo.DistanceAlongSpline += MoveSpeed * DeltaTime;

			// ���ö��� ���� �����ߴ��� Ȯ��
			if (CurrentInfo.DistanceAlongSpline >= SplineLength)
			{
				// Ǯ�� ��ȯ
				ReturnBlockToPool(CurrentInfo.Block);
			}
			else
			{
				// ��� ��ġ ������Ʈ
				FVector NewLocation = Spline->GetLocationAtDistanceAlongSpline(CurrentInfo.DistanceAlongSpline, ESplineCoordinateSpace::World);
				FRotator NewRotation = Spline->GetRotationAtDistanceAlongSpline(CurrentInfo.DistanceAlongSpline, ESplineCoordinateSpace::World);
				CurrentInfo.Block->SetActorLocation(NewLocation);
				CurrentInfo.Block->SetActorRotation(NewRotation);
			}
		}
		else
		{
			ActiveBlocks.RemoveAt(i);
		}
	}
}

void AConveyor::InitializeBlockPool()
{
	for (int32 i = static_cast<int32>(EBlockType::EBT_Full); i < static_cast<int32>(EBlockType::EBT_Max); i++)
	{
		EBlockType CurrentType = static_cast<EBlockType>(i);
		int32 PoolSize = MaxBlockPoolSizes.Contains(CurrentType) ? MaxBlockPoolSizes[CurrentType] : 10;

		//UE_LOG(LogTemp, Log, TEXT("Conveyor : Initializing pool for type %s with size %d"), *UEnum::GetValueAsString(TEXT("EBlockType"), CurrentType), PoolSize);

		if (!AvailableBlockPools.Contains(CurrentType))
		{
			AvailableBlockPools.Add(CurrentType, new TQueue<AKrillBlock*>());
		}
		for (int32 j = 0; j < PoolSize; j++)
		{
			AKrillBlock* NewBlock = GetWorld()->SpawnActor<AKrillBlock>(GetActorLocation(), FRotator::ZeroRotator);
			if (NewBlock)
			{
				// ���� �� �ٷ� ��Ȱ��ȭ �� Ÿ�� ����
				NewBlock->SetActorHiddenInGame(true);
				NewBlock->SetActorEnableCollision(false);
				NewBlock->SetActorTickEnabled(false);
				NewBlock->SetBlockType(CurrentType); // Ǯ�� �� �� �ش� Ÿ������ ����

				// �ش� Ÿ���� ť�� ��� �߰�
				AvailableBlockPools[CurrentType]->Enqueue(NewBlock);
			}
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("Conveyor : All block pools Initialized."));
}

void AConveyor::TrySpawnNextBlock()
{
	// ���� < �� ����
	if (BlocksSpawnedCount < NumBlocksToSpawn)
	{
		AKrillBlock* NewActiveBlock = GetBlockFromPool(EBlockType::EBT_Full);
		if (NewActiveBlock)
		{
			// ���ö��� ���� ������ ��� ��ġ
			NewActiveBlock->SetActorLocation(SplineStartLocation);
			NewActiveBlock->SetActorRotation(SplineStartRotation);

			FActiveBlockInfo NewInfo;
			NewInfo.Block = NewActiveBlock;
			NewInfo.DistanceAlongSpline = 0.0f; // ���ö��� ���� ����
			ActiveBlocks.Add(NewInfo);
			
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


AKrillBlock* AConveyor::GetBlockFromPool(EBlockType Type)
{
	TQueue<AKrillBlock*>* PoolPtr = AvailableBlockPools.FindRef(Type);

	if (!PoolPtr)
	{
		return nullptr;
	}

	AKrillBlock* Block = nullptr;
	if (PoolPtr->Dequeue(Block))
	{
		if (IsValid(Block))
		{
			Block->SetActorHiddenInGame(false);
			Block->SetActorEnableCollision(true);
			Block->SetActorTickEnabled(true);
			Block->SetBlockType(Type); // �ٽ� ����
			return Block;
		}
	}
	return nullptr;
}

void AConveyor::ReturnBlockToPool(AKrillBlock* BlockToReturn)
{
	if (IsValid(BlockToReturn))
	{
		// ActiveBlocks �迭���� ��� ����
		int32 FoundIndex = ActiveBlocks.IndexOfByPredicate([&](const FActiveBlockInfo& Info)
			{
				return Info.Block == BlockToReturn;
			});

		if (FoundIndex != INDEX_NONE)
		{
			ActiveBlocks.RemoveAt(FoundIndex);
		}

		BlockToReturn->SetActorHiddenInGame(true); // �Ⱥ��̰�
		BlockToReturn->SetActorEnableCollision(false); // �ݸ��� ����
		BlockToReturn->SetActorTickEnabled(false); // ����� ƽ�� ������ ���߿� ����� �� ������ �ּ�
		
		TQueue<AKrillBlock*>* PoolPtr = AvailableBlockPools.FindRef(BlockToReturn->BlockType);
		if (PoolPtr)
		{
			PoolPtr->Enqueue(BlockToReturn);
		}
		else
		{
			BlockToReturn->Destroy();
		}
	}
}

void AConveyor::AddBlockToConveyorAtWorldLocation(AKrillBlock* Block, const FVector& WorldLocation, const FRotator& WorldRotation)
{
	if (Block && Spline)
	{
		Block->SetActorLocation(WorldLocation);
		Block->SetActorRotation(WorldRotation);

		float ClosestInputKey = Spline->FindInputKeyClosestToWorldLocation(WorldLocation);

		float Distance = Spline->GetDistanceAlongSplineAtSplineInputKey(ClosestInputKey);

		FActiveBlockInfo NewInfo;
		NewInfo.Block = Block;
		NewInfo.DistanceAlongSpline = Distance;
		ActiveBlocks.Add(NewInfo);
	}
}

