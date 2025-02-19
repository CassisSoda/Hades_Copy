// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "KEnemy.h"
#include "KThanatos.generated.h"

UCLASS()
class PROJECT_PLUTO_API AKThanatos : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AKThanatos();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	*/

public:
	UPROPERTY(VisibleAnywhere , BlueprintReadOnly, Category = FSMComponent)
	class UKThanatosFSM* fsm;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf <class AKDestroyBox> BoxFactory;

	class AKDestroyBox* destroyBox;

public:
	void SetDestroyBox();
};
