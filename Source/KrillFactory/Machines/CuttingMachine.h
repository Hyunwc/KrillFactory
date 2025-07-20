// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CuttingMachine.generated.h"

class AConveyor;
class AKrillBlock;
class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class KRILLFACTORY_API ACuttingMachine : public AActor
{
	GENERATED_BODY()
	
public:	
	ACuttingMachine();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnCuttingZoneOverlapBegin(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCuttingZoneOverlapEnd(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:	
	virtual void Tick(float DeltaTime) override;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BladeMesh; // Į��

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* CuttingZone; // ���� �̺�Ʈ �߻� ����

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CuttingMachine")
	int32 NumberOfBlades; // Į�� ����
private:
	// ���� Į�� �޽� �ν��Ͻ��� ������ �迭
	UPROPERTY()
	TArray<UStaticMeshComponent*> BladeInstance;

	// Ǯ ����� ����ϱ� ���� �����̾� �ν��Ͻ��� ���忡��
	UPROPERTY()
	AConveyor* FoundConveyor;

	// ���� ���� ���� �ִ� ��ϵ��� ����
	UPROPERTY()
	TSet<AKrillBlock*> BlocksInCuttingZone;

	// �߸� ����� �ٽ� ������ �Ž¤� ���� ���� ��ٿ�(�ɼ�)
	FTimerHandle CuttingCooldownTimerHandle;
	void ClearCuttingCooldown();
	bool bIncuttingCooldown;
	void SpawnBlades(); // Į�� �޽� ���� �� ���� �Լ� (�������Ʈ���� ����??)
};
