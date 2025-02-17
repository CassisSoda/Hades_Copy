// Fill out your copyright notice in the Description page of Project Settings.


#include "KThanatosFSM.h"
#include "Kismet/GameplayStatics.h"
#include "KEnemy.h"
#include "KThanatosAnim.h"
#include "KDestroyBox.h"


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

	//��ų 1�� üũ�� �� �Ǿ� �������� ��������.
	if (actor_Enemy != nullptr)
	{
		target_Enemy = Cast<AKEnemy>(actor_Enemy);
	}


	//���� ��ü ��������
	me = Cast<AKThanatos>(GetOwner());


	if (me)
	{
		Anim = Cast<UKThanatosAnim>(me->GetMesh()->GetAnimInstance());
		Anim->AnimState = mState;
	}

}

void UKThanatosFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ����â�� ���� �޽��� ����ϱ�
	FString logMsg = UEnum::GetValueAsString(mState);
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Red, logMsg);
	

	switch (mState)
	{	
	case EThanatosState::Start: { State_Start(); } break;
	case EThanatosState::Idle: { State_Idle(); } break;
	case EThanatosState::Move: { State_Move(); } break;
	case EThanatosState::MoveFar: { State_MoveFar(); } break;
	case EThanatosState::Attack1: { State_Attack1(); } break;
	case EThanatosState::Attack2: { State_Attack2(); } break;
	}

}


void UKThanatosFSM::State_Start()
{
	currentTime += GetWorld()->DeltaTimeSeconds;

	if (currentTime > 3.0f)
	{
		//�̵� ���·� ��ȯ
		mState = EThanatosState::Idle;
		Anim->AnimState = mState;

		//��ȯ �� �ð� �ʱ�ȭ
		currentTime = 0.0f;
	}

}


void UKThanatosFSM::State_Idle()
{
	//�ð��� �帣�ٰ�
	currentTime += GetWorld()->DeltaTimeSeconds;

	//���� ��� �ð��� ��� �ð��� �ʰ��ߴٸ�
	if (currentTime > status.idleDelayTime)
	{
		//�̵� ���·� ��ȯ
		mState = EThanatosState::Move;
		Anim->AnimState = mState;

		//��ȯ �� �ð� �ʱ�ȭ
		currentTime = 0.0f;
	}

}

void UKThanatosFSM::State_Move()
{
	FVector destination = target_Enemy->GetActorLocation();
	FVector dir = destination - me->GetActorLocation();

	//1��(WalkingTime) ���� �Ŀ� 2��(IdleDelayTime) ����ϰ� ����
	currentTime += GetWorld()->DeltaTimeSeconds;

	if (dir.Size() > status.attackRange)
	{
		me->AddMovementInput(dir.GetSafeNormal());

		if (currentTime > status.walkingTime)
		{
			mState = EThanatosState::Idle;
			Anim->AnimState = mState;
			currentTime = 0.0f;
			//�Ÿ�üũ
		}
	}

	//(dir.Size() < status.attackRange)
	else 
	{
		//���� ���·� ��ȯ�ϰ� �ʹ�
		OnAttackProcess();

		currentTime = 0.0f;
	}
}
 

void UKThanatosFSM::State_MoveFar()
{
	FVector destination = target_Enemy->GetActorLocation();
	FVector dir = me->GetActorLocation() - destination;

	me->AddMovementInput(dir.GetSafeNormal());


	//1��(WalkingTime) ���� �Ŀ� 2��(IdleDelayTime) ����ϰ� ����
	currentTime += GetWorld()->DeltaTimeSeconds;

	if (currentTime > status.walkingTime)
	{
		mState = EThanatosState::Idle;
		Anim->AnimState = mState;
		currentTime = 0.0f;
	}
}



void UKThanatosFSM::State_Attack1()
{
	//���� �ð��� �� ���� ����

	//�ð��� �帣�ٰ�
	currentTime += GetWorld()->DeltaTimeSeconds;
	//���� �ð��̵Ǹ� ������ ����
	if (currentTime > status.attackDelayTime)
	{

		++skillCount;
		if (skillCount == maxSkillCount)
		{
			status.attackRange = 2000.0f;
		}
		
		currentTime = 0.0f;


		EndAttackProcess();
	}
}


void UKThanatosFSM::State_Attack2()
{
	currentTime += GetWorld()->DeltaTimeSeconds;
	

	//���� �ð��̵Ǹ� ������ ����
	if (currentTime > status.attackDelayTime)
	{
		GetWorld()->SpawnActor<AKDestroyBox>(boxPos, target_Enemy->GetTransform());
		//GetWorld()->SpawnActor<AKDestroyBox>(boxPos, FTransform(FRotator(0, 0, 0), FVector(2000, 2000, 2000), FVector(1.0f, 1.0f, 1.0f)));
		skillCount =0;
		status.attackRange = 1000.0f;

		currentTime = 0.0f;

		EndAttackProcess();
	}
}

void UKThanatosFSM::OnDamagedProcess()
{
	//�ݸ��� �ڽ� or �����ص� �ڽ��� ������ �ϱ�.
}

void UKThanatosFSM::OnAttackProcess()
{
		//IsAttack = true;
		Anim->bAttackPlay = true;

		if (skillCount >= maxSkillCount)
		{
			mState = EThanatosState::Attack2;
			Anim->AnimState = mState;
		}

		else
		{
			mState = EThanatosState::Attack1;
			Anim->AnimState = mState;

		}
}


void UKThanatosFSM::EndAttackProcess()
{
	//�׽�Ʈ�� ��Ʈ��.
	mState = EThanatosState::Idle;
	Anim->AnimState = mState;

	OnAttackEnd();

	/*
	if (��ų �ɸ��� ���� ���ʹ̰� ���� ��)
	{
		�� ������ ����.
	}

	else (���� ��Ʈ��)
	*/
}



void UKThanatosFSM::OnAttackEnd()
{
	Anim->bAttackPlay = false;
}


