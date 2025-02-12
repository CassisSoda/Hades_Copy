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

	// �ִϸ��̼� Ŭ���� �Ҵ�
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	ConstructorHelpers::FClassFinder<UAnimInstance> TempAnim(TEXT("/Game/RGY/Blueprints/ABP_PlayerZagreus.ABP_PlayerZagreus_C"));

	if (TempAnim.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(TempAnim.Class);
		
	}

	// �÷��̾ �̵� �������� ȸ���ϵ���
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		MovementComponent->bOrientRotationToMovement = true;
		MovementComponent->bUseControllerDesiredRotation = false;
	}

	GetCharacterMovement()->MaxWalkSpeed = Speed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = Speed;

	// �ʱ� ī�޶� ����
	springArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	springArmComp->SetupAttachment(RootComponent);
	springArmComp->SetRelativeLocation(FVector(0.0f));
	springArmComp->TargetArmLength = 1000.0f;
	springArmComp->bDoCollisionTest = false;
	springArmComp->SetWorldRotation(FRotator(-50.0f, -30.0f, 0.0f));

	camComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CamComp"));
	camComp->SetupAttachment(springArmComp);
}

// Called when the game starts or when spawned
void APlayerZagreus::BeginPlay()
{
	Super::BeginPlay();
	
	auto pc = Cast<APlayerController>(Controller);
	if (pc) {
		auto subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(pc->GetLocalPlayer());

		if (subsystem) {
			subsystem->AddMappingContext(IMC_Player, 0);
		}
	}

	{
		// ���� ����
		FName WeaponSocket(TEXT("muzzle_01")); // FX_weapon
		FTransform weaponPosition = GetMesh()->GetSocketTransform(TEXT("muzzle_01"));
		auto CurrentWeapon = GetWorld()->SpawnActor<APWBlade>(FVector::ZeroVector, FRotator::ZeroRotator);
		if (CurrentWeapon != nullptr) {
			weapon = CurrentWeapon;
			weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("FX_weapon"));
		}
	}

	GetCharacterMovement()->MaxWalkSpeed = Speed;
}

// Called every frame
void APlayerZagreus::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (PlayerDir == FVector::ZeroVector && NowState == EPlayerBehaviorState::Move) {
		NowState = EPlayerBehaviorState::Idle;
	}

	{
		// �÷��̾� �̵�
		AddMovementInput(PlayerDir.GetSafeNormal());
		PlayerDir = FVector::ZeroVector;
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
				GEngine->AddOnScreenDebugMessage(0, 1, FColor::Green, FString::Printf(TEXT("Combo : %d"), Combo));

				CurrentAnimTime += DeltaTime;
				if (CurrentAnimTime >= AnimWaitTime) {
					CurrentAnimTime = 0.0f;
					AttackProcess();
				}
			}
			else if (bReserveAttack) { // ���� ���� �ƴϰ� ���� ������ ��������� ���� ���μ��� ����
				AttackProcess();
			}
		}
	}

	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Green, UEnum::GetValueAsString(NowState));
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
		PlayerInput->BindAction(IA_Spell, ETriggerEvent::Started, this, &APlayerZagreus::Spell);
		PlayerInput->BindAction(IA_Interaction, ETriggerEvent::Started, this, &APlayerZagreus::Interaction);
	}
}

void APlayerZagreus::NotifyActorBeginOverlap(AActor* OtherActor)
{
	// ���� ���� �� ���� �ε����� ������ �����

}

void APlayerZagreus::SetAttackDir()
{
	auto PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController) {
		FHitResult hit;
		PlayerController->GetHitResultUnderCursor(ECC_Visibility, false, hit);

		MouseLocation = hit.Location;

		AttackDirection = MouseLocation - GetActorLocation();
		SetActorRotation(FRotator(0.0f, UKismetMathLibrary::ClampAxis(AttackDirection.Rotation().Yaw), 0.0f));
	}
}

void APlayerZagreus::AttackProcess()
{
	// �ִϸ��̼� Montrage_Play �� �ִϸ��̼� �÷��� �ð� ����

	if(!bAttackProcess) { // ���� ����
	
		bAttackProcess = true;
		bReserveAttack = false;
		SetAttackDir();

		Combo++;
		CurrentAnimTime = 0.0f;

		if (Combo > weapon->MaxCombo) {
			Combo = 1;
		}

		UE_LOG(LogTemp, Warning, TEXT("Attack Start. Combo : %d"), Combo);

		if (NowState != EPlayerBehaviorState::Attack) {
			NowState = EPlayerBehaviorState::Attack;
		}
	}
	else { // ���� �ִϸ��̼� ���� �� ����
		UE_LOG(LogTemp, Warning, TEXT("Attack End"));
		if (!bReserveAttack) {
			Combo = 0;
			if (NowState == EPlayerBehaviorState::Attack) {
				NowState = EPlayerBehaviorState::Idle;
			}
		}
		
		bAttackProcess = false;
	}
}

void APlayerZagreus::EndDodge()
{
	UE_LOG(LogTemp, Warning, TEXT("Dodge End"));

	GetCharacterMovement()->MaxWalkSpeed = Speed;
	// ���ʹ̿� �浹 Overlap ���� ����
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	if (NowState == EPlayerBehaviorState::Dodge) {
		NowState = EPlayerBehaviorState::Idle;
	}
}

void APlayerZagreus::OnDamage(int32 damage)
{
	HP -= damage;

	// �ǰ� �ִϸ��̼�
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

void APlayerZagreus::Move(const FInputActionValue& inputValue)
{
	if(NowState == EPlayerBehaviorState::Dodge || NowState == EPlayerBehaviorState::Attack) return;

	if (NowState != EPlayerBehaviorState::Move) {
		NowState = EPlayerBehaviorState::Move;
	}

	FVector2D value = inputValue.Get<FVector2D>();
	PlayerDir.X = value.X;
	PlayerDir.Y = value.Y;
}

// ���ʹ� ������ �� ������ ���⿡��
void APlayerZagreus::Attack(const FInputActionValue& inputValue)
{
	if (NowState == EPlayerBehaviorState::Dodge) {
		Combo = -100;
	}
	bReserveAttack = true;
}

void APlayerZagreus::Dodge(const FInputActionValue& inputValue)
{
	UE_LOG(LogTemp, Warning, TEXT("Dodge Start"));

	if (NowState != EPlayerBehaviorState::Dodge) {
		NowState = EPlayerBehaviorState::Dodge;
	}

	if (bAttackProcess) {
		bAttackProcess = false;
		bReserveAttack = false;
		Combo = 0;
		CurrentAnimTime = 0.0f;
	}

	GetCharacterMovement()->MaxWalkSpeed = DodgeSpeed;
	// ���ʹ̿� �浹 Ignore
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

// ���ʹ� ������ �� ������ ���⿡��
void APlayerZagreus::SpecialAtt(const FInputActionValue& inputValue)
{
	if (NowState != EPlayerBehaviorState::SpecialAtt) {
		NowState = EPlayerBehaviorState::SpecialAtt;
	}
	SetAttackDir();

	// Ư������ �ִϸ��̼� -> ������ NowState ����
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
