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
	//MaxBlockPoolSize = 50;
	NumBlocksToSpawn = 50;
	BlocksSpawnedCount = 0;

	// 타입별 기본 풀 크기 설정(에디터에서 오버라이드 가능)
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
	// 스플라인 시작 지점의 위치와 회전 캐시
	SplineStartLocation = Spline->GetLocationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World);
	SplineStartRotation = Spline->GetRotationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World);

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

	// TMap에 저장된 모든 TQueue 포인터들을 순회하며 메모리 해제
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

	// 현재 활성화된 블록들의 위치 업데이트
	for (int32 i = ActiveBlocks.Num() - 1; i >= 0; i--)
	{
		FActiveBlockInfo& CurrentInfo = ActiveBlocks[i];
	
		if (IsValid(CurrentInfo.Block))
		{
			CurrentInfo.DistanceAlongSpline += MoveSpeed * DeltaTime;

			// 스플라인 상의 새로운 중심 위치와 회전 가져오기
			FVector SplineLocation = Spline->GetLocationAtDistanceAlongSpline(CurrentInfo.DistanceAlongSpline, ESplineCoordinateSpace::World);
			FRotator SplineRotation = Spline->GetRotationAtDistanceAlongSpline(CurrentInfo.DistanceAlongSpline, ESplineCoordinateSpace::World);
			FVector SplineUpVector = Spline->GetUpVectorAtDistanceAlongSpline(CurrentInfo.DistanceAlongSpline, ESplineCoordinateSpace::World);
			FVector SplineTangent = Spline->GetTangentAtDistanceAlongSpline(CurrentInfo.DistanceAlongSpline, ESplineCoordinateSpace::World);
			SplineTangent.Normalize();
			
			FVector SplineRightVector = FVector::CrossProduct(SplineTangent, SplineUpVector);
			SplineRightVector.Normalize();

			FVector FinalBlockLocation = SplineLocation
				+ SplineRightVector * CurrentInfo.RelativeOffsetFromSpline.Y
				+ SplineUpVector * CurrentInfo.RelativeOffsetFromSpline.Z;

			CurrentInfo.Block->SetActorLocation(FinalBlockLocation);
			CurrentInfo.Block->SetActorRotation(SplineRotation);
			//
			//// 스플라인 끝에 도달했는지 확인
			//if (CurrentInfo.DistanceAlongSpline >= SplineLength)
			//{
			//	// 풀로 반환
			//	ReturnBlockToPool(CurrentInfo.Block);
			//}
			//else
			//{
			//	// 블록 위치 업데이트
			//	FVector NewLocation = Spline->GetLocationAtDistanceAlongSpline(CurrentInfo.DistanceAlongSpline, ESplineCoordinateSpace::World);
			//	FRotator NewRotation = Spline->GetRotationAtDistanceAlongSpline(CurrentInfo.DistanceAlongSpline, ESplineCoordinateSpace::World);
			//	CurrentInfo.Block->SetActorLocation(NewLocation);
			//	CurrentInfo.Block->SetActorRotation(NewRotation);
			//}
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
				// 생성 시 바로 비활성화 및 타입 설정
				NewBlock->SetActorHiddenInGame(true);
				NewBlock->SetActorEnableCollision(false);
				NewBlock->SetActorTickEnabled(false);
				NewBlock->SetBlockType(CurrentType); // 풀에 들어갈 때 해당 타입으로 설정

				// 해당 타입의 큐에 블록 추가
				AvailableBlockPools[CurrentType]->Enqueue(NewBlock);
			}
		}
	}
	
	UE_LOG(LogTemp, Log, TEXT("Conveyor : All block pools Initialized."));
}

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
			NewActiveBlock->SetActorRotation(SplineStartRotation);

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
		// ActiveBlocks 배열에서 블록 제거
		int32 FoundIndex = ActiveBlocks.IndexOfByPredicate([&](const FActiveBlockInfo& Info)
			{
				return Info.Block == BlockToReturn;
			});

		if (FoundIndex != INDEX_NONE)
		{
			ActiveBlocks.RemoveAt(FoundIndex);
		}

		BlockToReturn->SetActorHiddenInGame(true); // 안보이게
		BlockToReturn->SetActorEnableCollision(false); // 콜리전 해제
		BlockToReturn->SetActorTickEnabled(false); // 현재는 틱이 없지만 나중에 사용할 수 있으니 주석
		
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

		// WorldLocation에 가장 가까운 스플라인 상의 입력 키와 거리 계산
		float ClosestInputKey = Spline->FindInputKeyClosestToWorldLocation(WorldLocation);
		float Distance = Spline->GetDistanceAlongSplineAtSplineInputKey(ClosestInputKey);

		// 스플라인 상의 해당 지점 (중심) 위치
		FVector SplineLocationAtClosestPoint = Spline->GetLocationAtSplineInputKey(ClosestInputKey, ESplineCoordinateSpace::World);
		// 스플라인 상의 접선 방향 (블록의 전방 방향)
		FVector SplineTangentAtClosestPoint = Spline->GetTangentAtSplineInputKey(ClosestInputKey, ESplineCoordinateSpace::World);
		SplineTangentAtClosestPoint.Normalize();

		// 스플라인의 Up 벡터 (블록의 상방 방향)
		FVector SplineUpVector = Spline->GetUpVectorAtSplineInputKey(ClosestInputKey, ESplineCoordinateSpace::World);
		SplineUpVector.Normalize();

		// 스팔라인의 Right 벡터 (블록의 옆 방향) 
		FVector SplineRightVector = FVector::CrossProduct(SplineTangentAtClosestPoint, SplineUpVector);
		SplineRightVector.Normalize();

		// 블록의 월드 위치에서 스플라인 중심 위치를 뺀 벡터
		FVector DeltaVector = WorldLocation - SplineLocationAtClosestPoint;

		FVector RelativeOffset = FVector::ZeroVector;
		RelativeOffset.Y = FVector::DotProduct(DeltaVector, SplineRightVector);
		RelativeOffset.Z = FVector::DotProduct(DeltaVector, SplineUpVector);

		FActiveBlockInfo NewInfo;
		NewInfo.Block = Block;
		NewInfo.DistanceAlongSpline = Distance;
		NewInfo.RelativeOffsetFromSpline = RelativeOffset;
		ActiveBlocks.Add(NewInfo);
	}
}

