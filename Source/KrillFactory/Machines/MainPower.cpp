// Fill out your copyright notice in the Description page of Project Settings.


#include "Machines/MainPower.h"

AMainPower::AMainPower()
{
	//PrimaryActorTick.bCanEverTick = true;
	bIsPowerOn = false;
}

void AMainPower::TogglePower()
{
	bIsPowerOn = !bIsPowerOn;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, 
		FString::Printf(TEXT("MainPower : Power State Changed to %s"), bIsPowerOn ? TEXT("ON") : TEXT("OFF")));

	// 전원 상태가 변경되었음을 델리게이트를 통해 알린다.
	// 델리게이트에 등록된 모든 함수들이 호출된다.
	OnPowerStateChanged.Broadcast(bIsPowerOn);
}


