// Fill out your copyright notice in the Description page of Project Settings.

#include "Managers/KFPoolManager.h"
#include "Kismet/GameplayStatics.h"
#include "Machines/Conveyor.h"

AKFPoolManager::AKFPoolManager()
{
	PrimaryActorTick.bCanEverTick = false;

	Size = 30;

	PoolSize.Add(EBlockType::EBT_Full, 30);
	PoolSize.Add(EBlockType::EBT_Quarter, 50);
	PoolSize.Add(EBlockType::EBT_Eighth, 100);
	PoolSize.Add(EBlockType::EBT_Pack, 100);
}

void AKFPoolManager::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AConveyor::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		MainConveyor = Cast<AConveyor>(FoundActors[0]);
	}

	InitPooling();
}

void AKFPoolManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKFPoolManager::InitPooling()
{
	for (const auto& Elem : PoolSize)
	{
		EBlockType Type = Elem.Key;
		int32 Count = Elem.Value;

		if (!BlockClasses.Contains(Type))
		{
			continue;
		}

		FPoolMapWrapper PoolMapWrapper;

		for (int32 i = 0; i < Count; i++)
		{
			AKrillBlock* NewBlock = GetWorld()->SpawnActor<AKrillBlock>(BlockClasses[Type],
				FVector::ZeroVector, FRotator::ZeroRotator);
			NewBlock->SetActorHiddenInGame(true);
			NewBlock->SetActorEnableCollision(false);
			NewBlock->SetActorTickEnabled(false);
			NewBlock->SetBlockType(Type); // 풀에 들어갈 때 해당 타입으로 설정
			PoolMapWrapper.PoolMapArray.Add(NewBlock);
		}

		PoolMap.Add(Type, PoolMapWrapper);
	}
}

void AKFPoolManager::SpawnPool()
{
}

AKrillBlock* AKFPoolManager::GetPooling(EBlockType Type)
{
	if (FPoolMapWrapper* Wrapper = PoolMap.Find(Type))
	{
		if (!Wrapper->PoolMapArray.IsEmpty())
		{
			AKrillBlock* Block = Wrapper->PoolMapArray.Last();
			Block->SetActorHiddenInGame(false);
			Block->SetActorEnableCollision(true);
			Block->SetActorTickEnabled(true);
			Wrapper->PoolMapArray.Pop();
			return Block;
		}
	}
	return nullptr;
}

void AKFPoolManager::ReturnPooling(AKrillBlock* Block, EBlockType Type)
{
	if (!Block)
	{
		return;
	}

	if (FPoolMapWrapper* Wrapper = PoolMap.Find(Type))
	{
		Block->SetActorHiddenInGame(true);
		Block->SetActorEnableCollision(false);
		Block->SetActorTickEnabled(false);
		Wrapper->PoolMapArray.Add(Block);
	}
}

void AKFPoolManager::AddBlockToConveyor(AKrillBlock* Block, const FVector& WorldLocation)
{
	MainConveyor->AddBlock(Block, WorldLocation);
}

