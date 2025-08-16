// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Machines/KFMachineBase.h"
#include "KFRobotArm.generated.h"


class USkeletalMeshComponent;
class ABoxingMachine;
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ABoxingMachine* BoxingMachine; // 박스머신 참조
	
};
