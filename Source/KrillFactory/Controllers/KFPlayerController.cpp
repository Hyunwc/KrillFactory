// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/KFPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Machines/MainPower.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetTextLibrary.h"
#include "Camera/CameraActor.h"
#include "Components/TextBlock.h"
#include "UI/KFPopupUserWidget.h"
#include "Machines/KFFieldCamera.h"

AKFPlayerController::AKFPlayerController()
{
	// 플레이어 컨트롤러가 폰을 소유하지 않도록 설정
	bAutoManageActiveCameraTarget = false;
	MainHUD = nullptr;
	MainPowerActor = nullptr;
	CurrentCamera = nullptr;

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	TargetProductionCount = 0;
	CurrentCompletedCount = 0;
	bIsProduction = false;
}

void AKFPlayerController::BeginPlay()
{
	EnableInput(this);
	
	SetInputMode(FInputModeGameAndUI());
	
	// 위젯 클래스가 유효하고 아직 위젯이 생성되지 않았을 경우
	if (MainHUDWidgetClass)
	{
		// 위젯 인스턴스 생성
		MainHUD = CreateWidget(this, MainHUDWidgetClass);
		
		if (MainHUD)
		{
			// 화면에 위젯 추가
			MainHUD->AddToViewport();
		}
	}

	if (PopupWidgetClass)
	{
		PopupHUD = CreateWidget<UKFPopupUserWidget>(this, PopupWidgetClass);
		if (PopupHUD)
		{
			PopupHUD->AddToViewport();
			PopupHUD->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (OrderWidgetClass)
	{
		OrderWidget = CreateWidget(this, OrderWidgetClass);
		if (OrderWidget)
		{
			OrderWidget->AddToViewport();

			OrderWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (ProcessWidgetClass)
	{
		ProcessWidget = CreateWidget(this, ProcessWidgetClass);
		if (ProcessWidget)
		{
			ProcessWidget->AddToViewport();

			ProcessWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMainPower::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		MainPowerActor = Cast<AMainPower>(FoundActors[0]);
	}

	TArray<AActor*> FoundCamera;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), CameraActorClass, FoundCamera);

	if (FoundActors.Num() <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("KFPC : 1개도 못찾았어요")));

	}
	// CurrentCamera는 AKFFieldCamera 타입임
	for (AActor* Actor : FoundCamera)
	{
		AKFFieldCamera* Camera = Cast<AKFFieldCamera>(Actor);
		if (Camera && Camera->CameraTag == TEXT("MainCamera"))
		{
			CurrentCamera = Camera;
			SetViewTarget(CurrentCamera);
		}
	}

	ElapsedTime = 0;
	GetWorld()->GetTimerManager().SetTimer(ElapsedTimerHandle, this, &AKFPlayerController::UpdateTime, 1.0f, true);

}

void AKFPlayerController::ToggleMainPower()
{
	if (MainPowerActor)
	{
		MainPowerActor->TogglePower();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerController : MainPower Actor Not Found!"));
	}
}

void AKFPlayerController::SwitchCamera(FName CameraTag)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), CameraActorClass, FoundActors);

	if (CameraActorClass == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("KFPC : CameraActor Null")));

	}

	if (FoundActors.Num() <= 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("KFPC : 1개도 못찾았어요")));

	}
	for (AActor* Actor : FoundActors)
	{
		AKFFieldCamera* Camera = Cast<AKFFieldCamera>(Actor);
		if (Camera && Camera->CameraTag == CameraTag)
		{
			SetViewTargetWithBlend(Camera, 1.0f);
			CurrentCamera = Camera;
		}
	}
}

void AKFPlayerController::UpdateTime()
{
	ElapsedTime++;
}

FText AKFPlayerController::GetElapsedTimeText() const
{
	int32 Minutes = ElapsedTime / 60;
	int32 Seconds = ElapsedTime % 60;
	
	FString TimeString = FString::Printf(TEXT("접속 시간 : %02d : %02d"), Minutes, Seconds);
	return FText::FromString(TimeString);
}

void AKFPlayerController::SetTargetProductioncount(int32 NewTarget)
{
	TargetProductionCount = FMath::Max(0, NewTarget);
}

void AKFPlayerController::IncrementCompletedCount()
{
	if (bIsProduction)
	{
		CurrentCompletedCount++;

		if (TargetProductionCount > 0 && CurrentCompletedCount >= TargetProductionCount)
		{
			bIsProduction = false;
		}
	}
}

FText AKFPlayerController::GetTargetCountText() const
{
	return FText::AsNumber(TargetProductionCount);
}

FText AKFPlayerController::GetCompletedCountText() const
{
	return FText::AsNumber(CurrentCompletedCount);
}

FText AKFPlayerController::GetProductionStatusText() const
{
	if (!bIsProduction)
	{
		return FText::FromString(TEXT("준비 중..."));
	}
	else if (TargetProductionCount <= 0)
	{
		return FText::FromString(TEXT("목표 미설정"));
	}
	else if (CurrentCompletedCount >= TargetProductionCount)
	{
		return FText::FromString(TEXT("완료!"));
	}
	else
	{
		return FText::FromString(TEXT("진행 중..."));
	}
	
}

void AKFPlayerController::ShowOrderScreen()
{
	if (OrderWidget)
	{
		//OrderWidget->AddToViewport();
		OrderWidget->SetVisibility(ESlateVisibility::Visible);
	}
	// TODO : 혹시나 기타 위젯이 거슬린다면 지우도록 합니다..
}

void AKFPlayerController::StartProduction(int32 TargetCount)
{
	if (TargetCount <= 0)
	{
		return;
	}

	SetTargetProductioncount(TargetCount);
	CurrentCompletedCount = 0;
	bIsProduction = true;

	if (ProcessWidget)
	{
		ProcessWidget->SetVisibility(ESlateVisibility::Visible);
	}
}
