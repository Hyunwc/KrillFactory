// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KFPopupHUD.h"
#include "UI/KFPopupUserWidget.h"

void AKFPopupHUD::ShowPopup(const FText& InNameText, const FText& InInfoText)
{
	if (!PopupWidget && PopupWidgetClass)
	{
		PopupWidget = CreateWidget<UKFPopupUserWidget>(GetWorld(), PopupWidgetClass);
		PopupWidget->AddToViewport();
	}

	//if (PopupWidget)
	//{
	//	PopupWidget->UpdatePopupContent(InNameText, InInfoText);
	//	PopupWidget->SetVisibility(ESlateVisibility::Visible);
	//}
}
