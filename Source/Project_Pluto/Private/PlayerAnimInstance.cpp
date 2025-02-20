// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerZagreus.h"
#include "PlayerWeapon.h"

UPlayerAnimInstance::UPlayerAnimInstance()
{
	auto actor = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerZagreus::StaticClass());
	player = Cast<APlayerZagreus>(actor);
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if(player == nullptr) {
		player = Cast<APlayerZagreus>(TryGetPawnOwner());
		if(player == nullptr) return;
	}

	FVector velocity = player->GetVelocity();
	FVector forwardVector = player->GetActorForwardVector();

	// �� �� �̵��ӵ�
	Speed = FVector::DotProduct(forwardVector, velocity);

	// �� �� �̵��ӵ�
	FVector rightVector = player->GetActorRightVector();
	Direction = FVector::DotProduct(rightVector, velocity);
}

void UPlayerAnimInstance::AnimNotify_SpecialAttStart()
{
	// �� ���� ���� �켱 ���� �ֻ� -> �̵�, ���� �� �� �ִϸ��̼��� ������ ������ �ƹ��͵� ������ �� ����.
	player->bSpecialAtt = true;
	//player->bForceSpecialAtt = true;
	player->SetAttackDir();
}

void UPlayerAnimInstance::AnimNotify_AttackEffect()
{
	// �÷��̾ ������ �ִ� ������ ���� ���� ovelap üũ ����
	player->weapon->StartSpecialAtt();
}

void UPlayerAnimInstance::AnimNotify_AttackEnd()
{
	// �÷��̾ ������ �ִ� ������ ���� ���� overlap üũ ����
	player->weapon->EndSpecialAtt();
}

void UPlayerAnimInstance::AnimNotify_SpecialAttEnd()
{
	// ������ NowState ������ Idle �� ����
	player->Speed = player->RunSpeed;
	player->NowState = EPlayerBehaviorState::Idle;
	player->bSpecialAtt = false;
	player->bForceSpecialAtt = false;
}

void UPlayerAnimInstance::AnimNotify_DamagedEnd()
{
	if (player->HP > 0) {
		player->Speed = player->RunSpeed;
		player->NowState = EPlayerBehaviorState::Idle;
	}
	else {
		player->NowState = EPlayerBehaviorState::Die;
	}
}

void UPlayerAnimInstance::AnimNotify_LastCombo()
{
	if (player->Combo == player->weapon->MaxCombo) {
		player->weapon->LastCombo();
	}
}
