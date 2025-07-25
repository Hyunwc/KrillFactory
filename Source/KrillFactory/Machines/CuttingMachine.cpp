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
	//bIncuttingCooldown = false;
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

	CuttingZone->OnComponentBeginOverlap.AddDynamic(this, &ACuttingMachine::OnCuttingZoneOverlapBegin);
	CuttingZone->OnComponentEndOverlap.AddDynamic(this, &ACuttingMachine::OnCuttingZoneOverlapEnd);

	//SpawnBlades();
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

		AKrillBlock* NewQuaterBlock = FoundConveyor->GetBlockFromPool(EBlockType::EBT_Eighth);
		if (NewQuaterBlock)
		{
			// 분할된 블럭들 위치 지정해서 스폰시킴
			FoundConveyor->AddBlockToConveyor(NewQuaterBlock, OriginalLocation/*, OriginalRotation*/);
		}
	}
}

void ACuttingMachine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


