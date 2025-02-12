// Fill out your copyright notice in the Description page of Project Settings.


#include "PWBlade.h"
#include "EnemyInfo.h"
#include "PlayerZagreus.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

APWBlade::APWBlade()
{
	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());

	// �� ��� ����
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BladeMeshComp"));
	MeshComp->SetupAttachment(CollisionComp);

	ConstructorHelpers::FObjectFinder<UStaticMesh>TempMesh(TEXT("/Script/Engine.StaticMesh'/Game/RGY/Modelings/Blade/stygian_blade_-_hades.stygian_blade_-_hades'"));

	if (TempMesh.Succeeded()) {
		MeshComp->SetStaticMesh(TempMesh.Object);
		MeshComp->SetRelativeScale3D(FVector(0.08f));
		MeshComp->SetRelativeLocationAndRotation(FVector(51.0f, -3.0f, 140.0f), FRotator(-32.0f, -39.0f, 24.0f));
	}
	MeshComp->SetCollisionProfileName(TEXT("NoCollision"));

}

void APWBlade::BeginPlay()
{
	Super::BeginPlay();

	MaxCombo = 3;
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &APWBlade::OnBoxOverlap);
}

void APWBlade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APWBlade::Attack(AActor* OtherActor)
{
	if (OtherActor->IsA<AEnemyInfo>()) {
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

void APWBlade::SpecialAtt(AActor* OtherActor)
{
	if (OtherActor->IsA<AEnemyInfo>()) {
		AEnemyInfo* enemy = Cast<AEnemyInfo>(OtherActor);
		NovaSmash(enemy);
	}
}

// ���ݿ� atkBuff, ������ ���� (���� �� ����� Ȯ���� ����) ����

void APWBlade::Strike(AEnemyInfo* Enemy)
{
	// 20 �����
}

void APWBlade::Chop(AEnemyInfo* Enemy)
{
	// 25 �����
}

void APWBlade::Thrust(AEnemyInfo* Enemy)
{
	// 30 �����

	Knockback(Enemy);
}

void APWBlade::NovaSmash(AEnemyInfo* Enemy)
{
	// 50 �����

	BackstabBan(Enemy);
	Knockback(Enemy);
}

void APWBlade::Knockback(AEnemyInfo* Enemy)
{
	// �˹�
}

void APWBlade::BackstabBan(AEnemyInfo* Enemy)
{
	// �齺�� �Ұ�
}

void APWBlade::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	FHitResult BoxHit;

	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		FVector(5.f, 5.f, 5.f),
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,		//�����Ұ�
		EDrawDebugTrace::ForDuration,
		BoxHit,
		true		//�ڽŹ���
	);
}
