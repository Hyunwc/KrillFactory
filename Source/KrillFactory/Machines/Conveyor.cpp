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
	BlockSpawnInterval = 2.0f;  // 2초 간격으로 블록 투입
	NumBlocksToSpawn = 50;
	BlocksSpawnedCount = 0;

	// 타입별 기본 풀 크기 설정(에디터에서 오버라이드 가능)
	MaxBlockPoolSizes.Add(EBlockType::EBT_Full, 50);
	MaxBlockPoolSizes.Add(EBlockType::EBT_Quarter, 30);
	MaxBlockPoolSizes.Add(EBlockType::EBT_Eighth, 100);
}

void AConveyor::BeginPlay()
{
	Super::BeginPlay();
	
	if (Spline->GetNumberOfSplinePoints() < 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("Conveyor : 스플라인은 2개의 포인트를 필요로 합니다!!"));
	}
	// 스플라인 시작 지점의 위치와 회전 캐시
	SplineStartLocation = Spline->GetLocationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World);
	//SplineStartRotation = Spline->GetRotationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World);

	// 블록 풀 초기화
	InitializeBlockPool();

	// 첫 블록 투입 타이머 시작(처음엔 바로 투입)
	if (NumBlocksToSpawn > 0)
	{
		GetWorldTimerManager().SetTimer(BlockSpawnTimerHandle, this, &AConveyor::TrySpawnNextBlock, BlockSpawnInterval, true, 0.0f);
	}
}

void AConveyor::EndPlay(const EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);
}

void AConveyor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float SplineLength = Spline->GetSplineLength();

	// 현재 활성화된 블록들의 위치 업데이트
	for (int32 i = ActiveBlocks.Num() - 1; i >= 0; i--)
	{
		FActiveBlockInfo& CurrentInfo = ActiveBlocks[i];
	
		if (IsValid(CurrentInfo.Block))
		{
			CurrentInfo.DistanceAlongSpline += MoveSpeed * DeltaTime;

			// 끝에 도달했다면
			if (CurrentInfo.DistanceAlongSpline >= SplineLength)
			{
				ReturnBlockToPool(CurrentInfo.Block);
			}
			else
			{
				FVector SplineLocation = Spline->GetLocationAtDistanceAlongSpline(CurrentInfo.DistanceAlongSpline, ESplineCoordinateSpace::World);
				//FRotator SplineRotation = Spline->GetRotationAtDistanceAlongSpline(CurrentInfo.DistanceAlongSpline, ESplineCoordinateSpace::World);

				CurrentInfo.Block->SetActorLocation(SplineLocation);
				//CurrentInfo.Block->SetActorRotation(SplineRotation);
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
	for (const auto& Pair : MaxBlockPoolSizes)
	{
		EBlockType Type = Pair.Key;
		int32 Size = Pair.Value;

		// 에디터에서 설정된 해당 EBlockType의 블루프린트 클래스를 가져옴
		TSubclassOf<AKrillBlock> BlockBPClass = BlockBlueprints.FindRef(Type);
		
		if (!BlockBPClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("Conveyor : 해당 블루프린트가 없습니다"));
			continue;
		}

		// 해당 타입의 풀을 초기화하거나 찾는다.
		//TArray<AKrillBlock*>& PoolArray = BlockPool.FindOrAdd(Type);
		
		FBlockPool& Pool = BlockPool.FindOrAdd(Type);

		// 풀 크기만큼 블록을 미리 스폰하여 담는ㄷ.
		// 풀크기만큼 할당
		for (int32 j = 0; j < Size; j++)
		{
			AKrillBlock* NewBlock = GetWorld()->SpawnActor<AKrillBlock>(BlockBPClass, FVector::ZeroVector, FRotator::ZeroRotator);
			if (NewBlock)
			{
				// 생성 시 바로 비활성화 및 타입 설정
				NewBlock->SetActorHiddenInGame(true);
				NewBlock->SetActorEnableCollision(false);
				NewBlock->SetActorTickEnabled(false);
				NewBlock->SetBlockType(Type); // 풀에 들어갈 때 해당 타입으로 설정
				Pool.Blocks.Add(NewBlock);
			}
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("Conveyor : 모든 블록을 풀에 담았습니다!"));
}

// 블록 스폰 함수
void AConveyor::TrySpawnNextBlock()
{
	// 현재 < 총 갯수
	if (BlocksSpawnedCount < NumBlocksToSpawn)
	{
		AKrillBlock* NewActiveBlock = GetBlockFromPool(EBlockType::EBT_Full);
		if (NewActiveBlock)
		{
			// 스플라인 시작 지점에 블록 배치
			NewActiveBlock->SetActorLocation(SplineStartLocation);
			//NewActiveBlock->SetActorRotation(SplineStartRotation);

			FActiveBlockInfo NewInfo;
			NewInfo.Block = NewActiveBlock;
			NewInfo.DistanceAlongSpline = 0.0f; // 스플라인 시작 지점
			ActiveBlocks.Add(NewInfo);
			
			BlocksSpawnedCount++;
			UE_LOG(LogTemp, Log, TEXT("Conveyor : Spawned block %d / %d."), BlocksSpawnedCount, NumBlocksToSpawn);
		}
	}
	else
	{
		// 모든 블록을 투입했다면 타이머 중지
		GetWorldTimerManager().ClearTimer(BlockSpawnTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("Conveyor : All %d blocks Spawned."), NumBlocksToSpawn);
	}
}

AKrillBlock* AConveyor::GetBlockFromPool(EBlockType Type)
{
	//TArray<AKrillBlock*>* PoolArray = BlockPool.Find(Type);
	FBlockPool* PoolPtr = BlockPool.Find(Type);

	if (PoolPtr && PoolPtr->Blocks.Num() > 0)
	{
		// TArray의 Pop(false)는 배열의 마지막 요소를 제거하고 반환해준다.
		AKrillBlock* Block = PoolPtr->Blocks.Pop(false);
		if (IsValid(Block))
		{
			Block->SetActorHiddenInGame(false);
			Block->SetActorEnableCollision(true);
			Block->SetActorTickEnabled(true);
			Block->SetBlockType(Type); // 다시 설정
			return Block;
		}
	}
	return nullptr;
}

void AConveyor::ReturnBlockToPool(AKrillBlock* BlockToReturn)
{
	if (IsValid(BlockToReturn))
	{
		ActiveBlocks.RemoveAll([&](const FActiveBlockInfo& Info) { return Info.Block == BlockToReturn;});

		BlockToReturn->SetActorHiddenInGame(true); // 안보이게
		BlockToReturn->SetActorEnableCollision(false); // 콜리전 해제
		BlockToReturn->SetActorTickEnabled(false); // 현재는 틱이 없지만 나중에 사용할 수 있으니 주석
		
		//TArray<AKrillBlock*>* PoolArray = BlockPool.Find(BlockToReturn->BlockType);
		FBlockPool* PoolPtr = BlockPool.Find(BlockToReturn->BlockType);
		if (PoolPtr)
		{
			PoolPtr->Blocks.Add(BlockToReturn);
		}
		else
		{
			BlockToReturn->Destroy();
		}
	}
}

void AConveyor::AddBlockToConveyor(AKrillBlock* Block, const FVector& WorldLocation/*, const FRotator& WorldRotation*/)
{
	if (Block && Spline)
	{
		// 블록의 월드위치와 회전을 설정
		Block->SetActorLocation(WorldLocation);
		//Block->SetActorRotation(WorldRotation);

		float ClosestInputKey = Spline->FindInputKeyClosestToWorldLocation(WorldLocation);
		float Distance = Spline->GetDistanceAlongSplineAtSplineInputKey(ClosestInputKey);

		FActiveBlockInfo NewInfo;
		NewInfo.Block = Block;
		NewInfo.DistanceAlongSpline = Distance;
		ActiveBlocks.Add(NewInfo);
	}
}

