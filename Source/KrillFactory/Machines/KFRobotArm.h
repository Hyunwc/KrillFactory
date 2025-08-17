// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Machines/KFMachineBase.h"
#include "KFRobotArm.generated.h"

class USkeletalMeshComponent;
class UStaticMeshComponent;
class ABoxingMachine;
class AKFPallet;
class UControlRigComponent;
/**
 * 
 */
UCLASS()
class KRILLFACTORY_API AKFRobotArm : public AKFMachineBase
{
	GENERATED_BODY()

public:
	AKFRobotArm();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM")
	class UKFRobotArmFSM* FSM;

	// 박스머신 참조
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ABoxingMachine* BoxingMachine; 

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AKFPallet* Pallet;

	// 로봇암이 이동해야할 위치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
	FTransform TargetTransform;

	// 물건으르 쥐고 있는지?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM")
	bool bIsGripping;

	// 집게에 부착될 박스 메시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMeshComponent* BoxMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM")
	UControlRigComponent* ControlRig;

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "RobotArm")
	void MoveArmToTarget(const FTransform& Target);
};
