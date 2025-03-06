// Fill out your copyright notice in the Description page of Project Settings.


#include "KThanatosFSM.h"
#include "Kismet/GameplayStatics.h"
//#include "KEnemy.h"
#include "KThanatosAnim.h"
#include "KDestroyBox.h"
#include "Monster.h"
#include "KSkillBox.h"
#include "KCHeart.h"


UKThanatosFSM::UKThanatosFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


}

void UKThanatosFSM::BeginPlay()
{
	Super::BeginPlay();

	//GM = Cast<AHadesGameMode>(GetWorld()->GetAuthGameMode());

	// ...
	//���忡�� Enemy�� ã�ƿ���
	auto actor_Enemy = UGameplayStatics::GetActorOfClass(GetWorld(), ENEMYCLASS::StaticClass());

	//��ų 1�� üũ�� �� �Ǿ� �������� ��������.
	if (actor_Enemy != nullptr)
	{
		target_Enemy = Cast<ENEMYCLASS>(actor_Enemy);
	}


	auto auto_DestroyBox = UGameplayStatics::GetActorOfClass(GetWorld(), AKDestroyBox::StaticClass());
	if (auto_DestroyBox != nullptr)
	{
		destroyBox = Cast<AKDestroyBox>(auto_DestroyBox);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	auto auto_SkillBox = UGameplayStatics::GetActorOfClass(GetWorld(), AKSkillBox::StaticClass());
	if (auto_SkillBox != nullptr)
	{
		skillBox = Cast<AKSkillBox>(auto_SkillBox);
	}



	//���� ��ü ��������
	me = Cast<AKThanatos>(GetOwner());


	if (me)
	{
		Anim = Cast<UKThanatosAnim>(me->GetMesh()->GetAnimInstance());
		Anim->AnimState = mState;
	}

	bCanDestroySelf = false;

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
	case EThanatosState::Idle2: { State_Idle2(); } break;
	case EThanatosState::Move: { State_Move(); } break;
	case EThanatosState::MoveFar: { State_MoveFar(); } break;
	case EThanatosState::Attack1: { State_Attack1(); } break;
	case EThanatosState::Attack2: { State_Attack2(); } break;

	case EThanatosState::EndGame: { State_EndGame(); } break;
	}

	/*
	if (bCanAttack == false)
	{
		attackCurrent += DeltaTime;

		if (attackCurrent > status.attackDelayTime_1)
		{
			bCanAttack = true;
			attackCurrent = 0.f;
		}
	}
	*/

	if (bCanDestroySelf == true)
	{
		me->Destroy();
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
	SearchEnemy();


	if (endGameCount > 4)
	{
		mState = EThanatosState::EndGame;
		Anim->AnimState = mState;
	}

	//�ð��� �帣�ٰ�
	currentTime += GetWorld()->DeltaTimeSeconds;

	//���� ��� �ð��� ��� �ð��� �ʰ��ߴٸ�
	if (currentTime > status.idleDelayTime)
	{
		//���ʹ̰� ������ ���� or ��� Idle
		if (target_Enemy != nullptr)
		{
			//�̵� ���·� ��ȯ
			mState = EThanatosState::Move;
			Anim->AnimState = mState;

		}

		//��ȯ �� �ð� �ʱ�ȭ
		currentTime = 0.0f;
		endGameCount += 1;
	}


}


void UKThanatosFSM::State_Idle2()
{
	SearchEnemy();

	//�ð��� �帣�ٰ�
	currentTime += GetWorld()->DeltaTimeSeconds;

	//���� ��� �ð��� ��� �ð��� �ʰ��ߴٸ�
	if (currentTime > status.idleDelayTime2)
	{
		mState = EThanatosState::Idle;
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
		/*
		else if (skillCount< maxSkillCount && dir.Size() < status.attackRange - 200.f)
		{
			mState = EThanatosState::MoveFar;
			Anim->AnimState = mState;
			currentTime = 0.0f;

		}
		*/

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
	SearchEnemy();

	if (currentTime > status.attackDelayTime_1)
	{
		//if (target_Enemy != nullptr && target_Enemy->GetNowHp() > 0.f && 171.239894)
		if (target_Enemy != nullptr)
		{
			if (target_Enemy->GetNowHp() > 0.f)
			{
				me->SetSkillBox();

				//���� ��Ʈ - �� ����� �ٲ�
				//target_Enemy->SetbSoonDead();
				//target_Enemy->DamageToSelf(9999);

				me->destroyCount += 1;

				//UE_LOG(LogTemp, Error, TEXT("destroy : %d"), me->destroyCount);

				++skillCount;

			}
			
			target_Enemy->Destroy();
		}


		if (skillCount == maxSkillCount)
		{
			status.attackRange = 2000.0f;
		}

		EndAttackProcess();

	}

}


void UKThanatosFSM::State_Attack2()
{

	currentTime += GetWorld()->DeltaTimeSeconds;

	//���� �ð��̵Ǹ� ������ ����
	if (currentTime > status.attackDelayTime_2)
	{
		SearchEnemy();

		if (target_Enemy != nullptr && target_Enemy->GetNowHp() > 0.f)
		{
			skillCount = 0;

			status.attackRange = 1000.0f;

			currentTime = 0.0f;
			me->SetDestroyBox();
		
		}

		if (target_Enemy != nullptr)
		{
			target_Enemy->Destroy();
		}

		EndAttackProcess();

	}
}

/*
void UKThanatosFSM::State_Attack2()
{

	SearchEnemy();
	
	if (bCanAttack == true)
	{
		if (target_Enemy != nullptr && target_Enemy->GetNowHp() > 0.f)
		{
			skillCount = 0;

			status.attackRange = 1000.0f;

			currentTime = 0.0f;
			me->SetDestroyBox();


		}

		EndAttackProcess();
	}

}
*/


void UKThanatosFSM::OnDamagedProcess()
{
	//�ݸ��� �ڽ� or �����ص� �ڽ��� ������ �ϱ�.
}

void UKThanatosFSM::OnAttackProcess()
{
	FVector destination = target_Enemy->GetActorLocation();
	FVector dir = destination - me->GetActorLocation();

	FRotator TargetRotation = dir.Rotation();

	FRotator NewRotation = me->GetActorRotation();
	NewRotation.Yaw = TargetRotation.Yaw ;

	me->SetActorRotation(NewRotation);


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

		endGameCount = 0;
}


void UKThanatosFSM::EndAttackProcess()
{
	//bCanAttack = false;

	mState = EThanatosState::Idle2;
	Anim->AnimState = mState;

	//before_Target = target_Enemy;
	target_Enemy = nullptr;

	OnAttackEnd();

	endGameCount = 0;
	
}



void UKThanatosFSM::OnAttackEnd()
{
	Anim->bAttackPlay = false;
}


void UKThanatosFSM::SearchEnemy()
{
	auto auto_Enemy = UGameplayStatics::GetActorOfClass(GetWorld(), ENEMYCLASS::StaticClass());

	if (auto_Enemy != nullptr)
	{
		target_Enemy = Cast<ENEMYCLASS>(auto_Enemy);
		
		//while (target_Enemy == before_Target)
		//{
		//	target_Enemy = Cast<ENEMYCLASS>(auto_Enemy);
		//}
		
	}
}

//UGameplayStatics::GetAllActorsOfClass(GetWorld(), ENEMYCLASS::StaticClass(), target_Enemy_Array);
//TArray�� �޾ƺ����� �ߴµ� �̷� �ʹ� ���� ������ �迭 ������ �����ؾ� �ؼ� �̹��� ����߽��� ��
/*

void UKThanatosFSM::SearchEnemy()
{

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ENEMYCLASS::StaticClass(), target_Enemy_Array);
	//TArray
	if (target_Enemy_Array.Num() == 0)
	{
		target_Enemy = Cast<ENEMYCLASS>(target_Enemy_Array);

		//while (target_Enemy == before_Target)
		//{
		//	target_Enemy = Cast<ENEMYCLASS>(auto_Enemy);
		//}

	}
}

*/

FVector UKThanatosFSM::ReturnDest()
{
	FVector destination = target_Enemy->GetActorLocation();

	return destination;
}


void UKThanatosFSM::State_EndGame()
{
	
	FVector dir = lastPos - me->GetActorLocation();

	if (bEndGame == false)
	{
		if (dir.Size() < 5.f)
		{
			bEndGame = true;
		}

		else
		{
			me->AddMovementInput(dir.GetSafeNormal());
		}

	}

	else
	{
		auto auto_heartBox = UGameplayStatics::GetActorOfClass(GetWorld(), AKCHeart::StaticClass());

		if (auto_heartBox != nullptr)
		{
			
			
			//Ÿ���佺�� (���� ������ MAX 11����) 8���� �̻� ��Ҵٸ�
			if (me->destroyCount > 8)
			{
				//������ ���޵��� ����
				me->heartBox->Destroy();
				bCanDestroySelf = true;
			}

			//�� ���� ��쿡�� ���� ����
			else 
			{
				me->SetHeartBox();

			}

		}

	}

	
}

