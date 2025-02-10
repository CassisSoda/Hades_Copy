// Fill out your copyright notice in the Description page of Project Settings.

//�÷��̾� Ŭ���� ��, ���ʹ�(Ÿ���佺) Ŭ�������� ����!


#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KEnemyFSM.generated.h"


UENUM(BlueprintType)
enum class EKEnemyState : uint8
{
	Idle	UMETA(DisplayName = "���"),
	Move	UMETA(DisplayName = "�̵�"),
	MoveFar UMETA(DisplayName = "�̵�2"),
	Attack	UMETA(DisplayName = "����"),
	Damaged	UMETA(DisplayName = "������"),
	Dead	UMETA(DisplayName = "���"),
	EndState,
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_PLUTO_API UKEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UKEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	//State ���� ó��
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FSM)
	EKEnemyState mState = EKEnemyState::Idle;

	//�⺻ State
	virtual void State_Idle();
	virtual void State_Move();
	virtual void State_MoveFar();
	virtual void State_Attack();
	virtual void State_Damaged();
	virtual void State_Dead();

	//������ ���� �Լ�
	virtual void OnDamagedProcess();
	//���� ���� �Լ�
	virtual void OnAttackProcess();

public:

	struct Status
	{

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	int32 hp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float attackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
	float attackDelayTime;
	};

	
	//��ӹ޾ƾ� �� ��ġ�� ������ ������ �����޴°� ����?


	//Ÿ�� (�÷��̾� Ŭ������ ��� �ּ�ó��)
	//UPROPERTY(VisibleAnywhere, Category = FSM)
	//class ATPSPlayer* targetPlayer;

	//UPROPERTY(EditAnywhere, Category = FSM)

};
