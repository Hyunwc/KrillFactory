// Fill out your copyright notice in the Description page of Project Settings.


#include "Machines/CuttingMachine.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Products/KrillBlock.h"
#include "Machines/Conveyor.h"
#include "Kismet/GameplayStatics.h"

ACuttingMachine::ACuttingMachine()
{
	PrimaryActorTick.bCanEverTick = false;

	BladeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainBladeMesh"));
	RootComponent = BladeMesh;

	CuttingZone = CreateDefaultSubobject<UBoxComponent>(TEXT("CuttingZone"));
	CuttingZone->SetupAttachment(RootComponent);
	CuttingZone->SetBoxExtent(FVector(150.0f, 150.0f, 100.0f));
	CuttingZone->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CuttingZone->SetGenerateOverlapEvents(true);

	NumberOfBlades = 3;
	FoundConveyor = nullptr;
	bIncuttingCooldown = false;
}

void ACuttingMachine::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AConveyor::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		FoundConveyor = Cast<AConveyor>(FoundActors[0]);
		if (!FoundConveyor)
		{
			UE_LOG(LogTemp, Warning, TEXT("CuttingMachine : Found Conveyor type cast Faild!!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CuttingMachine : Not Found Conveyor!!"));
	}

	CuttingZone->OnComponentBeginOverlap.AddDynamic(this, &ACuttingMachine::OnCuttingZoneOverlapBegin);
	CuttingZone->OnComponentEndOverlap.AddDynamic(this, &ACuttingMachine::OnCuttingZoneOverlapEnd);

	SpawnBlades();
}

void ACuttingMachine::OnCuttingZoneOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AKrillBlock* OverlappingBlock = Cast<AKrillBlock>(OtherActor);

	// 블록이고, 아직 컷팅 쿨다운 중이 아니며, Full Block일 경우
	if (OverlappingBlock && !bIncuttingCooldown && OverlappingBlock->BlockType == EBlockType::EBT_Full)
	{
		// 컷팅 로직 시작
		if (FoundConveyor)
		{
			// 1. 기존 Full block을 풀로 반환
			FoundConveyor->ReturnBlockToPool(OverlappingBlock);

			// 2. 4등분된 블록들을 풀에서 가져와 그 위치에 다시 투입
			const FVector CutLocation = OverlappingBlock->GetActorLocation();
			const FRotator CutRotation = OverlappingBlock->GetActorRotation();

			// 4개 블록이 잘리는 위치에 나란히 놓이도록 오프셋 계산
			// 이 오프셋은 블록의 1/4 크기와 맞물려야 한다.

			float QuarterBlockHalfSize = 25.0f;
			float Gap = 5.0f; // 블록의 작은 간격

			// 블록의 로컬 Y축 방향으로 오프셋
			FVector RightDirection = CutRotation.RotateVector(FVector::RightVector);

			for (int32 i = 0; i < 4; i++)
			{
				AKrillBlock* QuaterBlock = FoundConveyor->GetBlockFromPool(EBlockType::EBT_Quarter);
				if (QuaterBlock)
				{
					// 4개의 쿼터 블록을 Y축으로 나란히 배치
					FVector SpawnOffset = RightDirection * ((i - 1.5f) * (QuarterBlockHalfSize * 2.0f + Gap));
					FVector NewSpawnLocation = CutLocation + SpawnOffset;

					FoundConveyor->AddBlockToConveyorAtWorldLocation(QuaterBlock, NewSpawnLocation, CutRotation);
				}
			}

			// 컷팅 쿨다운 시작(동일 블록이 다시 바로 잘리는 것 방지)
			bIncuttingCooldown = true;
			GetWorldTimerManager().SetTimer(CuttingCooldownTimerHandle, this, &ACuttingMachine::ClearCuttingCooldown, 0.5f, false);
		}
	}
}

void ACuttingMachine::OnCuttingZoneOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void ACuttingMachine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACuttingMachine::ClearCuttingCooldown()
{
	bIncuttingCooldown = false;
}

void ACuttingMachine::SpawnBlades()
{
	if (!BladeMesh->GetStaticMesh())
	{
		return;
	}

	// 첫번째 칼날
	BladeInstance.Add(BladeMesh);

	// 나머지 칼날. 칼갯수가 1이면 생성x
	for (int32 i = 1; i < NumberOfBlades; i++)
	{
		UStaticMeshComponent* NewBlade = NewObject<UStaticMeshComponent>(this, FName(*FString::Printf(TEXT("BladeMesh%d"), i)));
		if (NewBlade)
		{
			NewBlade->SetStaticMesh(BladeMesh->GetStaticMesh());
			NewBlade->SetupAttachment(RootComponent);
			NewBlade->SetRelativeLocation(FVector(0.0f, (float)i * 100.f, 0.0f));
			NewBlade->SetRelativeScale3D(BladeMesh->GetRelativeScale3D());
			NewBlade->RegisterComponent();

			BladeInstance.Add(NewBlade);
		}
	}
}

