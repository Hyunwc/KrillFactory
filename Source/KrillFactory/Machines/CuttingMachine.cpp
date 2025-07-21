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

	if (!OverlappingBlock)
	{
		return;
	}

	// 오버랩된 블록타입 확인
	if (OverlappingBlock->BlockType == EBlockType::EBT_Full)
	{
		// 1. 기존 풀 블록의 위치와 회전 저장
		FVector OriginalLocation = OverlappingBlock->GetActorLocation();
		FRotator OriginalRotation = OverlappingBlock->GetActorRotation();

		// 2. 기존 풀 블록을 컨베이어 풀로 반납
		if (!FoundConveyor)
		{
			UE_LOG(LogTemp, Error, TEXT("CuttingMachine : Conveyor Not Found!"));
			return;
		}
		FoundConveyor->ReturnBlockToPool(OverlappingBlock);

		//const float QuarterCubeHalfYExtent = 100.0f * 0.2f / 2.0f;

		//float HalfFullY = 0.8f / 2.0f;
		//float HalfQuarterY = 0.2f / 2.0f;

		//TArray<FVector> Offsets;
		//// 첫 번짼
		//Offsets.Add(FVector(0.0f, -HalfFullY + HalfQuarterY, 0.0f));
		//Offsets.Add(FVector(0.0f, -HalfFullY + HalfQuarterY * 3, 0.0f));
		//Offsets.Add(FVector(0.0f, HalfFullY - HalfQuarterY * 3, 0.0f));
		//Offsets.Add(FVector(0.0f, HalfFullY - HalfQuarterY, 0.0f));

		TArray<float> Y_Offsets_cm;
		Y_Offsets_cm.Add(-30.f);
		Y_Offsets_cm.Add(-10.f);
		Y_Offsets_cm.Add(10.f);
		Y_Offsets_cm.Add(30.f);

		for (int32 i = 0; i < 4; i++)
		{
			AKrillBlock* NewQuaterBlock = FoundConveyor->GetBlockFromPool(EBlockType::EBT_Quarter);
			if (NewQuaterBlock)
			{
				FVector LocalOffset = FVector(0.0f, Y_Offsets_cm[i], 0.0f);
				FVector WorldOffset = OriginalRotation.RotateVector(LocalOffset);
				FVector NewBlockLocation = OriginalLocation + WorldOffset;

				FoundConveyor->AddBlockToConveyorAtWorldLocation(NewQuaterBlock, NewBlockLocation, OriginalRotation);
			}
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

