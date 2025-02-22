#include "GameOverWidget.h"
#include "GameOverWidget.h"
// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// �� ��ư �Է� �̺�Ʈ�� �����ϴ� �Լ��� ����
	Button_Restart->OnClicked.AddDynamic(this, &UGameOverWidget::Restart);
	Button_Quit->OnClicked.AddDynamic(this, &UGameOverWidget::Quit);
}

void UGameOverWidget::Restart()
{
	// ���� ���� �� �÷����ϴ� ������ �ٽ� �ε�
	FString mapName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	UGameplayStatics::OpenLevel(GetWorld(), FName(*mapName));
}

void UGameOverWidget::Quit()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
}
