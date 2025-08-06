// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/KFInteractComponent.h"
#include "Machines/KFMachineBase.h"
#include "UI/KFPopupUserWidget.h"
#include "Components/WidgetComponent.h"

UKFInteractComponent::UKFInteractComponent()
{
    // 만약 InitializeComponent()를 사용하고 싶다면 이 값을 true로
	PrimaryComponentTick.bCanEverTick = true;

}


void UKFInteractComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


void UKFInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UKFInteractComponent::BeginFocus()
{
    if (AKFMachineBase* Owner = Cast<AKFMachineBase>(GetOwner()))
    {
        if (UWidgetComponent* Widget = Owner->GetPopupWidget())
        {
            Widget->SetVisibility(true);
        }
    }
}

void UKFInteractComponent::EndFocus()
{
    if (AKFMachineBase* Owner = Cast<AKFMachineBase>(GetOwner()))
    {
        if (UWidgetComponent* Widget = Owner->GetPopupWidget())
        {
            Widget->SetVisibility(false);
        }
    }
}

