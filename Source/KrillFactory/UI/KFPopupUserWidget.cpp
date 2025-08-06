// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/KFPopupUserWidget.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "Components/Image.h"
#include "Styling/SlateBrush.h"

void UKFPopupUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
    
    //Txt_Name = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Name1")));
    //Txt_Info = Cast<UTextBlock>(GetWidgetFromName(TEXT("Txt_Info1")));

	if (Txt_Name && Txt_Info && Img_Icon)
	{
		Txt_Name->SetText(CachedName);
		Txt_Info->SetText(CachedInfo);
	}

	//UpdatePopupContent(CachedNameText, CachedInfoText);
}

void UKFPopupUserWidget::UpdatePopupContent(const FText& InNameText, const FText& InInfoText, UTexture2D* InIcon)
{
	CachedName = InNameText;
	CachedInfo = InInfoText;

	if (Txt_Name && Txt_Info)
	{
		Txt_Name->SetText(CachedName);
		Txt_Info->SetText(CachedInfo);
	}

	if (Img_Icon && InIcon)
	{
		Img_Icon->SetBrushFromTexture(InIcon);
	}
	else if (Img_Icon)
	{
		Img_Icon->SetBrushFromTexture(nullptr);
	}
}
