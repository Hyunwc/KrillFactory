// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Objects/AInteractableBase.h"
#include "KFPallet.generated.h"

class UStaticMesh;
/**
 * 
 */
UCLASS()
class KRILLFACTORY_API AKFPallet : public AAInteractableBase
{
	GENERATED_BODY()

public:
	AKFPallet();

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	// 박스 메시를 지정
	UPROPERTY(EditAnywhere, Category = "Pallet")
	UStaticMesh* BoxMesh;

	// 박스 배치 정보 
	UPROPERTY()
	TArray<UStaticMeshComponent*> BoxInstance;

	// 박스 배치 위치
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pallet")
	TArray<FTransform> BoxSpawnTransforms;

	// 다음 박스를 쌓을 인덱스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pallet")
	int32 CurrentBoxIndex;

	// 생성할 박스의 개수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pallet")
	int32 MaxBoxCount;

	UPROPERTY(EditDefaultsOnly, Category = "Pallet")
	FVector BoxSpacing;

public:
	// 로봇암이 다음 목표 위치를 얻기 위해 호출하는 함수
	UFUNCTION(BlueprintCallable, Category = "Pallet")
	FTransform GetNextTargetTransform();

	// 로봇암이 박스를 내려놓았을 때 호출하는 함수
	UFUNCTION(BlueprintCallable, Category = "Pallet")
	void ShowBoxAtLocation();

private:
	// 박스 위치를 계산하여 배열을 채우는 내부 함수
	void CalculateBoxTransforms();

	// 박스 컴포넌트를 동적으로 생성하는 내부 함수
	void CreateBoxComponents();
};
