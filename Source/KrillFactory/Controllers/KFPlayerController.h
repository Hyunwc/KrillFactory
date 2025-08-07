// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KFPlayerController.generated.h"

class UUserWidget;
class AMainPower;
class UKFPopupUserWidget;

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
	TSubclassOf<UUserWidget> PopupWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> OrderWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> ProcessWidgetClass;

	UPROPERTY(BlueprintReadWrite)
	UKFPopupUserWidget* PopupHUD;

	UPROPERTY(BlueprintReadWrite)
	UUserWidget* OrderWidget;

	UPROPERTY(BlueprintReadWrite)
	UUserWidget* ProcessWidget;

	// 게임 생산 관리 변수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Production")
	int32 TargetProductionCount; // 목표 생산 개수

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Production")
	int32 CurrentCompletedCount; // 현재 완료된 생산 개수

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsProduction; // 생산 진행 중인지?

private:
	UPROPERTY()
	UUserWidget* MainHUD;

	UPROPERTY()
	AMainPower* MainPowerActor; // 메인 파워 참조

	UPROPERTY()
	ACameraActor* CurrentCamera;

	UPROPERTY()
	UUserWidget* PopupWidget;

	UPROPERTY()
	int32 ElapsedTime; // 경과 시간을 초 단위로 저장

	FTimerHandle ElapsedTimerHandle; // 타이머 핸들

public:
	UFUNCTION(BlueprintCallable, Category = "Power")
	void ToggleMainPower();  // Widget에서 버튼 클릭시 호출할 함수

	UFUNCTION(BlueprintCallable, Category = "Camera")
	void SwitchCamera(FName CameraTag); // 카메라 변경 이벤트 함수

	UFUNCTION()
	void UpdateTime(); // 매초마다 호출될 타이머

	UFUNCTION(BlueprintPure, Category = "Game State")
	FText GetElapsedTimeText() const;

	UFUNCTION(BlueprintCallable, Category = "Production")
	void SetTargetProductioncount(int32 NewTarget);

	UFUNCTION(BlueprintCallable, Category = "Production")
	void IncrementCompletedCount();

	UFUNCTION(BlueprintPure, Category = "Production")
	FText GetTargetCountText() const;

	UFUNCTION(BlueprintPure, Category = "Production")
	FText GetCompletedCountText() const;

	UFUNCTION(BlueprintPure, Category = "Production")
	FText GetProductionStatusText() const;

	UFUNCTION(BlueprintCallable)
	void ShowOrderScreen();

	UFUNCTION(BlueprintCallable)
	void StartProduction(int32 TargetCount);

	//UFUNCTION(BlueprintCallable)
	//void EndProduction();
};
