// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KEnemyFSM.h"
#include "KThanatos.h"
#include "KThanatosFSM.generated.h"



UCLASS()
class PROJECT_PLUTO_API UKThanatosFSM : public UKEnemyFSM
{
	GENERATED_BODY()


public:
	UKThanatosFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	//�⺻ State
	void State_Idle()override;
	void State_Move()override;
	void State_Attack()override;
	void State_Attack2();
	void State_MoveFar()override;

	//������ ���� �Լ�
	void OnDamagedProcess()override;
	//���� ���� �Լ�
	void OnAttackProcess()override;
	bool GetIsAttack();


public:
	//��� �ð�
	UPROPERTY(EditAnywhere, Category = FSM)
	float IdleDelayTime = 2.0f;

	//��� �ð�
	UPROPERTY(EditAnywhere, Category = FSM)
	float WalkingTime = 1.0f;


	//����ð�
	UPROPERTY(EditAnywhere, Category = FSM)
	float currentTime = 0.0f;

	//���� ����
	UPROPERTY(EditAnywhere, Category = FSM)
	float attackRange = 1000.0f;
	
	UPROPERTY(VisibleAnywhere, Category = FSM)
	class AKEnemy* target_Enemy;

	int count = 0;

	UPROPERTY(EditDefaultsOnly, BluePrintReadWrite, Category = FSM)
	bool IsAttack;


	//���� ����
	UPROPERTY()
	class AKThanatos* me;

	 Status status;


public:

};
