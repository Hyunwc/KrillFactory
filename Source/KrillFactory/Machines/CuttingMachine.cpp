// Fill out your copyright notice in the Description page of Project Settings.


#include "Machines/CuttingMachine.h"

// Sets default values
ACuttingMachine::ACuttingMachine()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACuttingMachine::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACuttingMachine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

