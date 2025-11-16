// Fill out your copyright notice in the Description page of Project Settings.


#include "Machines/Conveyor.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Products/KrillBlock.h"
#include "Machines/MainPower.h"
#include "Managers/KFPoolManager.h"

AConveyor::AConveyor()
{
	PrimaryActorTick.bCanEverTick = true;

	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	//SetRootComponent(Spline);
	Spline->SetupAttachment(Root);

	MoveSpeed = 100.0f;
	BlockSpawnInterval = 2.0f;  // 2초 간격으로 블록 투입
	NumBlocksToSpawn = 0;
	BlocksSpawnedCount = 0;

	MainPower = nullptr;
}

void AConveyor::BeginPlay()
{
	Super::BeginPlay();
	
	SplineStartLocation = Spline->GetLocationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World);

	TArray<AActor*> FoundPowerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMainPower::StaticClass(), FoundPowerActors);
	if (FoundPowerActors.Num() > 0)
	{
		MainPower = Cast<AMainPower>(FoundPowerActors[0]);
		if (IsValid(MainPower))
		{
			MainPower->OnPowerStateChanged.AddDynamic(this, &AConveyor::OnMainPowerStateChanged);
		}
	}
}

void AConveyor::EndPlay(const EEndPlayReason::Type Reason)
{
	// 구독 해제
	if (IsValid(MainPower))
	{
		MainPower->OnPowerStateChanged.RemoveDynamic(this, &AConveyor::OnMainPowerStateChanged);
	}

	PoolManager = nullptr;
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
				PoolManager->ReturnPooling(CurrentInfo.Block, CurrentInfo.Block->BlockType);
			}
			else
			{
				FVector SplineLocation = Spline->GetLocationAtDistanceAlongSpline(CurrentInfo.DistanceAlongSpline, ESplineCoordinateSpace::World);

				CurrentInfo.Block->SetActorLocation(SplineLocation);
			}
		}
		else
		{
			ActiveBlocks.RemoveAt(i);
		}
	}
}

// 블록 스폰 함수
void AConveyor::TrySpawnNextBlock()
{
	// 현재 < 총 갯수
	if (BlocksSpawnedCount < NumBlocksToSpawn)
	{
		AKrillBlock* NewBlock = PoolManager->GetPooling(EBlockType::EBT_Full);
		if (NewBlock)
		{
			// 스플라인 시작 지점에 블록 배치
			NewBlock->SetActorLocation(SplineStartLocation);

			FActiveBlockInfo NewInfo;
			NewInfo.Block = NewBlock;
			NewInfo.DistanceAlongSpline = 0.0f; // 스플라인 시작 지점
			ActiveBlocks.Add(NewInfo);
			
			BlocksSpawnedCount++;
		}
	}
	else
	{
		// 모든 블록을 투입했다면 타이머 중지
		GetWorldTimerManager().ClearTimer(BlockSpawnTimerHandle);
	}
}

void AConveyor::AddBlock(AKrillBlock* NewBlock, const FVector& NewLocation)
{
	if (NewBlock)
	{
		NewBlock->SetActorLocation(NewLocation);

		float ClosestInputKey = Spline->FindInputKeyClosestToWorldLocation(NewLocation);
		float Distance = Spline->GetDistanceAlongSplineAtSplineInputKey(ClosestInputKey);

		FActiveBlockInfo NewInfo;
		NewInfo.Block = NewBlock;
		NewInfo.DistanceAlongSpline = Distance;
		ActiveBlocks.Add(NewInfo);
	}
}

void AConveyor::OnMainPowerStateChanged(bool bPowerOn)
{
	// 델리게이트 신호에 따라 컨베이어의 전원 상태를 업데이트
	bIsPowerOn = bPowerOn;

	if (bIsPowerOn)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan,	FString::Printf(TEXT("Conveyor : Power On")));
		// 첫 블록 투입 타이머 시작(처음엔 바로 투입)
		if (NumBlocksToSpawn > 0)
		{
			MoveSpeed = 100.0f;
			// 맨 마지막에 파라미터 1개 추가하면 첫 타이머시 간격 없이 바로 실행됨 주의!!
			GetWorldTimerManager().SetTimer(BlockSpawnTimerHandle, this, &AConveyor::TrySpawnNextBlock, BlockSpawnInterval, true);
		}
	}
	else
	{
		MoveSpeed = 0.0f;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Conveyor : Power Off")));
		GetWorldTimerManager().ClearTimer(BlockSpawnTimerHandle);
	}
}

void AConveyor::SetBlockSpawnCount(const int32& TargetCount)
{
	NumBlocksToSpawn = TargetCount * 2;
}

void AConveyor::SetResetBlockSpawnCount()
{
	NumBlocksToSpawn = 0;
	BlocksSpawnedCount = 0;
}

