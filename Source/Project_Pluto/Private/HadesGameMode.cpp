// Fill out your copyright notice in the Description page of Project Settings.


#include "HadesGameMode.h"
#include "MainWidget.h"
#include "BossStateWidget.h"
#include "GameOverWidget.h"

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
		}
	}

	if (gameOverWidget != nullptr) {
		// gameOverWidget �������Ʈ ������ �޸𸮿� �ε�
		gameOverUI = CreateWidget<UGameOverWidget>(GetWorld(), gameOverWidget);
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

void AHadesGameMode::ShowBossState(bool bShow)
{
	if(mainUI == nullptr) return;

	mainUI->ShowBossState(bShow);
}

void AHadesGameMode::ShowGameOver(bool bShow)
{
	//if (mainUI == nullptr) return;

	////auto controller = GetWorld()->GetFirstPlayerController();
	////if(controller == nullptr) return;
	////controller->PlayerCameraManager->StartCameraFade(1.f, 0.f, 1.f, FLinearColor::Black, true, true);

	//if (bShow) {
	//	if (gameOverUI != nullptr) {
	//		gameOverUI->AddToViewport();
	//		//controller->SetInputMode(FInputModeUIOnly());
	//	}
	//}
	//else {
	//	if (gameOverUI != nullptr) {
	//		gameOverUI->RemoveFromParent();
	//		//controller->SetInputMode(FInputModeGameOnly());
	//	}
	//}
}
