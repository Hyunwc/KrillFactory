// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KFMachineBase.generated.h"

class UKFData;
class UWidgetComponent;
class USceneComponent;
class UKFPopupUserWidget;
class UBoxComponent;
class UKFInteractComponent;

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
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UKFPopupUserWidget* KFPopup;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interact")
	UKFInteractComponent* InteractComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	UBoxComponent* MouseInteractComponent;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UWidgetComponent* PopupWidget; // 팝업 위젯

protected:
	UFUNCTION(BlueprintCallable)
	void InitDataPopup();

	//UFUNCTION(BlueprintCallable)
	//virtual void BeginFocus() override;
	//
	//UFUNCTION(BlueprintCallable)
	//virtual void EndFocus() override;
public:
	UFUNCTION(BlueprintCallable)
	UWidgetComponent* GetPopupWidget() { return PopupWidget; }
};
