// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Lobby/MHGameInstance.h"
#include "SessionButtonWidget.generated.h"

/**
 *
 */
UCLASS()
class PROJECT_SAFI_JIIVA_API USessionButtonWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
public:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_Session;

	void Set(const FSessionInfo& InSessionInfo);
	int32 SessionNumber;

	UFUNCTION()
	void JoinSession();

	class UQuestBoardWidget* QuestBoardWidget;
};
