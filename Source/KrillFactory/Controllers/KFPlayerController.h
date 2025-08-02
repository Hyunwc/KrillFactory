// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KFPlayerController.generated.h"

class UUserWidget;
class AMainPower;

/**
 * 
 */
UCLASS()
class KRILLFACTORY_API AKFPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AKFPlayerController();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> MainHUDWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> MachinePopupWidgetClass;

private:
	UPROPERTY()
	UUserWidget* MainHUD;

	UPROPERTY()
	AMainPower* MainPowerActor; // 메인 파워 참조

	UPROPERTY()
	ACameraActor* CurrentCamera;

	UPROPERTY()
	UUserWidget* PopupWidget;

public:
	UFUNCTION(BlueprintCallable, Category = "Power")
	void ToggleMainPower();  // Widget에서 버튼 클릭시 호출할 함수

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SwitchCamera(FName CameraTag); // 카메라 변경 이벤트 함수

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowMachinePopup(FText MachineName);
};
