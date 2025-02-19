// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "BossFSM.h"
#include "CurtainFireProjectile.h"
#include "Components/ArrowComponent.h"
#include "DebugMacro.h"
#include "Kismet/KismetMathLibrary.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h"
#include "../../../../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABoss::ABoss()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/MW/Character/ParagonMorigesh/Characters/Heroes/Morigesh/Skins/DarkHeart/Meshes/Morigesh_DarkHeart.Morigesh_DarkHeart'"));

	// ���� ���� �бⰡ �����ߴٸ�,
	if (TempMesh.Succeeded())
	{
		// �ε��� �޽��� �־��ְ� �ʹ�.
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		// ��ġ���� ȸ������ �ݿ��ϰ� �ʹ�.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
	}

	ConstructorHelpers::FClassFinder<UAnimInstance> tempClass(TEXT("/Script/Engine.AnimBlueprint'/Game/MW/Blueprints/ABP_Boss.ABP_Boss_C'"));
	if (tempClass.Succeeded ())
	{
		GetMesh()->SetAnimInstanceClass(tempClass.Class);
	}
	
	// Boss Component �߰�
	fsm = CreateDefaultSubobject<UBossFSM>(TEXT("BossFSM"));

	ArrowComp = CreateDefaultSubobject <UArrowComponent>(TEXT("ArrowComp"));
	ArrowComp->SetupAttachment(RootComponent);
	ArrowComp->SetRelativeLocation(FVector(40.f, 0.f, 30.f));

#pragma region initStatSetting
	SetMaxHp(4400);

	SetDetectRange(10000.0f); //������ 10000.f - ������ 

	SetAttRange(1000.0f);

	SetDamage(0);

	//�⺻ ���������� Charge �߰�
	AttPatterns.Add(EAttackType::Charge);

#pragma endregion initSetting

}

// Called when the game starts or when spawned
void ABoss::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABoss::SelectPattern(int32 idx)
{
	switch (idx)
	{
		case 0:
			{
				AttTypeEnum = EAttackType::Charge;
				SetDamage(AttDamages[idx]);
				SetAttRange(AttRanges[idx]);
				break;
			}
		case 1:
			{
				AttTypeEnum = EAttackType::CurtainFire;
				SetDamage(AttDamages[idx]);
				SetAttRange(AttRanges[idx]);
				break;
			}
		case 2:
			{
				AttTypeEnum = EAttackType::Plate;
				SetDamage(AttDamages[idx]);
				SetAttRange(AttRanges[idx]);
				break;
			}

	}
}

void ABoss::AddAttPatterns()
{
	float percent = (float)GetNowHp() / GetMaxHp();

	if (percent <= 0.75f && percent > 0.5f)
	{
		AttPatterns.Add(EAttackType::CurtainFire);
	}


	if (percent <= 0.25f && percent > 0.f)
	{
		AttPatterns.Add(EAttackType::Plate);
	}
}

void ABoss::AttackPlayer(EAttackType type)
{
	switch (type)
	{
	case EAttackType::Charge:			{ Charge();			break; }
	case EAttackType::CurtainFire:		{ CurtainFire();		break; }
	case EAttackType::Plate:			{ Plate();			break; }
	}
}

void ABoss::Charge()
{
	FVector dir = GetTargetFromMe();
	dir.Normalize();
	this->LaunchCharacter(dir * 10000.f , true, false);

}

void ABoss::CurtainFire()
{

	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABoss::SpawnProjectile, 0.4f, true, 4.f);

}

void ABoss::SpawnProjectile()
{
	if (fireCounter >= 10)
	{
		//10�� ��� Ÿ�̸� ����
		GetWorldTimerManager().ClearTimer(TimerHandle);
		fireCounter = 0;
		return;
	}

	FActorSpawnParameters spawnParams;
	spawnParams.bNoFail = true;
	spawnParams.Owner = this;

	//���� ���� �÷��̾ �ٶ󺸰� �ϰ�ʹ�.
	APlayerController* controller = GetWorld()->GetFirstPlayerController();
	FVector playerLocation = controller->GetPawn()->GetActorLocation();
	FRotator BaseRotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation (), playerLocation);
	SetActorRotation(BaseRotation);

	// �߾� ���� ����
	float BaseAngle = BaseRotation.Yaw; 
	// �¿�� ������ ���� ����
	float AngleOffset = 15.0f; 

	for (int32 i = -2; i < 3; i++)
	{
		float angle = BaseAngle + (i * AngleOffset);
		FRotator fireAngle = FRotator(0.f, angle, 0.f);

		ACurtainFireProjectile* projectile = GetWorld()->SpawnActor <ACurtainFireProjectile>(ProjectileFactory, ArrowComp->GetComponentTransform(), spawnParams);

		
		projectile->SetVelocity(fireAngle.Vector ());
	}
	fireCounter++;

}



void ABoss::Plate()
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ABoss::SpawnPlate, 2.f, true);
}




void ABoss::SpawnPlate()
{
	if (plateCounter >= 5)
	{
		GetWorldTimerManager().ClearTimer(TimerHandle);
		plateCounter = 0;
		return;
	}

	APawn* pawn = GetWorld()->GetFirstPlayerController()->GetPawn ();
	FVector playerLocation = pawn->GetActorLocation();

	int32 randomPattern = FMath::RandRange(0, 1);

	// ���������� ã�Ƽ�
	if (randomPattern == 0)
	{
		FVector expectPos = playerLocation + pawn->GetVelocity() * 0.5f;
		TArray<FVector> spawnLocation;
		spawnLocation.Add(FVector(expectPos.X - dxdyRange, expectPos.Y, 0.f));
		spawnLocation.Add(FVector(expectPos.X + dxdyRange, expectPos.Y, 0.f));
		spawnLocation.Add(FVector(expectPos.X, expectPos.Y - dxdyRange, 0.f));
		spawnLocation.Add(FVector(expectPos.X, expectPos.Y + dxdyRange, 0.f));

		
		int32 randSpawn = FMath::RandRange(0, 3);

		GetWorld()->SpawnActor <APlateActor>(PlateFactory,FVector(playerLocation.X, playerLocation.Y, 0.f),		FRotator::ZeroRotator);


		for (int32 i = 0; i < 3; i++)
		{	
			FVector twistedLocation = GetRandomPos (spawnLocation[index[randSpawn][i]]);
			GetWorld()->SpawnActor <APlateActor>(PlateFactory,twistedLocation, FRotator(0.f));

		}

	}

	// ������ ���ִ°� ��������
	else
	{
		TArray<FVector> spawnLocation;
		spawnLocation.Add(FVector(playerLocation.X - dxdyRange, playerLocation.Y, 0.f));
		spawnLocation.Add(FVector(playerLocation.X + dxdyRange, playerLocation.Y, 0.f));
		spawnLocation.Add(FVector(playerLocation.X, playerLocation.Y - dxdyRange, 0.f));
		spawnLocation.Add(FVector(playerLocation.X, playerLocation.Y + dxdyRange, 0.f));

		for (auto point : spawnLocation)
		{
			GetWorld()->SpawnActor<APlateActor>(PlateFactory, point, FRotator(0.f));
		}

	}

	
	plateCounter++;

}

FVector ABoss::GetRandomPos(FVector pos)
{
	float randAngle = FMath::RandRange(0.f, 2.f * PI);
	float randRadius = FMath::RandRange(minRange, maxRange);

	float new_x = pos.X + FMath::Cos(randAngle) * randRadius;
	float new_y = pos.Y + FMath::Sin(randAngle) * randRadius;

	return FVector(new_x, new_y, pos.Z);
}

void ABoss::SelectCharge()
{
	AttTypeEnum = EAttackType::Charge;
	SetDamage(AttDamages[0]);
	SetAttRange(AttRanges[0]);

	fsm->BossAttDamage = GetDamage();
	fsm->BossAttRange = GetAttRange();
}

void ABoss::SelectCurtainFire()
{
	AttTypeEnum = EAttackType::CurtainFire;
	SetDamage(AttDamages[1]);
	SetAttRange(AttRanges[1]);

	fsm->BossAttDamage = GetDamage();
	fsm->BossAttRange = GetAttRange();
}

void ABoss::SelectPlate()
{
	AttTypeEnum = EAttackType::Plate;
	SetDamage(AttDamages[2]);
	SetAttRange(AttRanges[2]);

	fsm->BossAttDamage = GetDamage();
	fsm->BossAttRange = GetAttRange();

}


