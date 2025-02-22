// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "KEnemy.generated.h"

UCLASS()
class PROJECT_PLUTO_API AKEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AKEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	class UKEnemyFSM* fsm;

public:
	//Ÿ���佺 1�� ��ų�� �ɸ� ��� true üũ
	bool bSoonDead = false;

	
	float currentTime = 0.f;
	float deadDelayTime = 2.f;

	
	//bSoonDead�� True�� ��� deadDelayTime üũ�ؼ� ��Ʈ����
	void OnDeadProcess(float DeltaTime);

	//bSoonDead�� true�� �������ִ� �Լ�
	void SetbSoonDead();
	
};