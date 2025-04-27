// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/QuestBoardWidget.h"
#include "Components/Button.h"
#include "Project_Safi_jiiva.h"
#include "Components/WidgetSwitcher.h"
#include "Lobby/MHGameInstance.h"
#include "Widget/SessionButtonWidget.h"
#include "Components/VerticalBox.h"

void UQuestBoardWidget::NativeConstruct()
{
	Super::NativeConstruct();
	MHGI = Cast<UMHGameInstance>(GetWorld()->GetGameInstance());
	btn_MakeRoom->OnClicked.AddDynamic(this, &UQuestBoardWidget::makeRoomHandler);
	btn_JoinRoom->OnClicked.AddDynamic(this, &UQuestBoardWidget::joinRoomHandler);
	btn_MakeSession->OnClicked.AddDynamic(this, &UQuestBoardWidget::CreateRoomHandler);
	MHGI->onSearchCompleted.AddDynamic(this, &UQuestBoardWidget::AddSlotWidget);
	MHGI->OnCreateSessionCompleted.AddDynamic(this, &UQuestBoardWidget::OnCreateSessionCompletedHandler);
	MHGI->OnJoinSessionCompleted.AddDynamic(this, &UQuestBoardWidget::OnJoinSessionCompletedHandler);

	QuestMakeYButton->OnClicked.AddDynamic(this, &UQuestBoardWidget::QuestMakeYButtonHandler);
	QuestMakeNButton->OnClicked.AddDynamic(this, &UQuestBoardWidget::QuestMakeNButtonHandler);
	QuestJoinYButton->OnClicked.AddDynamic(this, &UQuestBoardWidget::QuestJoinYButtonHandler);
	QuestJoinNButton->OnClicked.AddDynamic(this, &UQuestBoardWidget::QuestJoinNButtonHandler);
}
//SelectWindow 함수 구현부
void UQuestBoardWidget::makeRoomHandler()
{
	WidgetSwitcher->SetActiveWidgetIndex(1);
}

void UQuestBoardWidget::joinRoomHandler()
{
	WidgetSwitcher->SetActiveWidgetIndex(2);
	JoinRoomBox->ClearChildren();
	MHGI->FindOtherSession();
}
// questWindow 함수 구현부
void UQuestBoardWidget::CreateRoomHandler()
{
	WidgetSwitcher->SetActiveWidgetIndex(3);

}
//QuestMakeButton 함수 구현부

void UQuestBoardWidget::QuestMakeYButtonHandler()
{
	MHGI->CreateMySession();
}

void UQuestBoardWidget::QuestMakeNButtonHandler()
{
	WidgetSwitcher->SetActiveWidgetIndex(1);

}

//QuestJoinButton 함수
void UQuestBoardWidget::QuestJoinYButtonHandler()
{
	auto gi = Cast<UMHGameInstance>(GetWorld()->GetGameInstance());
	if (gi)
	{
		gi->JoinSelectedSession(SessionNumber);
	}
}


void UQuestBoardWidget::QuestJoinNButtonHandler()
{
	WidgetSwitcher->SetActiveWidgetIndex(1);
}

void UQuestBoardWidget::SetSessionNumber(int32 Number)
{
	this->SessionNumber = Number;
}

void UQuestBoardWidget::AddSlotWidget(const struct FSessionInfo& InSessionInfo)
{
	auto slot = CreateWidget<USessionButtonWidget>(this, SessionButtonWidgetClass);
	slot->QuestBoardWidget = this;
	slot->Set(InSessionInfo);

	JoinRoomBox->AddChild(slot);
}

void UQuestBoardWidget::OnCreateSessionCompletedHandler()
{
	WidgetSwitcher->SetActiveWidgetIndex(5);
}

void UQuestBoardWidget::OnJoinSessionCompletedHandler()
{
	WidgetSwitcher->SetActiveWidgetIndex(5);
}
