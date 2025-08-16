// Fill out your copyright notice in the Description page of Project Settings.


#include "Machines/KFRobotArm.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/KFRobotArmFSM.h"
#include "BoxingMachine.h"
#include "Kismet/GameplayStatics.h"

AKFRobotArm::AKFRobotArm()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(Root);

	FSM = CreateDefaultSubobject<UKFRobotArmFSM>(TEXT("FSM"));
}

void AKFRobotArm::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> FoundActor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABoxingMachine::StaticClass(), FoundActor);
	if (FoundActor.Num() > 0)
	{
		BoxingMachine = Cast<ABoxingMachine>(FoundActor[0]);
	}

	if (FSM && BoxingMachine)
	{
		BoxingMachine->OnBoxComplete.AddDynamic(FSM, &UKFRobotArmFSM::HandleBoxComplete);
	}
}

void AKFRobotArm::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


