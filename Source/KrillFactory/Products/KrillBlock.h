// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KrillBlock.generated.h"

UENUM(BlueprintType)
enum class EBlockType : uint8
{
	EBT_Full	UMETA(DisplayName = "Full Block"),	  // ���� ũ�� 
	EBT_Quarter UMETA(DisplayName = "Quarter Block"), // 4��е�
	//EBT_Eighth	UMETA(DisplayName = "Eighth Block"),  // 8��е� (���� Ȯ��),
	//EBT_Packged UMETA(DisplayName = "Packaged Block"), // ��ҿ� ����� (���� Ȯ��),

	EBT_Max // ��Ƽ�� ��
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
	EBlockType BlockType; // ��� Ÿ�� ���� �߰�

public:
	// ��� Ÿ�� ���� �Լ�
	void SetBlockType(EBlockType NewType);
	// �޽ø� ��� Ÿ�Կ� �°� ������Ʈ�ϴ� �Լ�
	void UpdateMeshForBlockType();
};
