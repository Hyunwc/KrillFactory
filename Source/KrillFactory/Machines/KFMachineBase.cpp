// Fill out your copyright notice in the Description page of Project Settings.


#include "Machines/KFMachineBase.h"
#include "Data/KFData.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/SceneComponent.h"
#include "Components/TextBlock.h"
#include "Components/BoxComponent.h"
#include "Components/KFInteractComponent.h"
#include "UI/KFPopupUserWidget.h"

AKFMachineBase::AKFMachineBase()
{
	PrimaryActorTick.bCanEverTick = false;
	bIsPowerOn = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// TODO : MosueInteract용 콜리전 채널을 따로 만들어서 설정합니다
	MouseInteractComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("MouseCollision"));
	MouseInteractComponent->SetupAttachment(Root);
	MouseInteractComponent->SetCollisionProfileName("NoCollision");

	// 액터 컴포넌트는 자동으로 부착됨 SetupAttachment 필요업음
	InteractComponent = CreateDefaultSubobject<UKFInteractComponent>(TEXT("Interact"));
	
	PopupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Popup"));
	PopupWidget->SetupAttachment(Root);
	PopupWidget->SetRelativeLocation(FVector::ZeroVector);

	static ConstructorHelpers::FClassFinder<UUserWidget> PopupRef(TEXT("/Game/DT/Blueprints/Widgets/WBP_Popup.WBP_Popup_C"));
	if (PopupRef.Class != nullptr)
	{
		PopupWidget->SetWidgetClass(PopupRef.Class);
	}

	PopupWidget->SetVisibility(false);
	KFPopup = nullptr;
}

void AKFMachineBase::BeginPlay()
{
	Super::BeginPlay();

	if (PopupWidget)
	{
		KFPopup = Cast<UKFPopupUserWidget>(PopupWidget->GetUserWidgetObject());

		if (KFPopup)
		{
			// 유효하다면 데이터 초기화
			InitDataPopup();
		}
	}
}

void AKFMachineBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKFMachineBase::InitDataPopup()
{
	if (KFPopup && MachineData)
	{
		KFPopup->UpdatePopupContent(MachineData->Name, MachineData->Info);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT(" MachineBase : 데이터가 누락됐습니다"));
	}
}


