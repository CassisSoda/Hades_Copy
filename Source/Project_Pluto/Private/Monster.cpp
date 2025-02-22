// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"
#include "Engine/SkeletalMesh.h"
#include "MonsterFSM.h"
#include "MonsterAnimInstance.h"
#include "Kismet/KismetMathLibrary.h"
#include "CurtainFireProjectile.h"
#include "Components/ArrowComponent.h"

AMonster::AMonster()
{
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/MW/Enemy/Model/vampire_a_lusth.vampire_a_lusth'"));

	// ���� ���� �бⰡ �����ߴٸ�,
	if (TempMesh.Succeeded())
	{
		// �ε��� �޽��� �־��ְ� �ʹ�.
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		// ��ġ���� ȸ������ �ݿ��ϰ� �ʹ�.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));

		//�޽� ũ�� ����
		GetMesh()->SetRelativeScale3D(FVector(0.8f));
	}

	FSM = CreateDefaultSubobject<UMonsterFSM>(TEXT("FSM"));

	ConstructorHelpers::FClassFinder<UMonsterAnimInstance> tmep(TEXT("/Script/Engine.AnimBlueprint'/Game/MW/Blueprints/ABP_Monster.ABP_Monster'"));

	if (tmep.Succeeded())
	{
		GetMesh ()->SetAnimInstanceClass (tmep.Class);
	}

	ArrowComp = CreateDefaultSubobject <UArrowComponent>(TEXT("ArrowComp"));
	ArrowComp->SetupAttachment(RootComponent);
	ArrowComp->SetRelativeLocation(FVector(40.f, 0.f, 30.f));
	
	SetMaxHp(40.f);
	SetNowHp(GetMaxHp());
	SetDetectRange(1200.f);
	SetAttRange(800.f);
	SetDamage(3.f);
	SetAttType(EAttackType::CurtainFire);


}

void AMonster::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AMonster::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	DamageToSelf(DamageAmount);
	UE_LOG(LogTemp, Error, TEXT("Player Hit Monster"));

	FSM->OnDamageProcess();

	return DamageAmount;
}


void AMonster::CurtainFire()
{
	BaseRotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), GetWorld ()->GetFirstPlayerController()->GetPawn ()->GetActorLocation ());
	SetActorRotation(BaseRotation);

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AMonster::SpawnProjectile, 0.2f, true, 2.65f);
	

}

void AMonster::SpawnProjectile()
{
	if (fireCounter >= 2 || FSM->mState == EMonsterState::Die)
	{
		//4�� ��� Ÿ�̸� ����
		GetWorldTimerManager().ClearTimer(TimerHandle);
		fireCounter = 1;
		return;
	}

	GetWorld()->GetTimerManager().SetTimer(SingleTimer, this, &AMonster::ShootSingle, 0.1f, true);

}

void AMonster::ShootSingle()
{

	if (fireCounter >= 5 || FSM->mState == EMonsterState::Die)
	{
		//4�� ��� Ÿ�̸� ����
		GetWorldTimerManager().ClearTimer(SingleTimer);
		fireCounter = 1;
		return;
	}

	FActorSpawnParameters spawnParams;
	spawnParams.bNoFail = true;
	spawnParams.Owner = this;

	//���� ���� �÷��̾ �ٶ󺸰� �ϰ�ʹ�.
	APlayerController* controller = GetWorld()->GetFirstPlayerController();
	FVector playerLocation = controller->GetPawn()->GetActorLocation();
	BaseRotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), playerLocation);
	SetActorRotation(BaseRotation);

	// �߾� ���� ����
	float BaseAngle = BaseRotation.Yaw;
	// �¿�� ������ ���� ����
	float AngleOffset = 10.0f;

	fireCounter < 2 ? AngleOffset *= -1 : AngleOffset = FMath::Abs(AngleOffset);

	

	float angle = BaseAngle + (fireCounter * AngleOffset);
	FRotator fireAngle = FRotator(0.f, angle, 0.f);

	ACurtainFireProjectile* projectile = GetWorld()->SpawnActor <ACurtainFireProjectile>(ProjectileFactory, ArrowComp->GetComponentTransform(), spawnParams);

	UE_LOG(LogTemp, Error, TEXT("Call CurtainFire"));
	projectile->SetVelocity(fireAngle.Vector());
	fireCounter++;


}
