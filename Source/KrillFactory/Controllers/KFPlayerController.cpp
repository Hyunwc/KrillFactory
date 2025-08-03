// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/KFPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Machines/MainPower.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraActor.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetTextLibrary.h"

AKFPlayerController::AKFPlayerController()
{
	// 플레이어 컨트롤러가 폰을 소유하지 않도록 설정
	bAutoManageActiveCameraTarget = false;
	MainHUD = nullptr;
	MainPowerActor = nullptr;
	CurrentCamera = nullptr;

}

void AKFPlayerController::BeginPlay()
{
	EnableInput(this);
	bShowMouseCursor = true;

	SetInputMode(FInputModeGameAndUI());
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	// 위젯 클래스가 유효하고 아직 위젯이 생성되지 않았을 경우
	if (MainHUDWidgetClass && !MainHUD)
	{
		// 위젯 인스턴스 생성
		MainHUD = CreateWidget(this, MainHUDWidgetClass);
		
		if (MainHUD)
		{
			// 화면에 위젯 추가
			MainHUD->AddToViewport();
			// 마우스 커서가 보이게
			//bShowMouseCursor = true; 
		}
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMainPower::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		MainPowerActor = Cast<AMainPower>(FoundActors[0]);
	}

	// 월드에서 "MainCamera" 태그를 가진 액터를 찾아 뷰 타겟으로 설정
	TArray<AActor*> FoundCameraActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("MainCamera"), FoundCameraActors);
	if (FoundCameraActors.Num() > 0)
	{
		CurrentCamera = Cast<ACameraActor>(FoundCameraActors[0]);
		if (CurrentCamera)
		{
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
	TArray<AActor*> FoundCameraActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), CameraTag, FoundCameraActors);

	if (FoundCameraActors.Num() > 0)
	{
		ACameraActor* TargetCamera = Cast<ACameraActor>(FoundCameraActors[0]);
		if (TargetCamera)
		{
			// 부드럽게 시점 전환해주는 함수
			SetViewTargetWithBlend(TargetCamera, 1.0f);
			CurrentCamera = TargetCamera;
		}
	}
}

void AKFPlayerController::ShowMachinePopup(FText MachineName)
{
	if (MachinePopupWidgetClass && !PopupWidget)
	{
		PopupWidget = CreateWidget(this, MachinePopupWidgetClass);
		if (PopupWidget)
		{
			PopupWidget->AddToViewport();

			UTextBlock* MachineNameText = Cast<UTextBlock>(PopupWidget->GetWidgetFromName(TEXT("MachineName")));
			if (MachineNameText)
			{
				MachineNameText->SetText(MachineName);
			}
		}
	}
	else if (MachinePopupWidgetClass)
	{
		PopupWidget->SetVisibility(ESlateVisibility::Visible);
		UTextBlock* MachineNameText = Cast<UTextBlock>(PopupWidget->GetWidgetFromName(TEXT("MachineName")));
		if (MachineNameText)
		{
			MachineNameText->SetText(MachineName);
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
	
	FString TimeString = FString::Printf(TEXT("경과 시간 : %02d : %02d"), Minutes, Seconds);
	return FText::FromString(TimeString);
}
