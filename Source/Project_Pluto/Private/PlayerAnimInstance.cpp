// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerZagreus.h"

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
