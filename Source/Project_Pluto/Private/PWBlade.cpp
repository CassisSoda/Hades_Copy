// Fill out your copyright notice in the Description page of Project Settings.


#include "PWBlade.h"
#include "EnemyInfo.h"
#include "PlayerZagreus.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

APWBlade::APWBlade()
{
	// �ּ� ó�� : �浹�� Į�� �κп� �µ��� ���� ���. �÷��̾��� Ÿ�ݰ��� ���� �浹 ������ Ȯ �ø�

	CollisionComp->SetBoxExtent(FVector(50.0f, 50.0f, 100.0f));
	//CollisionComp->SetBoxExtent(FVector(10.0f, 10.0f, 60.0f));
	CollisionComp->SetRelativeLocation(FVector(-6.0f, 4.0f, 90.0f));
	//CollisionComp->SetRelativeLocation(FVector(-6.0f, 4.0f, 105.0f));

	// �� ����
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BladeMeshComp"));
	MeshComp->SetupAttachment(CollisionComp);

	ConstructorHelpers::FObjectFinder<UStaticMesh>TempMesh(TEXT("/Script/Engine.StaticMesh'/Game/RGY/Modelings/Blade/stygian_blade_-_hades.stygian_blade_-_hades'"));

	if (TempMesh.Succeeded()) {
		MeshComp->SetStaticMesh(TempMesh.Object);
		MeshComp->SetRelativeScale3D(FVector(0.08f));
		MeshComp->SetRelativeLocationAndRotation(FVector(45.0f, 45.0f, 50.0f), FRotator(-23.0f, -23.0f, 36.0f));
		//MeshComp->SetRelativeLocationAndRotation(FVector(45.0f, 45.0f, 25.0f), FRotator(-23.0f, -23.0f, 36.0f));
	}
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// ����� ���� ����
	EffectCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("EffectCollisionComp"));
	EffectCollisionComp->SetupAttachment(RootComp);
	EffectCollisionComp->SetSphereRadius(500.0f);
	EffectCollisionComp->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CollisionComp->SetCollisionObjectType(ECC_Pawn);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	EffectCollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EffectCollisionComp->SetCollisionObjectType(ECC_Pawn);
	EffectCollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	EffectCollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void APWBlade::BeginPlay()
{
	Super::BeginPlay();

	MaxCombo = 3;
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &APWBlade::OnBoxOverlap);
	EffectCollisionComp->OnComponentBeginOverlap.AddDynamic(this, &APWBlade::OnEffectOverlap);
}

void APWBlade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APWBlade::AttackProcess(AActor* OtherActor)
{
	// EnemyInfo �� ����� Ÿ������ Ȯ�� ��. -> EnemyInfo ���忡�� PlayerWeapon overlap ���� �ٲٴ°� ���� ����!!!
	if (OtherActor->IsA<AEnemyInfo>()) {
		UE_LOG(LogTemp, Warning, TEXT("Attack to Enemy!"));
		AEnemyInfo* enemy = Cast<AEnemyInfo>(OtherActor);

		// ���° �޺��ΰ��� ���� ���� ����
		switch (player->Combo)
		{
		case 1:
			Strike(enemy);
			break;
		case 2:
			Chop(enemy);
			break;
		case 3:
			Thrust(enemy);
			break;
		default:
			break;
		}
	}
}

void APWBlade::SpecialAttProcess(AActor* OtherActor)
{
	if (OtherActor->IsA<AEnemyInfo>()) {
		UE_LOG(LogTemp, Warning, TEXT("Special Attack to Enemy!"));
		AEnemyInfo* enemy = Cast<AEnemyInfo>(OtherActor);
		NovaSmash(enemy);
	}
}

void APWBlade::StartAttack()
{
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void APWBlade::EndAttack()
{
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void APWBlade::StartSpecialAtt()
{
	EffectCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void APWBlade::EndSpecialAtt()
{
	EffectCollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// ���ݿ� atkBuff, ������ ���� (���� �� ����� Ȯ���� ����) ����

void APWBlade::Strike(AEnemyInfo* Enemy)
{
	// 20 �����
	int32 damage = 20;

	UGameplayStatics::ApplyDamage(Enemy, damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
}

void APWBlade::Chop(AEnemyInfo* Enemy)
{
	// 25 �����
	int32 damage = 25;

	UGameplayStatics::ApplyDamage(Enemy, damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
}

void APWBlade::Thrust(AEnemyInfo* Enemy)
{
	// 30 �����
	int32 damage = 30;

	UGameplayStatics::ApplyDamage(Enemy, damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());

	FVector direction = FVector(player->AttackDirection.X, player->AttackDirection.Y, 0.0f);
	Knockback(Enemy, direction);
	player->LaunchCharacter(direction.GetSafeNormal() * 3000, false, false);
}

void APWBlade::NovaSmash(AEnemyInfo* Enemy)
{
	// 50 �����
	int32 damage = 50;

	UGameplayStatics::ApplyDamage(Enemy, damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());

	FVector direction = Enemy->GetActorLocation() - this->GetActorLocation();
	direction = FVector(direction.X, direction.Y, 0.0f);
	BackstabBan(Enemy);
	Knockback(Enemy, direction);
}

void APWBlade::Knockback(AEnemyInfo* Enemy, FVector dir)
{
	// �˹�
	Enemy->LaunchCharacter(dir.GetSafeNormal() * 3000, false, false);
}

void APWBlade::BackstabBan(AEnemyInfo* Enemy)
{
	// �齺�� �Ұ� -> ���� ���� ����� ���� X
}

void APWBlade::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	switch (player->NowState)
	{
	case EPlayerBehaviorState::Attack:
		AttackProcess(OtherActor);
		break;
	default:
		break;
	}
}

void APWBlade::OnEffectOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	switch (player->NowState)
	{
	case EPlayerBehaviorState::SpecialAtt:
		if(player->bSpecialAtt) SpecialAttProcess(OtherActor);
		break;
	default:
		break;
	}
}
