// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/KFPallet.h"
//#include "Scene/StaticMesh.h"

AKFPallet::AKFPallet()
{
	PrimaryActorTick.bCanEverTick = false;

	// 기본값 설정
	MaxBoxCount = 50;
	BoxSpacing = FVector(50.f, 50.f, 50.f); // 일단 테스트 간격
	CurrentBoxIndex = 0;
}

void AKFPallet::BeginPlay()
{
	Super::BeginPlay();
}

void AKFPallet::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// 에디터에서 컴포넌트 미리보기
	if (GetWorld() && GetWorld()->IsGameWorld() == false)
	{
		// 기존 컴포넌트 제거
		for (UStaticMeshComponent* Box : BoxInstance)
		{
			if (Box)
			{
				Box->DestroyComponent();
			}
		}

		BoxInstance.Empty();
		BoxSpawnTransforms.Empty();

		// 박스 위치 계산
		CalculateBoxTransforms();

		// 박스 컴포넌트 생성 및 위치 설정
		CreateBoxComponents();
	}
}

void AKFPallet::Tick(float DeltaTime)
{
}

FTransform AKFPallet::GetNextTargetTransform()
{
	if (CurrentBoxIndex < MaxBoxCount)
	{
		return BoxSpawnTransforms[CurrentBoxIndex++];
	}
	// 인덱스 초과 시 기본값 반환
	return FTransform::Identity;
}

void AKFPallet::ShowBoxAtLocation()
{
	if (CurrentBoxIndex > 0 && CurrentBoxIndex <= MaxBoxCount)
	{
		UStaticMeshComponent* Box = BoxInstance[CurrentBoxIndex - 1];
		if (Box)
		{
			Box->SetHiddenInGame(false);
		}
	}
}

void AKFPallet::CalculateBoxTransforms()
{
	int32 BoxesPerFloor = 5;
	int32 NumbersOfFloors = 10;
	float BoxHeight = 25.f; // 박스 높이

	for (int32 i = 0; i < MaxBoxCount; i++)
	{
		int32 XIndex = i % BoxesPerFloor;
		int32 ZIndex = i / BoxesPerFloor;

		float X = XIndex * BoxSpacing.X;
		float Y = 0.0f;
		float Z = ZIndex * BoxHeight;

		// FTransform으로 변환 후 배열에 저장
		FTransform BoxTransform(FVector(X, Y, Z));
		BoxSpawnTransforms.Add(BoxTransform);
	}
}

void AKFPallet::CreateBoxComponents()
{
	for (const FTransform& BoxTransform : BoxSpawnTransforms)
	{
		// 스태틱 메시 컴포넌트 동적 생성
		UStaticMeshComponent* NewBox = NewObject<UStaticMeshComponent>(this);
		if (NewBox)
		{
			NewBox->RegisterComponent();
			NewBox->SetStaticMesh(BoxMesh);
			NewBox->SetWorldTransform(BoxTransform);
			NewBox->SetHiddenInGame(true);
			NewBox->AttachToComponent(Root, FAttachmentTransformRules::KeepWorldTransform);
			BoxInstance.Add(NewBox);
		}
	}
}
