// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KFMachineBase.generated.h"

class UKFData;
class UWidgetComponent;
class USceneComponent;

UCLASS()
class KRILLFACTORY_API AKFMachineBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AKFMachineBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UKFData* MachineData; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	bool bIsPowerOn; // 기계 전원

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UWidgetComponent* PopupWidget; // 팝업 위젯

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	class UKFPopupUserWidget* KFPopup;

protected:
	UFUNCTION(BlueprintCallable)
	void InitDataPopup();

	//UFUNCTION(BlueprintCallable)
	//virtual void BeginFocus() override;
	//
	//UFUNCTION(BlueprintCallable)
	//virtual void EndFocus() override;
};
