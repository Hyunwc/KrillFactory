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

	// StaticMesh �Ҵ� �� ũ�� ����
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (MeshRef.Object != nullptr)
	{
		FullBlockMesh = MeshRef.Object;
		//FullBlockMesh->SetWorldScale3D(FVector(0.75f, 1.0f, 0.25f));
		QuarterBlockMesh = MeshRef.Object;
		EightBlockMesh = MeshRef.Object;
	}

	BlockType = EBlockType::EBT_Full; // ó���� Full
	SetBlockType(EBlockType::EBT_Full);
}

void AKrillBlock::BeginPlay()
{
	Super::BeginPlay();
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
		ScaleToUse = FVector(1.0f, 1.0f, 1.0f); // �⺻(�ʿ信 ���� ����)
		break;
	case EBlockType::EBT_Quarter:
		MeshToUse = QuarterBlockMesh;
		ScaleToUse = FVector(0.5f, 0.5f, 0.5f); // 1/4
		break;
	case EBlockType::EBT_Eighth:
		MeshToUse = EightBlockMesh;
		ScaleToUse = FVector(0.25f, 0.25f, 0.25f); // 1/4
		break;
	default:
		MeshToUse = FullBlockMesh;
		ScaleToUse = FVector(1.0f, 1.0f, 1.0f); 
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
	//UpdateMeshForBlockType(); // Ÿ�� ���� �� ������Ʈ
}

//void AKrillBlock::UpdateMeshForBlockType()
//{
//	if (!BlockMesh)
//	{
//		return;
//	}
//}



