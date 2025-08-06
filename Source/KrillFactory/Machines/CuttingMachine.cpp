// Fill out your copyright notice in the Description page of Project Settings.


#include "Machines/CuttingMachine.h"
#include "Machines/Conveyor.h"
#include "Machines/MainPower.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Products/KrillBlock.h"
#include "Kismet/GameplayStatics.h"

ACuttingMachine::ACuttingMachine()
{
	PrimaryActorTick.bCanEverTick = false;

	BladeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainBladeMesh"));
	BladeMesh->SetupAttachment(Root);
	BladeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	CuttingZone = CreateDefaultSubobject<UBoxComponent>(TEXT("CuttingZone"));
	CuttingZone->SetupAttachment(Root);
	CuttingZone->SetBoxExtent(FVector(150.0f, 150.0f, 100.0f));
	CuttingZone->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	CuttingZone->SetGenerateOverlapEvents(true);

	FoundConveyor = nullptr;

	EightSpawnInterval = 0.1f;

	MainPower = nullptr;
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
			UE_LOG(LogTemp, Warning, TEXT("CuttingMachine : 컨베이어 캐스팅에 실패했습니다!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CuttingMachine : 컨베이어가 없어요!!"));
	}

	TArray<AActor*> FoundPowerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMainPower::StaticClass(), FoundPowerActors);
	if (FoundPowerActors.Num() > 0)
	{
		MainPower = Cast<AMainPower>(FoundPowerActors[0]);
		if (IsValid(MainPower))
		{
			MainPower->OnPowerStateChanged.AddDynamic(this, &ACuttingMachine::OnMainPowerStateChanged);
			UE_LOG(LogTemp, Log, TEXT("CuttingMachine : MainPower Delegate Binding Successed"));
		}
	}

	CuttingZone->OnComponentBeginOverlap.AddDynamic(this, &ACuttingMachine::OnCuttingZoneOverlapBegin);
	CuttingZone->OnComponentEndOverlap.AddDynamic(this, &ACuttingMachine::OnCuttingZoneOverlapEnd);
}

void ACuttingMachine::OnCuttingZoneOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void ACuttingMachine::OnCuttingZoneOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AKrillBlock* OverlappingBlock = Cast<AKrillBlock>(OtherActor);

	if (!OverlappingBlock)
	{
		return;
	}

	// 오버랩된 블록타입 확인
	if (OverlappingBlock->BlockType == EBlockType::EBT_Full)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Cutt : Overlap!!")));

		// 1. 기존 풀 블록의 위치와 회전 저장
		FVector OriginalLocation = OverlappingBlock->GetActorLocation();
		//FRotator OriginalRotation = OverlappingBlock->GetActorRotation();

		// 2. 기존 풀 블록을 컨베이어 풀로 반납
		if (!FoundConveyor)
		{
			UE_LOG(LogTemp, Error, TEXT("CuttingMachine : Conveyor Not Found!"));
			return;
		}
		FoundConveyor->ReturnBlockToPool(OverlappingBlock);

		AKrillBlock* NewQuaterBlock = FoundConveyor->GetBlockFromPool(EBlockType::EBT_Quarter);
		if (NewQuaterBlock)
		{
			// 분할된 블럭들 위치 지정해서 스폰시킴
			FoundConveyor->AddBlockToConveyor(NewQuaterBlock, OriginalLocation/*, OriginalRotation*/);
		}
	}
	else if (OverlappingBlock->BlockType == EBlockType::EBT_Quarter)
	{
		// 1. 기존 풀 블록의 위치와 회전 저장
		FVector OriginalLocation = OverlappingBlock->GetActorLocation();
		//FRotator OriginalRotation = OverlappingBlock->GetActorRotation();

		// 2. 기존 풀 블록을 컨베이어 풀로 반납
		if (!FoundConveyor)
		{
			UE_LOG(LogTemp, Error, TEXT("CuttingMachine : Conveyor Not Found!"));
			return;
		}
		FoundConveyor->ReturnBlockToPool(OverlappingBlock);
		
		GetWorldTimerManager().ClearTimer(EighthTimer);
		PendingSpawnEighthBlocks.Empty();

		for (int32 i = 0; i < 4; i++)
		{
			AKrillBlock* NewEighthBlock = FoundConveyor->GetBlockFromPool(EBlockType::EBT_Eighth);
			if (NewEighthBlock)
			{
				PendingSpawnEighthBlocks.Add(NewEighthBlock);
				//GetWorldTimerManager().SetTimer(EighthTimer, this, )
				//// 분할된 블럭들 위치 지정해서 스폰시킴
				//FoundConveyor->AddBlockToConveyor(NewEighthBlock, OriginalLocation/*, OriginalRotation*/);
			}
		}
		
		if (PendingSpawnEighthBlocks.Num() > 0)
		{
			GetWorldTimerManager().SetTimer(EighthTimer, this, &ACuttingMachine::SpawnNextEighthBlock, EightSpawnInterval, true, 0.0f);
		}
	}

	//GetWorldTimerManager().ClearTimer(EighthTimer);
}

void ACuttingMachine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACuttingMachine::OnMainPowerStateChanged(bool bPowerOn)
{
	// 파워 토글 로직 
	bIsPowerOn = bPowerOn;

	// On시 톱날 회전(아직 없지만) 시작 + 이벤트 활성화
	// off시 반대로
	if (bIsPowerOn)
	{
		// TODO : 여기에 톱날 회전 애니메이션 또는 함수를 추가합니다.
		CuttingZone->SetGenerateOverlapEvents(true);
	}
	else
	{
		CuttingZone->SetGenerateOverlapEvents(false);
	}
	
}

void ACuttingMachine::SpawnNextEighthBlock()
{
	if (PendingSpawnEighthBlocks.Num() == 0)
	{
		GetWorldTimerManager().ClearTimer(EighthTimer);
		return; 
	}

	AKrillBlock* EighthToSpawn = PendingSpawnEighthBlocks[0];
	PendingSpawnEighthBlocks.RemoveAt(0);

	FoundConveyor->AddBlockToConveyor(EighthToSpawn, CuttingZoneExitLocation/*, OriginalRotation*/);
}


