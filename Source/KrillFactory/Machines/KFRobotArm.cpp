// Fill out your copyright notice in the Description page of Project Settings.


#include "Machines/KFRobotArm.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/KFRobotArmFSM.h"
#include "BoxingMachine.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Objects/KFPallet.h"
#include "ControlRigComponent.h"

AKFRobotArm::AKFRobotArm()
{
	PrimaryActorTick.bCanEverTick = true;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(Root);

	FSM = CreateDefaultSubobject<UKFRobotArmFSM>(TEXT("FSM"));

	ControlRig = CreateDefaultSubobject<UControlRigComponent>(TEXT("ControlRig"));
	ControlRig->SetupAttachment(SkeletalMesh);

	// 박스 메시 컴포넌트 생성 및 집게 소켓에 부착
	BoxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxMesh"));
	BoxMesh->SetupAttachment(SkeletalMesh, FName("GripSocket"));
	BoxMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxMesh->SetHiddenInGame(true);
	//BoxMesh->SetVisibility(false);

	bIsGripping = false;
	TargetTransform = FTransform::Identity;
}

void AKFRobotArm::BeginPlay()
{
	Super::BeginPlay();

	BoxingMachine = Cast<ABoxingMachine>(UGameplayStatics::GetActorOfClass(GetWorld(), ABoxingMachine::StaticClass()));
	if (BoxingMachine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("RobotArm : 박싱머신 유효!!")));
	}
	Pallet = Cast<AKFPallet>(UGameplayStatics::GetActorOfClass(GetWorld(), AKFPallet::StaticClass()));
	if (Pallet)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("RobotArm : 파레트 유효!!")));
	}
	//TArray<AActor*> FoundActor;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABoxingMachine::StaticClass(), FoundActor);
	//if (FoundActor.Num() > 0)
	//{
	//	BoxingMachine = Cast<ABoxingMachine>(FoundActor[0]);
	//}

	if (FSM && BoxingMachine)
	{
		BoxingMachine->OnBoxComplete.AddDynamic(FSM, &UKFRobotArmFSM::HandleBoxComplete);
	}
}

void AKFRobotArm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (FSM->CurrentState)
	{
	case ERobotArmState::RS_Idle:
		break;
	case ERobotArmState::RS_Carry:
		// 박스 집어서 운반하는 상태
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan,
		//	FString::Printf(TEXT("RobotArm : 운반 상태로 들어왔다!!")));
		if (bIsGripping == false)
		{
			// Pallet에서 다음 목표 위치를 얻어 Transform에 저장
			if (Pallet)
			{
				TargetTransform = Pallet->GetNextTargetTransform();
			}

			MoveArmToTarget(TargetTransform);
			
			BoxMesh->SetHiddenInGame(false);
			bIsGripping = true;
		}

		//// 컨트롤릭에 목표 트랜스폼을 전달하여 로봇암을 움직임
		//if (ControlRig)
		//{
		//	// 컨트롤릭 블루프린트 내의 InTargetTransform 변수 업데이트
		//	ControlRig->SetControlTransform(FName("joint2_ctrl"), TargetTransform);
		//}

		// 목표 위치 도달을 감지하여 다음 단계로 전환
		FVector CurrentLocation = SkeletalMesh->GetSocketLocation(FName("GripSocket"));
		if (FVector::Distance(CurrentLocation, TargetTransform.GetLocation()) < 10.f)
		{
			// 목표 도달
			if (Pallet)
			{
				Pallet->ShowBoxAtLocation();
			}
			BoxMesh->SetHiddenInGame(true);
			bIsGripping = false;

			FSM->ChangeState(ERobotArmState::RS_Return);
		}
		break;

	case ERobotArmState::RS_Return:
		// 박스를 내려놓고 시작 위치로

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan,
		//	FString::Printf(TEXT("RobotArm : 리턴 상태로 들어왔다!!")));
		// 시작 위치 트랜스폼을 설정(직접 설정해야함 일단 임의로)
		FTransform ReturnTransform = FTransform::Identity;

		MoveArmToTarget(ReturnTransform);
		// 컨트롤릭에 시작 위치 트랜스폼을 전달하여 돌아간다.
		//if (ControlRig)
		//{
		//	ControlRig->SetControlTransform(FName("joint2_ctrl"), ReturnTransform);
		//}

		FVector CurrentReturnLocation = SkeletalMesh->GetSocketLocation(FName("GripSocket"));
		if (FVector::Distance(CurrentReturnLocation, ReturnTransform.GetLocation()) < 10.f)
		{
			FSM->ChangeState(ERobotArmState::RS_Idle);
		}
	}
	
}


