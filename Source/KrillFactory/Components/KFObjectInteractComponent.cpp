// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/KFObjectInteractComponent.h"
#include "Objects/AInteractableBase.h"
#include "UI/KFPopupUserWidget.h"
#include "UI/KFPopupHUD.h"
#include "Components/WidgetComponent.h"
#include "Controllers/KFPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Data/KFData.h"

UKFObjectInteractComponent::UKFObjectInteractComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

}

void UKFObjectInteractComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<AAInteractableBase>(GetOwner());
	PC = Cast<AKFPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
}


void UKFObjectInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UKFObjectInteractComponent::BeginFocus()
{
	if (Owner)
	{
		if (PC)
		{
			if (PC->PopupHUD)
			{
				PC->PopupHUD->UpdatePopupContent(Owner->Data->Name, Owner->Data->Info, Owner->Data->Icon);
				PC->PopupHUD->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}

			//if (UWidgetComponent* Button = Owner->GetButtonWidget())
			//{
			//	Button->SetVisibility(true);
			//}
		}
	}
}

void UKFObjectInteractComponent::EndFocus()
{
	if (Owner)
	{
		if (PC)
		{
			if (PC->PopupHUD)
			{
				PC->PopupHUD->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void UKFObjectInteractComponent::Click()
{
}

