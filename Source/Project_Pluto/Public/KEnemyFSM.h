// Fill out your copyright notice in the Description page of Project Settings.

//�÷��̾� Ŭ���� ��, ���ʹ�(Ÿ���佺) Ŭ�������� ����!
//2025.02.16 Ÿ���佺 FSM �и���


#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KEnemyFSM.generated.h"


UENUM(BlueprintType)
enum class EKEnemyState : uint8
{
	Start	UMETA(DisplayName = "Start"),
	Idle	UMETA(DisplayName = "Idle"),
	Move	UMETA(DisplayName = "Move"),
	MoveFar UMETA(DisplayName = "MoveFar"),
	Attack	UMETA(DisplayName = "Attack_1"),
	Attack2	UMETA(DisplayName = "Attack_2"),
	Damaged	UMETA(DisplayName = "Damage"),
	Dead	UMETA(DisplayName = "Dead"),
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


	
	//��ӹ޾ƾ� �� ��ġ�� ������ ������ �����޴°� ����?


	//Ÿ�� (�÷��̾� Ŭ������ ��� �ּ�ó��)
	//UPROPERTY(VisibleAnywhere, Category = FSM)
	//class ATPSPlayer* targetPlayer;

	//UPROPERTY(EditAnywhere, Category = FSM)

};
