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

	//weapon = CreateDefaultSubobject<APWBlade>(TEXT("PlayerWeapon"));
	//weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("FX_weapon"));
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
}

// Called every frame
void APlayerZagreus::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (PlayerDir == FVector::ZeroVector && NowState == EPlayerBehaviorState::Move) {
		NowState = EPlayerBehaviorState::Idle;
	}

	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Green, UEnum::GetValueAsString(NowState));

	{
		// �÷��̾� �̵�
		//PlayerDir.Normalize();
		//SetActorLocation(GetActorLocation() + PlayerDir * Speed * DeltaTime);
		AddMovementInput(PlayerDir.GetSafeNormal());
		PlayerDir = FVector::ZeroVector;
	}

	{
		// �޺� ���� Ȯ��
		if (isCombo && NowState == EPlayerBehaviorState::Attack) {
			CurrentAttackTime += DeltaTime;
			if (CurrentAttackTime >= ComboWaitTime) {
				isCombo = false;
				// ���� ���� �� �ʱ�ȭ �������� Ȥ�� �𸣴� ���⼭�� �޺� �ʱ�ȭ
				Combo = 0;
				CurrentAttackTime = 0.0f;
				
				NowState = EPlayerBehaviorState::Idle;
			}
		}
	}
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
	SetAttackDir();
	UE_LOG(LogTemp, Warning, TEXT("Player : %.1f , %.1f"), GetActorLocation().X, GetActorLocation().Y);
	UE_LOG(LogTemp, Warning, TEXT("Mouse : %.1f , %.1f"), MouseLocation.X, MouseLocation.Y);

	if (!isCombo) { // �޺� ���� �÷��� ����
		Combo = 0;
		isCombo = true;
	}

	Combo++;
	CurrentAttackTime = 0.0f;

	if (Combo > weapon->MaxCombo) {
		Combo = 1;
	}

	if (NowState == EPlayerBehaviorState::Dodge) {
		NowState = EPlayerBehaviorState::Attack;
		Combo = 3;
		// Thrust �ִϸ��̼� ����

		Combo = 0;
		isCombo = false;
		NowState = EPlayerBehaviorState::Idle;
	} else if (NowState != EPlayerBehaviorState::Attack) {
		NowState = EPlayerBehaviorState::Attack;
	}

	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Red, FString::Printf(TEXT("Combo : %d"), Combo));
	// �ִϸ��̼� ��� ������� ���? -> �ִϸ��̼��� ������ ���� �Է� ������ �⺻ Idle �� ���ư���
}

void APlayerZagreus::Dodge(const FInputActionValue& inputValue)
{
	if (NowState != EPlayerBehaviorState::Dodge) {
		NowState = EPlayerBehaviorState::Dodge;
	}

	Speed = 1000.0f;
	// ���ʹ̿� �� �ε������� ó��
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	
	// �ִϸ��̼� �ð��� ������ �浹 üũ�� ���ǵ� ����
	Speed = 700.0f;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
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
