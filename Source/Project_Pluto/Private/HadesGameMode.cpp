// Fill out your copyright notice in the Description page of Project Settings.


#include "HadesGameMode.h"
#include "MainWidget.h"
#include "BossStateWidget.h"
#include "GameOverWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameClearWidget.h"
#include "HadesSaveGame.h"


//������ ��Ʈ�� ���� ���� �߰� ��Ʈ�Դϴ�.
AHadesGameMode::AHadesGameMode()
{

	//�뷡 �̰����� �־�ΰ� ������� ��Ŵ
	//������ �뷡��... ���� �������� ������... ����Ʈ���� �����Ƚ��ϴ�.
	//ConstructorHelpers::FObjectFinder<USoundBase> tmpSound_0(TEXT("/Script/Engine.SoundWave'/Game/KJY/RSS/God_of_the_Dead_-//_Hades_mp3.God_of_the_Dead_-_Hades_mp3'"));
	

	//1-3���� ������ ���Խ�Ŵ
	ConstructorHelpers::FObjectFinder<USoundBase> tmpSound_1(TEXT("/Script/Engine.SoundWave'/Game/KJY/RSS/Hades_-_The_Painful_Way.Hades_-_The_Painful_Way'"));

	ConstructorHelpers::FObjectFinder<USoundBase> tmpSound_2(TEXT("/Script/Engine.SoundWave'/Game/KJY/RSS/Mouth_of_Styx_-_Hades_OST.Mouth_of_Styx_-_Hades_OST'"));

	ConstructorHelpers::FObjectFinder<USoundBase> tmpSound_3(TEXT("/Script/Engine.SoundWave'/Game/KJY/RSS/Out_of_Tartarus_-_Hades_OST.Out_of_Tartarus_-_Hades_OST'"));

	int32 randMusic = FMath::RandRange(1,3);

	while (randMusic == beforeMusic)
	{
		randMusic = FMath::RandRange(1, 3);
	}

		switch (randMusic)
		{
		case 1:
			GameSound = tmpSound_1.Object;
			beforeMusic = randMusic;
			break;

		case 2:
			GameSound = tmpSound_2.Object;
			beforeMusic = randMusic;
			break;

		case 3:
			GameSound = tmpSound_3.Object;
			beforeMusic = randMusic;
			break;
		}


}



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

	LoadGameData();

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


	//���� �÷��� �߰�
	UGameplayStatics::PlaySound2D(GetWorld(), GameSound);
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

	if(bShow) {
		HPBuff = 0;
		NowHP = 0;
		SaveGameData();
	}
}

void AHadesGameMode::ShowGameClear(bool bShow)
{
	if (mainUI == nullptr) return;

	UGameplayStatics::SetGamePaused(GetWorld(), bShow);
	mainUI->ShowGameClear(bShow);

	if (bShow) {
		HPBuff = 0;
		NowHP = 0;
		SaveGameData();
	}
}

void AHadesGameMode::SaveGameData()
{
	// SaveGame ��ü�� ����
	UHadesSaveGame* sg = Cast<UHadesSaveGame>(UGameplayStatics::CreateSaveGameObject(UHadesSaveGame::StaticClass()));

	// ��ü�� ����
	sg->HPBuffSave = HPBuff;
	sg->nowHPSave = NowHP;

	// ���Ϸ� ����
	UGameplayStatics::SaveGameToSlot(sg, SaveSlotName, UserIndex);


	UE_LOG(LogTemp, Error, TEXT("Save Game : Buff : %d, NowHP : %d"), HPBuff, NowHP);
}

void AHadesGameMode::LoadGameData()
{
	// ������ �����ϴ��� Ȯ��
	bool isExist = UGameplayStatics::DoesSaveGameExist(SaveSlotName, UserIndex);
	if (isExist == false) return;

	// ������ ������ �о ShootingSaveGame ��ü�� ����
	UHadesSaveGame* sg = Cast<UHadesSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));

	// ���� �о�ͼ� HighScore ����
	if (sg != nullptr) {
		HPBuff = sg->HPBuffSave;
		NowHP = sg->nowHPSave;
	}
}
