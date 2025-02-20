// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayerWeapon.generated.h"

UCLASS()
class PROJECT_PLUTO_API APlayerWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


protected:
	// ���� ���� �� �о�ͼ� �����ؾ��Ұ� ���⿡? �ҷ��´�
	UPROPERTY()
	class APlayerZagreus* player;

	// ���� �����Լ��� ������� �򰥷��� �׳� �����Լ���
	virtual void AttackProcess(AActor* OtherActor); // �Ϲ� ���� & �޺�
	virtual void SpecialAttProcess(AActor* OtherActor); // Ư�� ���� (Q ��ų)

public:
	UPROPERTY(EditAnywhere)
	class USceneComponent* RootComp;

	// ���� overlap
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* CollisionComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combo")
	int32 MaxCombo = 1; // �� ���⺰ �ִ� Attack �޺�

	virtual void StartAttack();
	virtual void EndAttack();
	virtual void StartSpecialAtt();
	virtual void EndSpecialAtt();

	virtual void LastCombo();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buff")
	float attackBuff = 0.0f; // �Ϲ� ���� ��ȭ (�Ͻ��� ��ȭ)
};
