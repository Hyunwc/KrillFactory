// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AInteractableBase.generated.h"

class USceneComponent;
class UBoxComponent;
class UKFData;
class UKFObjectInteractComponent;
class UWidgetComponent;
class UStaticMeshComponent;

UCLASS()
class KRILLFACTORY_API AAInteractableBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AAInteractableBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	UBoxComponent* MouseInteractComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UWidgetComponent* ButtonWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interact")
	UKFObjectInteractComponent* InteractComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	UStaticMeshComponent* Mesh; 

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UWidgetComponent* PopupWidget; // 팝업 위젯

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Data")
	UKFData* Data;

public:
	UFUNCTION(BlueprintCallable)
	UWidgetComponent* GetPopupWidget() { return PopupWidget; }
};
