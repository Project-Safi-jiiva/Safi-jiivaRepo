// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestBoardWidget.generated.h"

/**
 *
 */
UCLASS()
class PROJECT_SAFI_JIIVA_API UQuestBoardWidget : public UUserWidget
{
	GENERATED_BODY()


	virtual void NativeConstruct() override;

public:
	UPROPERTY()
	class UMHGameInstance* MHGI;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;

	//SelectWindow À§Á¬
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_MakeRoom;

	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_JoinRoom;

	// questWindow À§Á¬
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_MakeSession;
	//JoinWindow À§Á¬
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UVerticalBox* JoinRoomBox;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="UI")
	TSubclassOf<class USessionButtonWidget>SessionButtonWidgetClass;

	class USessionButtonWidget* SessionButtonWidget;

	//QuestMakeButton À§Á¬
	UPROPERTY(BlueprintReadWrite , meta = (BindWidget))
	class UButton* QuestMakeYButton;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UButton* QuestMakeNButton;

	//QuestJoinButton À§Á¬
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UButton* QuestJoinYButton;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UButton* QuestJoinNButton;

	//QuestPartyWindow À§Á¬
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* HunterName1;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* HunterName2;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* HunterName3;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* HunterName4;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* hunterReady1;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* hunterReady2;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* hunterReady3;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UTextBlock* hunterReady4;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_hostReady;
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget))
	class UButton* btn_Start;

	//ÇÔ¼ö

	//SelectWindow ÇÔ¼ö
	UFUNCTION()
	void makeRoomHandler();
	UFUNCTION()
	void joinRoomHandler();
	// questWindow ÇÔ¼ö
	UFUNCTION()
	void CreateRoomHandler();

	//QuestMakeButton ÇÔ¼ö
	UFUNCTION()
	void QuestMakeYButtonHandler();
	UFUNCTION()
	void QuestMakeNButtonHandler();
	//QuestJoinButton ÇÔ¼ö
	UFUNCTION()
	void QuestJoinYButtonHandler();
	UFUNCTION()
	void QuestJoinNButtonHandler();

	int32 SessionNumber;
	void SetSessionNumber(int32 SessionNumber);

	UFUNCTION()
	void AddSlotWidget(const struct FSessionInfo& InSessionInfo);

	UFUNCTION()
	void OnCreateSessionCompletedHandler();
	UFUNCTION()
	void OnJoinSessionCompletedHandler();


};
