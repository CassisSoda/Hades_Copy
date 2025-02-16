// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KThanatos.h"
#include "KThanatosFSM.generated.h"

UENUM(BlueprintType)
enum class EThanatosState : uint8
{
	Start	UMETA(DisplayName = "Start"),
	Idle	UMETA(DisplayName = "Idle"),
	Move	UMETA(DisplayName = "Move"),
	MoveFar UMETA(DisplayName = "MoveFar"),
	Attack1	UMETA(DisplayName = "Attack_1"),
	Attack2	UMETA(DisplayName = "Attack_2"),
	Damaged	UMETA(DisplayName = "Damage"),
	Dead	UMETA(DisplayName = "Dead"),
	EndState,
};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_PLUTO_API UKThanatosFSM : public UActorComponent
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

	struct Status
	{

	//Ÿ���佺�� HP������ ���� (����)
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	//int32 hp;

	//Idle (�����) ���ϴ� �ð�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float idleDelayTime = 0.5f;

	//�ȴ� �ð�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float walkingTime = 1.0f;

	//���� ��Ÿ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float attackRange = 1000.0f;

	//���� ������ Ÿ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float attackDelayTime = 0.9f;

	};



public:
	//State ���� ó��
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FSM)
	EThanatosState mState = EThanatosState::Start;

public:

	//�⺻ State
	void State_Start();
	void State_Idle();
	void State_Move();
	void State_MoveFar();
	void State_Attack1();
	void State_Attack2();


	//������ ���� �Լ�
	void OnDamagedProcess();
	//���� ���� �Լ�
	void OnAttackProcess();
	void EndAttackProcess();
	//bool GetIsAttack();

	//�����ڷ� OnAttackEnd
	void OnAttackEnd();



public:

	//����ð�
	UPROPERTY(EditAnywhere, Category = FSM)
	float currentTime = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = FSM)
	class AKEnemy* target_Enemy;

	int skillCount = 0;
	int maxSkillCount = 4;

	Status status;


	//���� ����
	UPROPERTY()
	class AKThanatos* me;

	UPROPERTY()
	class UKThanatosAnim* Anim;

//
//	UPROPERTY(EditDefaultsOnly, BluePrintReadWrite, Category = FSM)
//	bool IsAttack = false;

};
