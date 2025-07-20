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

	// ����̰�, ���� ���� ��ٿ� ���� �ƴϸ�, Full Block�� ���
	if (OverlappingBlock && !bIncuttingCooldown && OverlappingBlock->BlockType == EBlockType::EBT_Full)
	{
		// ���� ���� ����
		if (FoundConveyor)
		{
			// 1. ���� Full block�� Ǯ�� ��ȯ
			FoundConveyor->ReturnBlockToPool(OverlappingBlock);

			// 2. 4��е� ��ϵ��� Ǯ���� ������ �� ��ġ�� �ٽ� ����
			const FVector CutLocation = OverlappingBlock->GetActorLocation();
			const FRotator CutRotation = OverlappingBlock->GetActorRotation();

			// 4�� ����� �߸��� ��ġ�� ������ ���̵��� ������ ���
			// �� �������� ����� 1/4 ũ��� �¹����� �Ѵ�.

			float QuarterBlockHalfSize = 25.0f;
			float Gap = 5.0f; // ����� ���� ����

			// ����� ���� Y�� �������� ������
			FVector RightDirection = CutRotation.RotateVector(FVector::RightVector);

			for (int32 i = 0; i < 4; i++)
			{
				AKrillBlock* QuaterBlock = FoundConveyor->GetBlockFromPool(EBlockType::EBT_Quarter);
				if (QuaterBlock)
				{
					// 4���� ���� ����� Y������ ������ ��ġ
					FVector SpawnOffset = RightDirection * ((i - 1.5f) * (QuarterBlockHalfSize * 2.0f + Gap));
					FVector NewSpawnLocation = CutLocation + SpawnOffset;

					FoundConveyor->AddBlockToConveyorAtWorldLocation(QuaterBlock, NewSpawnLocation, CutRotation);
				}
			}

			// ���� ��ٿ� ����(���� ����� �ٽ� �ٷ� �߸��� �� ����)
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

	// ù��° Į��
	BladeInstance.Add(BladeMesh);

	// ������ Į��. Į������ 1�̸� ����x
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

