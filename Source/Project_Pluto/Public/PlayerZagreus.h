// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputActionValue.h"
#include "PlayerZagreus.generated.h"

UENUM(BlueprintType)
enum class EPlayerBehaviorState : uint8 {
	Idle UMETA(DisplayName="Idle"),
	Move UMETA(DisplayName="Move(WASD)"),
	Attack UMETA(DisplayName="Attack(Left Click)"),
	Dodge UMETA(DisplayName="Dodge(Space)"),
	SpecialAtt UMETA(DisplayName="SpecialAtt(Q)"),
	Spell UMETA(DisplayName="Spell(Right Click)"),
	Interaction UMETA(DisplayName="Interaction(E)")
};

UENUM(BlueprintType)
enum class EPlayerWeapon : uint8 {
	Blade UMETA(DisplayName = "Blade")
};

UCLASS()
class PROJECT_PLUTO_API APlayerZagreus : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerZagreus();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


// �÷��̾� ����
public:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	UPROPERTY(EditAnywhere, Category = Camera)
	class USpringArmComponent* springArmComp;

	UPROPERTY(EditAnywhere, Category = Camera)
	class UCameraComponent* camComp;


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerState)
	EPlayerBehaviorState NowState = EPlayerBehaviorState::Idle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerState)
	EPlayerWeapon NowWeapon = EPlayerWeapon::Blade;


	UPROPERTY(EditAnywhere, Category="PlayerSetting")
	float Speed = 700.0f;

	UPROPERTY(EditAnywhere, Category="PlayerSetting")
	float DodgeSpeed = 4000.0f;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Weapon)
	class APlayerWeapon* weapon;

	// void ChangeWeapon();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerSetting")
	int32 InitHP = 50; // �÷��̾� �ʱ� ü��

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerSetting")
	int32 MaxHP = 50; // �÷��̾� �ִ� ü��

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerSetting")
	int32 HP = 50; // �÷��̾� ���� ü��


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerSetting")
	float PlusHP = 0; // ���� ��ȭ : �߰� HP �ۼ�Ʈ

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "PlayerSetting")
	float PlusAtk = 0; // ���� ��ȭ : �߰� ���ݷ� �ۼ�Ʈ


	UPROPERTY()
	class UPlayerAnimInstance* AnimInstance;


// �÷��̾� ����
public:
	class APlayerController* pController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	float AnimWaitTime = 0.6f; // �ִϸ��̼� ��� �ð�. Montage_Play �� �ִϸ��̼� �ð� ����

	float CurrentAnimTime = 0.0f;

	float DefaultAnimWaitTime = 0.6f;

	float DodgeTime = 0.5f;


	FVector PlayerDir;


	FVector AttackDirection; // ���� ���� (�÷��̾� ���� ���콺 ����)

	FVector MouseLocation; // ���콺 ��ġ (�Է� ���� ���� ���콺 ��ġ)

	// ������ �� ���콺 ��ġ �� ���� ���ϱ�
	void SetAttackDir();


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
	int32 Combo = 0; // ���° �޺��ΰ�

	bool bAttackProcess = false; // ���� ���ΰ�
	
	bool bReserveAttack = false; // ���� ����

	void AttackProcess(); // ���� ����


	void EndDodge();


	// �ǰ�
	void OnDamage(int32 damage);

	void SetPermanetBuff();

	// hp �ִ�ġ ���� ������ �Ծ��� ��� / ���ڰ� : ���밪 �÷���, �ۼ�Ʈ �÷���
	void SetBuffMaxHP(int32 plusHpAbs, float plusHpPro);


// Input
public:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* IMC_Player;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Move;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Attack;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Dodge;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_SpecialAtt;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Spell;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Interaction;


	void Move(const FInputActionValue& inputValue); // �̵�
	void Attack(const FInputActionValue& inputValue); // �Ϲ� ���� & �޺�
	void Dodge(const FInputActionValue& inputValue); // ȸ�� & ���
	void SpecialAtt(const FInputActionValue& inputValue); // Ư�� ���� (Q ��ų)
	void Spell(const FInputActionValue& inputValue); // ���� (��Ŭ��)
	void Interaction(const FInputActionValue& inputValue); // ��ȣ�ۿ� (E ��ư)
};
