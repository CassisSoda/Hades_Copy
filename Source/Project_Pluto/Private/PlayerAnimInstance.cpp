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

void UPlayerAnimInstance::AnimNotify_AttackEffect()
{
	// �÷��̾ ������ �ִ� ������ ���� ���� ovelap üũ ����
	// �� ���� ���� �켱 ���� �ֻ� -> ȸ��, �̵�, ���� �� �� �ִϸ��̼��� ������ ������ �ƹ��͵� ������ �� ����.
	player->bSpecialAtt = true;
	player->bForceSpecialAtt = true;
	player->SetAttackDir();
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
	player->bSpecialAtt = false;
	player->bForceSpecialAtt = false;
	player->Speed = player->RunSpeed;
	player->NowState = EPlayerBehaviorState::Idle;
}
