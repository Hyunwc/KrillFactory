// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KFPopupUserWidget.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class KRILLFACTORY_API UKFPopupUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Name;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Txt_Info;

public:
	UFUNCTION(BlueprintCallable, Category = "Popup Widget")
	void UpdatePopupContent(const FText& InNameText, const FText& InInfoText);

private:
	FText CachedName;
	FText CachedInfo;
};
