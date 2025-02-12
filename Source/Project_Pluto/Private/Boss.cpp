// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "BossFSM.h"



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

#pragma region initStatSetting
	SetMaxHp(4400);

	SetDetectRange(10000.0f);

	SetAttRange(1000.0f);

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


