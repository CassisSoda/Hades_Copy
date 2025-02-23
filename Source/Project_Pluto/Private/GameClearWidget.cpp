// Fill out your copyright notice in the Description page of Project Settings.


#include "GameClearWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

void UGameClearWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// �� ��ư �Է� �̺�Ʈ�� �����ϴ� �Լ��� ����
	Button_Restart->OnClicked.AddDynamic(this, &UGameClearWidget::Restart);
	Button_Quit->OnClicked.AddDynamic(this, &UGameClearWidget::Quit);
}

void UGameClearWidget::Restart()
{
	// ���� Ŭ����� ���� ������ �ε�
	UGameplayStatics::OpenLevel(GetWorld(), FName("HadesMap1"));
}

void UGameClearWidget::Quit()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetWorld()->GetFirstPlayerController(), EQuitPreference::Quit, false);
}
