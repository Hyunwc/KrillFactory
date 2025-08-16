// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KFRobotArmFSM.generated.h"

UENUM(BlueprintType)
enum class ERobotArmState : uint8
{
	RS_Idle UMETA(DisplayName = "Idle"),
	RS_Carry UMETA(DisplayName = "Carry"),
	RS_Return UMETA(DisplayName = "Return"),

	RS_Max UMETA(Hidden) // 센티널 값
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class KRILLFACTORY_API UKFRobotArmFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	UKFRobotArmFSM();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM")
	ERobotArmState CurrentState; // 로봇암의 상태	

	FTimerHandle StateTimerHandle;

	void IdleState(); // 대기 상태

	void CarryState(); // 운반 상태

	void ReturnState(); // 복귀 상태

	void FinishedIdle();
	void FinishedCarry();
	void FinishedReturn();

	UFUNCTION()
	void HandleBoxComplete(); 

	UFUNCTION(BlueprintCallable)
	void ChangeState(ERobotArmState NewState);
		
};
