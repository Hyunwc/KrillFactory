// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/KFInteractComponent.h"
#include "Machines/KFMachineBase.h"
#include "UI/KFPopupUserWidget.h"
#include "UI/KFPopupHUD.h"
#include "Components/WidgetComponent.h"
#include "Controllers/KFPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Data/KFData.h"

UKFInteractComponent::UKFInteractComponent()
{
    // 만약 InitializeComponent()를 사용하고 싶다면 이 값을 true로
	PrimaryComponentTick.bCanEverTick = false;
	bIsFocus = false;
}


void UKFInteractComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<AKFMachineBase>(GetOwner());
	PC = Cast<AKFPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
}


void UKFInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UKFInteractComponent::BeginFocus()
{
	if (bIsFocus)
	{
		return;
	}

	bIsFocus = true;

	if (Owner)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Interact : Owner Ok!!")));

		if (PC)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Interact : PC Ok!!")));

			if (PC->PopupHUD)
			{
				PC->PopupHUD->UpdatePopupContent(Owner->MachineData->Name, Owner->MachineData->Info);
				PC->PopupHUD->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}
    }
}

void UKFInteractComponent::EndFocus()
{
	if (!bIsFocus)
	{
		return;
	}

	bIsFocus = false;

	if (Owner)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Interact : Owner Ok!!")));

		if (PC)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("Interact : PC Ok!!")));

			if (PC->PopupHUD)
			{
				PC->PopupHUD->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
}

void UKFInteractComponent::Click()
{
}

