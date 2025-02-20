// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerZagreus.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PWBlade.h"
#include "PlayerAnimInstance.h"

// Sets default values
APlayerZagreus::APlayerZagreus()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// �÷��̾� �޽� ���� �� �ʱ� ��ġ ���� ����
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/RGY/Modelings/ParagonGreystone/Characters/Heroes/Greystone/Meshes/Greystone.Greystone'"));

	if (TempMesh.Succeeded()) {
		GetMesh()->SetSkeletalMesh(TempMesh.Object);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("PlayerZagreus SkeletalMesh loding fail."));
	}
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// �÷��̾ �̵� �������� ȸ���ϵ���
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	// �ִϸ��̼� Ŭ���� �Ҵ�
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	ConstructorHelpers::FClassFinder<UAnimInstance> TempAnim(TEXT("/Game/RGY/Blueprints/ABP_PlayerZagreus.ABP_PlayerZagreus_C"));

	if (TempAnim.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(TempAnim.Class);
	}


	// �ʱ� ī�޶� ����
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	springArmComp->SetupAttachment(RootComponent);
	springArmComp->SetRelativeLocation(FVector(0.0f));
	springArmComp->TargetArmLength = 1200.0f;
	springArmComp->bDoCollisionTest = false;
	springArmComp->SetWorldRotation(FRotator(-50.0f, -30.0f, 0.0f));

	camComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CamComp"));
	camComp->SetupAttachment(springArmComp);

	GetCapsuleComponent()->SetCollisionObjectType(ECC_Pawn);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

// Called when the game starts or when spawned
void APlayerZagreus::BeginPlay()
{
	Super::BeginPlay();

	// Input �� ��Ʈ�ѷ� ����
	pController = Cast<APlayerController>(Controller);
	if (pController) {
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pController->GetLocalPlayer());

		if (subsystem) {
			subsystem->AddMappingContext(IMC_Player, 0);
		}
	}

	// ���� ��ȯ ����
	{
		FName WeaponSocket(TEXT("muzzle_01")); // FX_weapon
		FTransform weaponPosition = GetMesh()->GetSocketTransform(TEXT("muzzle_01"));
		auto CurrentWeapon = GetWorld()->SpawnActor<APWBlade>(FVector::ZeroVector, FRotator::ZeroRotator);
		if (CurrentWeapon != nullptr) {
			weapon = CurrentWeapon;
			weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("FX_weapon"));
		}
	}

	// �ִϸ��̼� Instance ��������
	AnimInstance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());

	// ĳ���� �ʱ� �̵� ���ǵ� ����
	Speed = RunSpeed;
}

// Called every frame
void APlayerZagreus::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// �ǰ� �ִϸ��̼� ���� ��
	if(bDamaged) return;

	// �ٴڿ� �� �ڰ� Q ��ų ���� ~ ���� ���� �� ������ ������ �ƹ� �͵� ���� ���Ѵ�.
	if(bSpecialAtt) {
		// ���׳��� ���
		if (NowState != EPlayerBehaviorState::SpecialAtt) {
			// Q ��ų ȿ�� ����
			weapon->EndSpecialAtt();
			bSpecialAtt = false;
			bForceSpecialAtt = false;
			if (Speed == SpecialAttRunSpeed) {
				Speed = RunSpeed;
			}
		}
		return;
	}

	// ȸ�� ���� ���� (��� �ν�����)������ ��� ���� ��ǲ �� ��ٷ� ��
	if (bDodgeAttackWait) {
		CheckDodgeAttackInput(DeltaTime);
	}

	// ȸ�� �� ���� ȸ�� �ȵǵ��� Delay Ÿ�� üũ
	if (bDodgeDelayWait) {
		CheckDodgeDelay(DeltaTime);
	}

	// �÷��̾ Move �����ε� �������� ���� ��� Idle ���·� ����
	if (PlayerDir == FVector::ZeroVector && NowState == EPlayerBehaviorState::Move) {
		NowState = EPlayerBehaviorState::Idle;
	}

	// �÷��̾� �̵�
	{
		if(NowState == EPlayerBehaviorState::Move || NowState == EPlayerBehaviorState::Dodge || NowState == EPlayerBehaviorState::SpecialAtt) {
			if (PlayerDir == FVector::ZeroVector) {
				if (pController != nullptr) {
					PlayerDir = pController->GetPawn()->GetActorForwardVector();
				}
			}

			if (NowState != EPlayerBehaviorState::SpecialAtt && Speed == SpecialAttRunSpeed) {
				if(NowState == EPlayerBehaviorState::Dodge) Speed = DodgeSpeed;
				else Speed = RunSpeed;
			}

			SetActorRotation(FRotator(0.0f, UKismetMathLibrary::ClampAxis(PlayerDir.Rotation().Yaw), 0.0f));

			SetActorLocation(GetActorLocation() + PlayerDir.GetSafeNormal() * Speed * DeltaTime, true);

			PlayerDir = FVector::ZeroVector;
		}
	}

	{
		if (NowState == EPlayerBehaviorState::Dodge) { // �켱���� �ְ� ȸ��
			CurrentAnimTime += DeltaTime;
			if (CurrentAnimTime >= AnimWaitTime) {
				CurrentAnimTime = 0.0f;
				EndDodge();
			}
		}
		else {
			if (bAttackProcess) { // ���� ���̸� ���� �ִϸ��̼� ���
				// GEngine->AddOnScreenDebugMessage(0, 1, FColor::Green, FString::Printf(TEXT("Combo : %d"), Combo));

				CurrentAnimTime += DeltaTime;
				if (CurrentAnimTime >= AnimWaitTime) {
					CurrentAnimTime = 0.0f;
					AttackProcess();
				}
			}
			else if (bReserveAttack) { // ���� ���� �ƴϰ� ���� ������ ��������� ���� ���μ��� ����
				weapon->StartAttack();
				AttackProcess();
			}
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Green, FString::Printf(TEXT("Player State : %s"), *UEnum::GetValueAsString(NowState)));
	GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("Player Collision : %s"), *UEnum::GetValueAsString(GetCapsuleComponent()->GetCollisionResponseToChannel(ECC_Pawn))));
}

// Called to bind functionality to input
void APlayerZagreus::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto PlayerInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (PlayerInput) {
		PlayerInput->BindAction(IA_Move, ETriggerEvent::Triggered, this, &APlayerZagreus::Move);
		PlayerInput->BindAction(IA_Attack, ETriggerEvent::Started, this, &APlayerZagreus::Attack);
		PlayerInput->BindAction(IA_Dodge, ETriggerEvent::Started, this, &APlayerZagreus::Dodge);
		PlayerInput->BindAction(IA_SpecialAtt, ETriggerEvent::Started, this, &APlayerZagreus::SpecialAtt);
		//PlayerInput->BindAction(IA_Spell, ETriggerEvent::Started, this, &APlayerZagreus::Spell);
		//PlayerInput->BindAction(IA_Interaction, ETriggerEvent::Started, this, &APlayerZagreus::Interaction);
		
		PlayerInput->BindAction(IA_CheatInvincible, ETriggerEvent::Started, this, &APlayerZagreus::CheatInvincible);
	}
}

void APlayerZagreus::NotifyActorBeginOverlap(AActor* OtherActor)
{
	// ���� ���� �� ���� �ε����� ������ �����

}

void APlayerZagreus::CheckDodgeDelay(float DeltaTime)
{
	CurrentDodgeDelayWait += DeltaTime;

	if (CurrentDodgeDelayWait >= DodgeDelayTime) {
		bDodgeDelayWait = false;
		CurrentDodgeDelayWait = 0.0f;
	}
}

void APlayerZagreus::CheckDodgeAttackInput(float DeltaTime)
{
	CurrentDodgeAttackWait += DeltaTime;

	if ((NowState != EPlayerBehaviorState::Idle && NowState != EPlayerBehaviorState::Move) || CurrentDodgeAttackWait >= DodgeAttackTime) {
		bDodgeAttackWait = false;
		CurrentDodgeAttackWait = 0.0f;
		return;
	}

	if (bReserveAttack) { // �� �κ� ������ Ȯ�� �����ϰ� ��� ��Ʈ����ũ �����ڵ�� ����
		Combo = weapon->MaxCombo - 1;
		bDodgeAttackWait = false;
		CurrentDodgeAttackWait = 0.0f;
		AttackProcess();
	}
}

void APlayerZagreus::EndDodge()
{
	Speed = RunSpeed;
	// ���ʹ̿� �浹 Overlap ���� ����
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	if (NowState == EPlayerBehaviorState::Dodge) {
		NowState = EPlayerBehaviorState::Idle;
	}

	AnimWaitTime = DefaultAnimWaitTime;

	bDodgeAttackWait = true;
	CurrentDodgeAttackWait = 0.0f;

	bDodgeDelayWait = true;
	CurrentDodgeDelayWait = 0.0f;

	if (bForceSpecialAtt) {
		StartSpecialAtt();
		return;
	}

	if (bReserveAttack) { // �� �κ� ������ Ȯ�� �����ϰ� ��� ��Ʈ����ũ �����ڵ�� ����
		Combo = weapon->MaxCombo - 1;
		AttackProcess();
	}
}

void APlayerZagreus::SetAttackDir()
{
	auto PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController) {
		FHitResult hit;
		PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, hit);

		MouseLocation = hit.Location;

		AttackDirection = MouseLocation - GetActorLocation();
		AttackDirection = FVector(AttackDirection.X, AttackDirection.Y, 0.0f); // Z �� ���� Ȥ���� �� ������ ó��
		SetActorRotation(FRotator(0.0f, UKismetMathLibrary::ClampAxis(AttackDirection.Rotation().Yaw), 0.0f));
	}
}

void APlayerZagreus::AttackProcess()
{
	if(bSpecialAtt || !CheckChangeStateEnabled(EPlayerBehaviorState::Attack)) return;
	Speed = RunSpeed;

	if(!bAttackProcess) { // ���� ����
	
		bAttackProcess = true;
		bReserveAttack = false;
		SetAttackDir();

		Combo++;
		CurrentAnimTime = 0.0f;

		if (Combo > weapon->MaxCombo) {
			Combo = 1;
		}

		if (NowState != EPlayerBehaviorState::Attack) {
			NowState = EPlayerBehaviorState::Attack;
		}
	}
	else { // ���� �ִϸ��̼� ���� �� ����
		if (!bReserveAttack) {
			Combo = 0;
			if (NowState == EPlayerBehaviorState::Attack) {
				NowState = EPlayerBehaviorState::Idle;
			}
			weapon->EndAttack();
		}
		
		bAttackProcess = false;
	}
}

float APlayerZagreus::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if(HP == 0) return 0;

	if (GetCapsuleComponent()->GetCollisionResponseToChannel(ECC_Pawn) == ECR_Ignore) {
		return HP;
	}

	HP = FMath::Clamp(HP-Damage, 0.f, (float)(MaxHP));

	if(NowState != EPlayerBehaviorState::Damaged && CheckChangeStateEnabled(EPlayerBehaviorState::Damaged)) {
		NowState = EPlayerBehaviorState::Damaged;
	}

	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, FString::Printf(TEXT("Player HP : %d"), HP));

	return HP;
}

void APlayerZagreus::StartSpecialAtt()
{
	if (NowState != EPlayerBehaviorState::SpecialAtt && CheckChangeStateEnabled(EPlayerBehaviorState::SpecialAtt)) {
		NowState = EPlayerBehaviorState::SpecialAtt;
		Speed = SpecialAttRunSpeed;
		bForceSpecialAtt = false;
		bSpecialAtt = false;
	}
}

void APlayerZagreus::SetPermanetBuff()
{
	MaxHP = InitHP + FMath::Floor(InitHP * PlusHP);
}

void APlayerZagreus::SetBuffMaxHP(int32 plusHpAbs, float plusHpPro)
{
	// ���� HP �� �ִ� HP ���濡 ���� ����. ������ ��� ���� Ȯ�� �ʿ�

	MaxHP += plusHpAbs;
	HP += plusHpAbs;

	MaxHP += FMath::Floor(MaxHP * plusHpPro);
	HP += FMath::Floor(HP * plusHpPro);
}

bool APlayerZagreus::CheckChangeStateEnabled(EPlayerBehaviorState state)
{
	if(NowState == state) return true;

	switch (NowState)
	{
	case EPlayerBehaviorState::Idle:
		if(state != EPlayerBehaviorState::Spawn && state != EPlayerBehaviorState::Die)
			return true;
		break;
	case EPlayerBehaviorState::Move:
		if (state != EPlayerBehaviorState::Spawn && state != EPlayerBehaviorState::Die)
			return true;
		break;
	case EPlayerBehaviorState::Attack:
		if(state == EPlayerBehaviorState::Idle || state == EPlayerBehaviorState::Dodge || state == EPlayerBehaviorState::Damaged)
			return true;
		break;
	case EPlayerBehaviorState::Dodge:
		if(state == EPlayerBehaviorState::Idle || state == EPlayerBehaviorState::Attack || state == EPlayerBehaviorState::SpecialAtt)
			return true;
		break;
	case EPlayerBehaviorState::SpecialAtt:
		if(state == EPlayerBehaviorState::Idle || state == EPlayerBehaviorState::Attack || state == EPlayerBehaviorState::Dodge || state == EPlayerBehaviorState::Damaged)
			return true;
		break;
	case EPlayerBehaviorState::Spell:
		break;
	case EPlayerBehaviorState::Interaction:
		break;
	case EPlayerBehaviorState::Damaged:
		if(state == EPlayerBehaviorState::Idle || state == EPlayerBehaviorState::Die)
			return true;
		break;
	case EPlayerBehaviorState::Die:
		break;
	case EPlayerBehaviorState::Spawn:
		if(state == EPlayerBehaviorState::Idle)
			return true;
		break;
	default:
		break;
	}
	return false;
}

void APlayerZagreus::Move(const FInputActionValue& inputValue)
{
	if(bDamaged || bSpecialAtt) return;

	//if(NowState != EPlayerBehaviorState::Idle && NowState != EPlayerBehaviorState::Move) {
	//	if (PlayerDir == FVector::ZeroVector) {
	//		FVector2D value = inputValue.Get<FVector2D>();
	//		PlayerDir.X = value.X;
	//		PlayerDir.Y = value.Y;
	//	}
	//	return;
	//}

	if (NowState != EPlayerBehaviorState::Move && CheckChangeStateEnabled(EPlayerBehaviorState::Move)) {
		NowState = EPlayerBehaviorState::Move;
	}

	FVector2D value = inputValue.Get<FVector2D>();
	PlayerDir.X = value.X;
	PlayerDir.Y = value.Y;
}

// ���ʹ� ������ �� ������ ���⿡��
void APlayerZagreus::Attack(const FInputActionValue& inputValue)
{
	if(bDamaged || bSpecialAtt || !CheckChangeStateEnabled(EPlayerBehaviorState::Attack)) return;
	bReserveAttack = true;
	Speed = RunSpeed;
	bForceSpecialAtt = false;
}

void APlayerZagreus::Dodge(const FInputActionValue& inputValue)
{
	// �ǰ� ���� / ȸ�� ���� ��� �ð� / Q ��ų ���� �� / ��� ���� �� ��� �Ұ����� ������ ���
	if(bDamaged || bDodgeDelayWait || bSpecialAtt || NowState == EPlayerBehaviorState::Dodge || !CheckChangeStateEnabled(EPlayerBehaviorState::Dodge)) return;

	if (NowState == EPlayerBehaviorState::SpecialAtt) {
		if (bForceSpecialAtt) {
			return;
		}
		else {
			bForceSpecialAtt = true;
		}
	}

	NowState = EPlayerBehaviorState::Dodge;

	if (bAttackProcess) {
		bAttackProcess = false;
		bReserveAttack = false;
		Combo = 0;
		CurrentAnimTime = 0.0f;
		weapon->EndAttack();
	}

	Speed = DodgeSpeed;
	// ���ʹ̿� �浹 Ignore
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	bDodgeAttackWait = false;
	AnimWaitTime = DodgeTime;
}

// ���ʹ� ������ �� ������ ���⿡��
void APlayerZagreus::SpecialAtt(const FInputActionValue& inputValue)
{
	if(bDamaged) return;
	StartSpecialAtt();
}

void APlayerZagreus::Spell(const FInputActionValue& inputValue)
{
	if (NowState != EPlayerBehaviorState::Spell) {
		NowState = EPlayerBehaviorState::Spell;
	}
	SetAttackDir();

	// ���� �ִϸ��̼� -> ������ NowState ����
}

void APlayerZagreus::Interaction(const FInputActionValue& inputValue)
{
	if (NowState != EPlayerBehaviorState::Interaction) {
		NowState = EPlayerBehaviorState::Interaction;
	}

	// ��ȣ�ۿ�
}

void APlayerZagreus::CheatInvincible(const FInputActionValue& inputValue)
{
	if (GetCapsuleComponent()->GetCollisionResponseToChannel(ECC_Pawn) == ECR_Ignore) {
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
	else {
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	}
}
