// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_PLUTO_API UPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPlayerAnimInstance();

	// �� ������ ���ŵǴ� �Լ�
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;


	// �÷��̾� �̵� �ӵ�
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = TPSPlayerAnim)
	float Speed = 0.0f;

	// �÷��̾� �¿� �̵� �ӵ� -> ������ ���� �ʳ� ������
	UPROPERTY( EditDefaultsOnly , BlueprintReadWrite , Category = TPSPlayerAnim )
	float Direction = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	class APlayerZagreus* player;


	// Q Skill Attack Effect Start
	UFUNCTION()
	void AnimNotify_AttackEffect();

	// Q Skill Attack Effect End
	UFUNCTION()
	void AnimNotify_AttackEnd();

	// Q Skill Attack Anim End
	UFUNCTION()
	void AnimNotify_SpecialAttEnd();
};
