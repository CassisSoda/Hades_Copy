// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterFSM.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerZagreus.h"
#include "Monster.h"
#include "Components/CapsuleComponent.h"
#include "MonsterAnimInstance.h"
#include "Runtime/AIModule/Classes/AIController.h"
#include "NavigationSystem.h"
#include "Runtime/AIModule/Classes/Navigation/PathFollowingComponent.h"
#include "DebugMacro.h"

// Sets default values for this component's properties
UMonsterFSM::UMonsterFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMonsterFSM::BeginPlay()
{
	Super::BeginPlay();

	// ...

	AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerZagreus::StaticClass());
	if (actor)
		target = Cast<APlayerZagreus>(actor);

	me = Cast<AMonster>(GetOwner());

	if (me)
	{
		Anim = Cast<UMonsterAnimInstance>(me->GetMesh ()->GetAnimInstance ()); 
		Anim->MonsterFSM = this;
	}
	
	AI = Cast <AAIController>(me->GetController ());

	
}


// Called every frame
void UMonsterFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	FString logMsg = UEnum::GetValueAsString(mState);
	Debug::Print(logMsg);

	switch (mState)
	{
	case EMonsterState::Idle: { State_Idle(); }	break;
	case EMonsterState::Move: { State_Move(); }	break;
	case EMonsterState::Attack_Start: { State_Attack_Start(); }	break;
	case EMonsterState::Attack_End: { State_Attack_End(); }	break;
	case EMonsterState::Damage: { State_Damage(); }	break;
	case EMonsterState::Die: { State_Die(); }	break;
	}

}

void UMonsterFSM::State_Idle()
{
	currentTime += GetWorld()->DeltaTimeSeconds;


	if (me->GetTargetFromMe ().Size() <= me->GetDetectRange() * 10.f)
	{
		//���ο� ���� ��ġ ��������
		GetRandomPosInNavMesh(me->GetActorLocation(), me->GetAttRange(), randomPos);

		// �̵� ���·� �����ϰ� �ʹ�.
		mState = EMonsterState::Move;

		Anim->AnimState = mState;

		
	}

}

void UMonsterFSM::State_Move()
{
	if (!target || !me)	return;

	FVector destination = target->GetActorLocation();

	FVector dir = destination - me->GetActorLocation();

	//me->AddMovementInput(dir);

	auto ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());

	// ������ ��ã�� ��� ������ �˻�
	FPathFindingQuery query;
	FAIMoveRequest req;

	// ���������� ������ �� �ִ� ����
	req.SetAcceptanceRadius(3.f);
	req.SetGoalLocation(dir);

	// ��ã�� ���� ���� ����
	AI->BuildPathfindingQuery(req, query);

	// ��ã�� ����� ��������
	FPathFindingResult result = ns->FindPathSync(query);

	// ���������� �� ã�� ���� ���� Ȯ��
	if (result.Result == ENavigationQueryResult::Success)
	{
		//Ÿ������ �̵�
		AI->MoveToLocation(dir);
	}
	else
	{
		//���� ��ġ�� �̵�
		auto randResult =	AI->MoveToLocation(randomPos);

		//�������� �����ϸ�
		if (randResult == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			//���ο� ���� ��ġ ��������
			GetRandomPosInNavMesh(me->GetActorLocation(), me->GetAttRange (), randomPos);
		}

	}
	
	
	//AI->MoveToLocation(dir);

	if (dir.Size() <= me->GetAttRange())
	{
		mState = EMonsterState::Attack_Start;

		Anim->AnimState = mState;

		currentTime = attackDelayTime - 0.5f;

	}
	
	

}

void UMonsterFSM::State_Attack_Start()
{
	if (Anim->bAttackPlay == true) return;
	
	AI->StopMovement ();
	currentTime += GetWorld()->DeltaTimeSeconds;

	if (currentTime > attackDelayTime)
	{
		me->CurtainFire ();
		Anim->bAttackPlay = true;

		currentTime = 0.f;
	}

	float dist = FVector::Dist(me->GetActorLocation(), target->GetActorLocation());

	if (dist > me->GetAttRange())
	{
		mState = EMonsterState::Move;

		Anim->AnimState = mState;
	}

}

void UMonsterFSM::State_Attack_End()
{

	currentTime += GetWorld ()->DeltaTimeSeconds;

	if (currentTime > attackDelayTime)
	{
		mState = EMonsterState::Idle;
		Anim->AnimState = mState;
		currentTime = 0.f;

		//���ο� ���� ��ġ ��������
		GetRandomPosInNavMesh(me->GetActorLocation(), 500.f, randomPos);
	}

}


void UMonsterFSM::State_Damage()
{
	AI->StopMovement ();

	currentTime += GetWorld()->DeltaTimeSeconds;

	if (currentTime > damageDelayTime)
	{
		mState = EMonsterState::Idle;
		currentTime = 0.f;

		Anim->AnimState = mState;
	}
}

void UMonsterFSM::State_Die()
{
	if(!bDieDone) return;

	FVector p0 = me->GetActorLocation();
	FVector vt = FVector::DownVector * disapperSpeed * GetWorld()->DeltaTimeSeconds;
	FVector p = p0 + vt;
	me->SetActorLocation(p);

	if (p.Z < -100.f)
	{
		me->Destroy();
	}

}

void UMonsterFSM::OnDamageProcess()
{
	if (me->GetNowHp() > 0.f)
	{
		mState = EMonsterState::Damage;

	}
	else
	{
		mState = EMonsterState::Die;
		me->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//Anim->Montage_Play (Anim->MonsterMontage);
		me->PlayAnimMontage (Anim->MonsterMontage);
		
	}

	Anim->AnimState = mState;
}

bool UMonsterFSM::GetRandomPosInNavMesh(FVector centerLocation, float radius, FVector& dest)
{
	auto ns = UNavigationSystemV1::GetNavigationSystem (GetWorld ());

	FNavLocation loc;

	bool result = ns->GetRandomReachablePointInRadius(centerLocation, radius, loc);

	dest = loc.Location;
	return result;
}

