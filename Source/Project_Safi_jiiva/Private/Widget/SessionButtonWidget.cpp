// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/SessionButtonWidget.h"
#include "Components/Button.h"
#include "Lobby/MHGameInstance.h"
#include "Widget/QuestBoardWidget.h"
#include "Components/WidgetSwitcher.h"

void USessionButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	btn_Session->OnClicked.AddDynamic(this, &USessionButtonWidget::JoinSession);
}

void USessionButtonWidget::Set(const FSessionInfo& InSessionInfo)
{
	SessionNumber = InSessionInfo.index;
}

void USessionButtonWidget::JoinSession()
{
	QuestBoardWidget->SessionNumber = SessionNumber;
	QuestBoardWidget->WidgetSwitcher->SetActiveWidgetIndex(4);

}
