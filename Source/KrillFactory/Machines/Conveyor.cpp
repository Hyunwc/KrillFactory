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
	BlockSpawnInterval = 2.0f;  // 2초 간격으로 블록 투입
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

void AConveyor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const float SplineLength = Spline->GetSplineLength();

	// 현재 활성화된 블록들의 위치 업데이트
	for (int32 i = ActiveBlocks.Num() - 1; i >= 0; i--)
	{
		AKrillBlock* CurrentKrill = ActiveBlocks[i];
		if (IsValid(CurrentKrill))
		{
			ActiveBlockDistances[i] += MoveSpeed * DeltaTime;

			// 스플라인 끝에 도달했는지 확인
			if (ActiveBlockDistances[i] >= SplineLength)
			{
				// 풀로 반환
				ReturnBlockToPool(CurrentKrill);

				// ActiveBlocks와 ActiveBlockDistances 배열에서 제거
				ActiveBlocks.RemoveAt(i);
				ActiveBlockDistances.RemoveAt(i);
			}
			else
			{
				// 블록 위치 업데이트
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

	// 블록 할당
	for (int32 i = 0; i < MaxBlockPoolSize; i++)
	{
		AKrillBlock* NewKrill = GetWorld()->SpawnActor<AKrillBlock>(GetActorLocation(), FRotator::ZeroRotator);
		if (NewKrill)
		{
			BlockPool.Add(NewKrill);
			ReturnBlockToPool(NewKrill); // 풀에 반환하면서 비활성화
		}
	}
	UE_LOG(LogTemp, Log, TEXT("Conveyor : Initialized block pool with %d blocks."), BlockPool.Num());
}

void AConveyor::TrySpawnNextBlock()
{
	// 현재 < 총 갯수
	if (BlocksSpawnedCount < NumBlocksToSpawn)
	{
		AKrillBlock* NewActiveBlock = GetBlockFromPool();
		if (NewActiveBlock)
		{
			// 스플라인 시작 지점에 블록 배치
			NewActiveBlock->SetActorLocation(SplineStartLocation);
			NewActiveBlock->SetActorRotation(SplineStartRotation);

			ActiveBlocks.Add(NewActiveBlock);
			ActiveBlockDistances.Add(0.0f); // 새 블록은 스플라인 시작 지점(0.0f)에서 시작
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

AKrillBlock* AConveyor::GetBlockFromPool()
{
	AKrillBlock* Krill = nullptr;
	if (AvailableBlocks.Dequeue(Krill))
	{
		if (IsValid(Krill))
		{
			Krill->SetActorHiddenInGame(false); // 
			Krill->SetActorEnableCollision(true); // 
			//Krill->SetActorTickEnabled(false); // 현재는 틱이 없지만 나중에 사용할 수 있으니 주석
			return Krill;
		}
	}

	return nullptr;
}

void AConveyor::ReturnBlockToPool(AKrillBlock* BlockToReturn)
{
	if (IsValid(BlockToReturn))
	{
		BlockToReturn->SetActorHiddenInGame(true); // 안보이게
		BlockToReturn->SetActorEnableCollision(false); // 콜리전 해제
		//BlockToReturn->SetActorTickEnabled(false); // 현재는 틱이 없지만 나중에 사용할 수 있으니 주석
		AvailableBlocks.Enqueue(BlockToReturn); // 큐에 반환
	}
}

