// Fill out your copyright notice in the Description page of Project Settings.

#include "Machines/PackagingMachine.h"
#include "Machines/Conveyor.h"
#include "Machines/MainPower.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Products/KrillBlock.h"
#include "Managers/KFPoolManager.h"

APackagingMachine::APackagingMachine()
{
	PrimaryActorTick.bCanEverTick = false;

	PackagingZone = CreateDefaultSubobject<UBoxComponent>(TEXT("Pack"));
	PackagingZone->SetupAttachment(Root);
	PackagingZone->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	PackagingZone->SetGenerateOverlapEvents(true);

	FoundConveyor = nullptr;
	MainPower = nullptr;
}

void APackagingMachine::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AConveyor::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		FoundConveyor = Cast<AConveyor>(FoundActors[0]);
		if (!FoundConveyor)
		{
			UE_LOG(LogTemp, Warning, TEXT("PackagingMachine : 컨베이어 캐스팅에 실패!!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PackagingMachine : 컨베이어가 없어요!!"));
	}

	TArray<AActor*> FoundPowerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMainPower::StaticClass(), FoundPowerActors);
	if (FoundPowerActors.Num() > 0)
	{
		MainPower = Cast<AMainPower>(FoundPowerActors[0]);
		if (IsValid(MainPower))
		{
			MainPower->OnPowerStateChanged.AddDynamic(this, &APackagingMachine::OnMainPowerStateChanged);
			UE_LOG(LogTemp, Log, TEXT("PackagingMachine : MainPower Delegate Binding Successed"));
		}
	}

	PackagingZone->OnComponentBeginOverlap.AddDynamic(this, &APackagingMachine::OnPackagingZoneOverlapBegin);
	PackagingZone->OnComponentEndOverlap.AddDynamic(this, &APackagingMachine::OnPackagingZoneOverlapEnd);
}

void APackagingMachine::OnPackagingZoneOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void APackagingMachine::OnPackagingZoneOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AKrillBlock* OverlappingBlock = Cast<AKrillBlock>(OtherActor);

	if (OverlappingBlock == nullptr)
	{
		return;
	}

	if (OverlappingBlock->BlockType == EBlockType::EBT_Eighth)
	{
		// 1. 기존 풀 블록의 위치와 회전 저장
		FVector OriginalLocation = OverlappingBlock->GetActorLocation();

		// 2. 기존 풀 블록을 컨베이어 풀로 반납
		if (!FoundConveyor)
		{
			return;
		}

		//OverlappingBlock->SetActorHiddenInGame(true);
		PoolManager->ReturnPooling(OverlappingBlock, OverlappingBlock->BlockType);

		AKrillBlock* NewPackBlock = PoolManager->GetPooling(EBlockType::EBT_Pack);
		if (NewPackBlock)
		{
			// 분할된 블럭들 위치 지정해서 스폰시킴
			PoolManager->AddBlockToConveyor(NewPackBlock, OriginalLocation);
		}
	}
}

void APackagingMachine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APackagingMachine::OnMainPowerStateChanged(bool bPowerOn)
{
	bIsPowerOn = bPowerOn;

	if (bIsPowerOn)
	{
		PackagingZone->SetGenerateOverlapEvents(true);
	}
	else
	{
		PackagingZone->SetGenerateOverlapEvents(false);
	}
}

