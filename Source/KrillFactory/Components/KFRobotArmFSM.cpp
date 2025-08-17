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
}

