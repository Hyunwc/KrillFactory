// Fill out your copyright notice in the Description page of Project Settings.

#include "Products/KrillBlock.h"
#include "Components/StaticMeshComponent.h"

AKrillBlock::AKrillBlock()
{
	PrimaryActorTick.bCanEverTick = false;

	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlockMesh"));
	SetRootComponent(BlockMesh);

	BlockMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	BlockMesh->SetGenerateOverlapEvents(true);

	// StaticMesh 할당 및 크기 설정
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (MeshRef.Object != nullptr)
	{
		FullBlockMesh = MeshRef.Object;
		QuarterBlockMesh = MeshRef.Object;
		EightBlockMesh = MeshRef.Object;
	}

	// TODO : 머티리얼 추가

	BlockType = EBlockType::EBT_Full; // 처음은 Full
}

void AKrillBlock::BeginPlay()
{
	Super::BeginPlay();

	SetBlockType(BlockType);
}

void AKrillBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AKrillBlock::SetBlockType(EBlockType NewType)
{
	BlockType = NewType;

	UStaticMesh* MeshToUse = nullptr;
	FVector ScaleToUse = FVector::OneVector;

	switch (BlockType)
	{
	case EBlockType::EBT_Full:
		MeshToUse = FullBlockMesh;
		ScaleToUse = FVector(0.4f, 0.8f, 0.2f); // 기본(필요에 따라 변경)
		break;
	case EBlockType::EBT_Quarter:
		MeshToUse = QuarterBlockMesh;
		ScaleToUse = FVector(0.4f, 0.2f, 0.2f); 
		break;
	case EBlockType::EBT_Eighth:
		MeshToUse = EightBlockMesh;
		ScaleToUse = FVector(0.2f, 0.2f, 0.2f);
		break;
	default:
		MeshToUse = FullBlockMesh;
		ScaleToUse = FVector(0.4f, 0.8f, 0.2f);
		break;
	}

	if (BlockMesh && MeshToUse)
	{
		BlockMesh->SetStaticMesh(MeshToUse);
		BlockMesh->SetRelativeScale3D(ScaleToUse);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load cube!!"));
	}
}




