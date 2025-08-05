// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "KFData.generated.h"

/**
 * 
 */
UCLASS()
class KRILLFACTORY_API UKFData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FText Info;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	class UTexture2D* Icon;
};
