// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "KFPopupHUD.generated.h"

class UKFPopupUserWidget;
/**
 * 
 */
UCLASS()
class KRILLFACTORY_API AKFPopupHUD : public AHUD
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UKFPopupUserWidget* PopupWidget;

	// 아키타입에서 할당
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> PopupWidgetClass;

public:
	UFUNCTION(BlueprintCallable)
	void ShowPopup(const FText& InNameText, const FText& InInfoText);
};
