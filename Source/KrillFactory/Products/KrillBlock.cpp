// Fill out your copyright notice in the Description page of Project Settings.

#include "Products/KrillBlock.h"
#include "Components/StaticMeshComponent.h"

AKrillBlock::AKrillBlock()
{
	PrimaryActorTick.bCanEverTick = true;
	BlockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Block"));
	SetRootComponent(BlockMesh);

	BlockType = EBlockType::EBT_Full; // ó���� Full

	// StaticMesh �Ҵ� �� ũ�� ����
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Shapes/Shape_Cube.Shape_Cube'"));
	if (MeshRef.Object != nullptr)
	{
		BlockMesh->SetStaticMesh(MeshRef.Object);

		BlockMesh->SetWorldScale3D(FVector(0.75f, 1.0f, 0.25f));
	}

	BlockMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
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
	UpdateMeshForBlockType(); // Ÿ�� ���� �� ������Ʈ
}

void AKrillBlock::UpdateMeshForBlockType()
{
	if (!BlockMesh)
	{
		return;
	}
}



