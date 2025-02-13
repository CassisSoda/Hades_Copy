// Fill out your copyright notice in the Description page of Project Settings.


#include "KThanatosFSM.h"
#include "Kismet/GameplayStatics.h"
#include "KEnemy.h"

UKThanatosFSM::UKThanatosFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}

void UKThanatosFSM::BeginPlay()
{
	Super::BeginPlay();

	status.attackDelayTime = 2.0f;
	// ...
	//���忡�� Enemy�� ã�ƿ���
	auto actor_Enemy = UGameplayStatics::GetActorOfClass(GetWorld(), AKEnemy::StaticClass());

	if (actor_Enemy != nullptr)
	{
		target_Enemy = Cast<AKEnemy>(actor_Enemy);
	}

	//���� ��ü ��������
	me = Cast<AKThanatos>(GetOwner());



}

void UKThanatosFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ����â�� ���� �޽��� ����ϱ�
	FString logMsg = UEnum::GetValueAsString(mState);
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Red, logMsg);
	

	switch (mState)
	{
	case EKEnemyState::Idle:	{ State_Idle();		} break;
	case EKEnemyState::Move:	{ State_Move();		} break;
	case EKEnemyState::MoveFar: { State_MoveFar();	} break;
	case EKEnemyState::Attack:	{ State_Attack();	} break;
	case EKEnemyState::Attack2:	{ State_Attack2();	} break;
	}

}

void UKThanatosFSM::State_Idle()
{
	//�ð��� �帣�ٰ�
	currentTime += GetWorld()->DeltaTimeSeconds;

	//���� ��� �ð��� ��� �ð��� �ʰ��ߴٸ�
	if (currentTime > IdleDelayTime)
	{
		//�̵� ���·� ��ȯ
		mState = EKEnemyState::Move;

		//��ȯ �� �ð� �ʱ�ȭ
		currentTime = 0.0f;
	}

}

void UKThanatosFSM::State_Move()
{
	FVector destination = target_Enemy->GetActorLocation();
	FVector dir = destination - me->GetActorLocation();
	me->AddMovementInput(dir.GetSafeNormal());

	//1��(WalkingTime) ���� �Ŀ� 2��(IdleDelayTime) ����ϰ� ����
	currentTime += GetWorld()->DeltaTimeSeconds;

	if (currentTime > WalkingTime)
	{
		mState = EKEnemyState::Idle;
		currentTime = 0.0f;
		//�Ÿ�üũ
		if (dir.Size() < attackRange)
		{
			//���� ���·� ��ȯ�ϰ� �ʹ�
			mState = EKEnemyState::Attack;
			currentTime = 0.0f;
		}
	}

	


}

void UKThanatosFSM::State_Attack()
{
	//���� �ð��� �� ���� ����

	//�ð��� �帣�ٰ�
	currentTime += GetWorld()->DeltaTimeSeconds;
	//���� �ð��̵Ǹ� ������ ����
	if (currentTime > status.attackDelayTime)
	{
		currentTime = 0.0f;

		count++;
		mState = EKEnemyState::MoveFar;
		IsAttack = false;
	}
}


void UKThanatosFSM::State_Attack2()
{
	attackRange = 2000.0f;
	currentTime += GetWorld()->DeltaTimeSeconds;

	//���� �ð��̵Ǹ� ������ ����
	if (currentTime > status.attackDelayTime)
	{
		currentTime = 0.0f;
		
		count=0;
		mState = EKEnemyState::MoveFar;
	}

	attackRange = 1000.0f;
	IsAttack = false;
}

void UKThanatosFSM::State_MoveFar()
{
	FVector destination = target_Enemy->GetActorLocation();
	FVector dir = me->GetActorLocation() - destination;



	me->AddMovementInput(dir.GetSafeNormal());

	//1��(WalkingTime) ���� �Ŀ� 2��(IdleDelayTime) ����ϰ� ����
	currentTime += GetWorld()->DeltaTimeSeconds;

	if (currentTime > WalkingTime)
	{
		mState = EKEnemyState::Idle;
		currentTime = 0.0f;
	}

	
}

void UKThanatosFSM::OnDamagedProcess()
{
	//�ݸ��� �ڽ� or �����ص� �ڽ��� ������ �ϱ�.
}

void UKThanatosFSM::OnAttackProcess()
{
		IsAttack = true;


		if (count >= 10)
		{
			mState = EKEnemyState::Attack2;
		}

		else
		{
			mState = EKEnemyState::Attack;

		}
}

bool UKThanatosFSM::GetIsAttack()
{
	return IsAttack;
}

