// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KrillBlock.generated.h"

UENUM(BlueprintType)
enum class EBlockType : uint8
{
	EBT_Full	UMETA(DisplayName = "Full Block"),	  // 원래 크기 
	EBT_Quarter UMETA(DisplayName = "Quarter Block"), // 4등분된
	//EBT_Eighth	UMETA(DisplayName = "Eighth Block"),  // 8등분된 (추후 확장),
	//EBT_Packged UMETA(DisplayName = "Packaged Block"), // 비닐에 포장된 (추후 확장),

	EBT_Max // 센티널 값
};

UCLASS()
class KRILLFACTORY_API AKrillBlock : public AActor
{
	GENERATED_BODY()
	
public:	
	AKrillBlock();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Product")
	class UStaticMeshComponent* BlockMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Block")
	EBlockType BlockType; // 블록 타입 변수 추가

public:
	// 블록 타입 설정 함수
	void SetBlockType(EBlockType NewType);
	// 메시를 블록 타입에 맞게 업데이트하는 함수
	void UpdateMeshForBlockType();
};
