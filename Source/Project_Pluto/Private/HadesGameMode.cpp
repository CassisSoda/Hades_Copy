// Fill out your copyright notice in the Description page of Project Settings.


#include "HadesGameMode.h"
#include "MainWidget.h"
#include "BossStateWidget.h"
#include "GameOverWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameClearWidget.h"

void AHadesGameMode::BeginPlay()
{
	Super::BeginPlay();

	auto controller = GetWorld()->GetFirstPlayerController();
	
	if(controller == nullptr) return;

	// ���콺 Ŀ�� ����
	controller->bShowMouseCursor = true;

	// ���콺 ������ Project Settings ���� Default �� �����Ѱ� ����
	controller->DefaultMouseCursor = EMouseCursor::Default;
	controller->CurrentMouseCursor = EMouseCursor::Default;

	if (mainWidget != nullptr) {
		// mainWidget �������Ʈ ������ �޸𸮿� �ε�
		mainUI = CreateWidget<UMainWidget>(GetWorld(), mainWidget);

		// ������ �޸𸮿� �ε�Ǹ� �� ��Ʈ�� ���
		if (mainUI != nullptr) {
			mainUI->AddToViewport();

			// �⺻������ input focus �������� ���� -> ���� ����
			//controller->SetInputMode(FInputModeGameOnly());

			if (mainUI->BossStateWidget != nullptr) {
				mainUI->BossStateWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}

	if (gameOverWidget != nullptr) {
		// gameOverWidget �������Ʈ ������ �޸𸮿� �ε�
		gameOverUI = CreateWidget<UGameOverWidget>(GetWorld(), gameOverWidget);
		//gameOverUI->AddToViewport();
		//gameOverUI->SetVisibility(ESlateVisibility::Hidden);
	}

	if (gameClearWidget != nullptr) {
		// gameClearWidget �������Ʈ ������ �޸𸮿� �ε�
		gameClearUI = CreateWidget<UGameClearWidget>(GetWorld(), gameClearWidget);
		//gameClearUI->AddToViewport();
		//gameClearUI->SetVisibility(ESlateVisibility::Hidden);
	}
}

void AHadesGameMode::SetPlayerHP(int32 Cur, int32 Max)
{
	if(mainUI == nullptr) return;
	mainUI->SetHP(Cur, Max);
	
	mainUI->SetNowHP(Cur);
	mainUI->SetMaxHP(Max);
}

void AHadesGameMode::SetBossHP(int32 Cur, int32 Max)
{
	if(mainUI == nullptr || mainUI->BossStateWidget == nullptr) return;
	mainUI->BossStateWidget->SetHP(Cur, Max);
}

void AHadesGameMode::SetBossName(FString name)
{
	if (mainUI == nullptr || mainUI->BossStateWidget == nullptr) return;
	mainUI->BossStateWidget->SetName(name);
}

void AHadesGameMode::ShowBossState(bool bShow)
{
	if(mainUI == nullptr) return;

	mainUI->ShowBossState(bShow);
}

void AHadesGameMode::ShowGameOver(bool bShow)
{
	if (mainUI == nullptr) return;

	UGameplayStatics::SetGamePaused(GetWorld(), bShow);
	mainUI->ShowGameOver(bShow);
}

void AHadesGameMode::ShowGameClear(bool bShow)
{
	if (mainUI == nullptr) return;

	UGameplayStatics::SetGamePaused(GetWorld(), bShow);
	mainUI->ShowGameClear(bShow);
}
