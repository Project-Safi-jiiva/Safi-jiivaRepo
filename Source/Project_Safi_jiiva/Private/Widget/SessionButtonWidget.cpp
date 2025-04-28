// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/SessionButtonWidget.h"
#include "Components/Button.h"
#include "Lobby/MHGameInstance.h"
#include "Widget/QuestBoardWidget.h"
#include "Components/WidgetSwitcher.h"

void USessionButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UMHGameInstance* MHGI = Cast<UMHGameInstance>(GetWorld()->GetGameInstance());
	btn_Session->OnClicked.AddDynamic(this, &USessionButtonWidget::JoinSession);
	OnJoinSessionRequested.AddDynamic(MHGI, &UMHGameInstance::HandleJoinSessionRequested);
}

void USessionButtonWidget::Set(const FSessionInfo& InSessionInfo)
{
	SessionNumber = InSessionInfo.index;
	WidgetInSessionInfo = InSessionInfo;
}

void USessionButtonWidget::JoinSession()
{
	QuestBoardWidget->SessionNumber = SessionNumber;
	QuestBoardWidget->WidgetSwitcher->SetActiveWidgetIndex(4);
	WidgetInSessionInfo.roomName;
	OnJoinSessionRequested.Broadcast(SessionNumber, WidgetInSessionInfo.roomName);
}

