// Fill out your copyright notice in the Description page of Project Settings.


#include "Objects/AInteractableBase.h"
#include "Data/KFData.h"
#include "Components/WidgetComponent.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Components/KFObjectInteractComponent.h"
#include "Components/StaticMeshComponent.h"

AAInteractableBase::AAInteractableBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	MouseInteractComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	MouseInteractComponent->SetupAttachment(Root);
	MouseInteractComponent->SetCollisionProfileName("NoCollision");

	// TODO : 위젯컴포넌트는 MachineBase에서 잘 동작하면 구현하도록 합니다.

	PopupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Popup"));
	PopupWidget->SetupAttachment(Root);
	PopupWidget->SetRelativeLocation(FVector::ZeroVector);

	static ConstructorHelpers::FClassFinder<UUserWidget> PopupRef(TEXT("/Game/DT/Blueprints/Widgets/WBP_PopupHUD.WBP_PopupHUD_C"));
	if (PopupRef.Class != nullptr)
	{
		PopupWidget->SetWidgetClass(PopupRef.Class);
	}

	PopupWidget->SetVisibility(false);

	InteractComponent = CreateDefaultSubobject<UKFObjectInteractComponent>(TEXT("Interact"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
}

void AAInteractableBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAInteractableBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

