// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/KFRobotArmFSM.h"

UKFRobotArmFSM::UKFRobotArmFSM()
{
	PrimaryComponentTick.bCanEverTick = true;

	CurrentState = ERobotArmState::RS_Idle; // 처음은 대기 상태
}

void UKFRobotArmFSM::BeginPlay()
{
	Super::BeginPlay();
	
}

void UKFRobotArmFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UKFRobotArmFSM::IdleState()
{

}

void UKFRobotArmFSM::CarryState()
{
	GetWorld()->GetTimerManager().SetTimer(StateTimerHandle, this, &UKFRobotArmFSM::FinishedCarry, 5.0f, false);
}

void UKFRobotArmFSM::ReturnState()
{
	GetWorld()->GetTimerManager().SetTimer(StateTimerHandle, this, &UKFRobotArmFSM::FinishedReturn, 5.0f, false);
}

void UKFRobotArmFSM::FinishedIdle()
{
}

void UKFRobotArmFSM::FinishedCarry()
{
	GetWorld()->GetTimerManager().ClearTimer(StateTimerHandle);
	ChangeState(ERobotArmState::RS_Return);
}

void UKFRobotArmFSM::FinishedReturn()
{
	GetWorld()->GetTimerManager().ClearTimer(StateTimerHandle);
	ChangeState(ERobotArmState::RS_Idle);
}

void UKFRobotArmFSM::HandleBoxComplete()
{
	ChangeState(ERobotArmState::RS_Carry);
}

void UKFRobotArmFSM::ChangeState(ERobotArmState NewState)
{
	CurrentState = NewState;

	// 실행창에 상태 메시지 출력
	FString LogMsg = UEnum::GetValueAsString(CurrentState);
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Cyan, LogMsg);

	switch (CurrentState)
	{
	case ERobotArmState::RS_Idle:
		IdleState();
		break;
	case ERobotArmState::RS_Carry:
		CarryState();
		break;
	case ERobotArmState::RS_Return:
		ReturnState();
		break;
	default:
		break;
	}
}

