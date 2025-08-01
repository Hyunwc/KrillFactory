// Fill out your copyright notice in the Description page of Project Settings.


#include "Machines/BoxingMachine.h"
#include "Machines/Conveyor.h"
#include "Machines/MainPower.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Products/KrillBlock.h"

ABoxingMachine::ABoxingMachine()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	BoxingZone = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxingZone"));
	BoxingZone->SetupAttachment(Root);
	//BoxingZone->SetBoxExtent(FVector(150.f, 150.f, 150.f));
	BoxingZone->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	BoxingZone->SetGenerateOverlapEvents(true);

	PackCount = 0;
	bBoxingComplete = false;
	
	bIsPowerOn = false;
	FoundConveyor = nullptr;
	MainPower = nullptr;
}

void ABoxingMachine::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AConveyor::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		FoundConveyor = Cast<AConveyor>(FoundActors[0]);
		if (!FoundConveyor)
		{
			UE_LOG(LogTemp, Warning, TEXT("BoxingMachine : 컨베이어 캐스팅 실패!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BoxingMachine : 컨베이어가 없어요!!"));
	}

	TArray<AActor*> FoundPowerActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMainPower::StaticClass(), FoundPowerActors);
	if (FoundPowerActors.Num() > 0)
	{
		MainPower = Cast<AMainPower>(FoundPowerActors[0]);
		if (IsValid(MainPower))
		{
			MainPower->OnPowerStateChanged.AddDynamic(this, &ABoxingMachine::OnMainPowerStateChanged);
			UE_LOG(LogTemp, Log, TEXT("BoxingMachine : MainPower Delegate Binding Successed"));
		}
	}

	BoxingZone->OnComponentBeginOverlap.AddDynamic(this, &ABoxingMachine::OnBoxingZoneOverlapBegin);
	BoxingZone->OnComponentEndOverlap.AddDynamic(this, &ABoxingMachine::OnBoxingZoneOverlapEnd);
}

void ABoxingMachine::OnBoxingZoneOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void ABoxingMachine::OnBoxingZoneOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AKrillBlock* OverlappingBlock = Cast<AKrillBlock>(OtherActor);

	if (OverlappingBlock == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("box : null!!")));
		return;
	}

	if (OverlappingBlock->BlockType == EBlockType::EBT_Pack)
	{
		// 기존 풀 블록 반납
		if (!FoundConveyor)
		{
			return;
		}

		FoundConveyor->ReturnBlockToPool(OverlappingBlock);

		// 현재 Pack BP가 2조각이 양옆으로 붙어있는 상태이기 때문
		PackCount += 2;

		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow, FString::Printf(TEXT("box : %d 개 완료"), PackCount));

	}
}

void ABoxingMachine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABoxingMachine::OnMainPowerStateChanged(bool bPowerOn)
{
	bIsPowerOn = bPowerOn;

	if (bIsPowerOn)
	{
		BoxingZone->SetGenerateOverlapEvents(true);
	}
	else
	{
		BoxingZone->SetGenerateOverlapEvents(false);
	}
}

