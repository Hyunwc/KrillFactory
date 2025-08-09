// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "KFFieldCamera.generated.h"

/**
 * 
 */
UCLASS()
class KRILLFACTORY_API AKFFieldCamera : public ACameraActor
{
	GENERATED_BODY()

public:
	// 월드에서 배치 후 편집하기 위함
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "List Data")
	FText DisplayName;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "List Data")
	FName CameraTag;
};
