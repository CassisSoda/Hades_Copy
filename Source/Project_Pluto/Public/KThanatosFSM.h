// Fill out your copyright notice in the Description page of Project Settings.

//cpp ������ ��ü�� ���� define
//109�� Ŭ���� Ÿ�� ���� + �ش� define ����� Enemy class ���� ����
//TArray ����� �� �������
// �� ���ʹ� �ε�� �迭����, ���ʹ� ���������� ������ ����.
// 1. Ÿ���佺(Ȥ�� ��� �ٸ� �� ��)�� ������ �Ҵ�, �÷��̾� + NPC ��� �ش� ������ üũ�ϱ�
// 2. ���Ӹ�忡�� ������ �����ϱ�


#define ENEMYCLASS AMonster

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
	Idle2	UMETA(DisplayName = "Idle2"),
	Move	UMETA(DisplayName = "Move"),
	MoveFar UMETA(DisplayName = "MoveFar"),
	Attack1	UMETA(DisplayName = "Attack_1"),
	Attack2	UMETA(DisplayName = "Attack_2"),
	Damaged	UMETA(DisplayName = "Damage"),
	Dead	UMETA(DisplayName = "Dead"),
	EndGame	UMETA(DisplayName = "EndGame"),
	EndState,
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_PLUTO_API UKThanatosFSM : public UActorComponent
{
	GENERATED_BODY()


public:
	UKThanatosFSM();

	/*
public:
	UPROPERTY()
	class AHadesGameMode* GM;
	*/

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

	//Idle (�����) ���ϴ� �ð�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float idleDelayTime2 = 3.0f;


	//�ȴ� �ð�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float walkingTime = 1.0f;

	//���� ��Ÿ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float attackRange = 1000.0f;

	//���� ������ Ÿ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float attackDelayTime_1 = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float attackDelayTime_2 = 2.0f;
	

	};


public:
	//State ���� ó��
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FSM)
	EThanatosState mState = EThanatosState::Start;

public:

	//�⺻ State
	void State_Start();
	void State_Idle();
	void State_Idle2();
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


	void SearchEnemy();

	void State_EndGame();


public:


	
	bool bEndGame = false;
	bool bCanDestroySelf = false;

	//����ð�
	UPROPERTY(EditAnywhere, Category = FSM)
	float currentTime = 0.0f;
	float attackCurrent = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = FSM)
	class AMonster* target_Enemy;
	class AMonster* before_Target;



	//TArray<AActor*> target_Enemy_Array;

	//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//	TSubclassOf <class AKDestroyBox> boxPos;

	class AKDestroyBox* destroyBox;

	class AKSkillBox* skillBox;

	int skillCount = 0;
	int maxSkillCount = 1;

	int endGameCount = 0;

	

	Status status;

	//���� ����
	UPROPERTY()
	class AKThanatos* me;

	UPROPERTY()
	class UKThanatosAnim* Anim;

//	FVector firstPos = FVector(3695, -240, 170);
//	FVector seconfPos = FVector(3600, -240, 170);
	FVector lastPos = FVector(3600, -240, 170);
	FVector Location_Zero;


	int32 checkPoint = 0;
	FVector ReturnDest();
};
