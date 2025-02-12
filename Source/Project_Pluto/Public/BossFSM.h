// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BossFSM.generated.h"


// ����� ���� ����
UENUM(BlueprintType)
enum class EBossState : uint8
{
	Idle			UMETA(DisplayName = "Idle"),
	Move			UMETA(DisplayName = "Move"),
	Attack		UMETA(DisplayName = "Attack"),
	Hit			UMETA(DisplayName = "Hit"),
	PhaseChange	UMETA(DisplayName = "PhaseChange"),
	Die			UMETA(DisplayName = "Die")
};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_PLUTO_API UBossFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBossFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	//���¸� ��Ÿ���� ����
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "FSM")
	EBossState state = EBossState::Idle;

	FORCEINLINE float GetDeltaTime() { return GetWorld()->DeltaTimeSeconds; }

#pragma region State Function

	void State_Idle();

	void State_Move();

	void State_Attack();

	void State_Hit();

	void State_PhaseChange();

	void State_Die();

#pragma endregion State Function

#pragma region Values for State Change

	float nowTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "FSM")
	float PhaseChangeTime = 4.0f;

	FVector destination;
	FVector direction;



#pragma endregion Values for State Change

	UPROPERTY(VisibleAnywhere, Category = "FSM")
	class ABoss* me;
	
	//�������� �� ���� ���͸� ���ϴ� �Լ�
	void SetDesDir();

	void OnTakeDamage();

		
};
