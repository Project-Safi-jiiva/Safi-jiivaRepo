// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionSettings.h"
#include "MHGameInstance.generated.h"

/**
 *
 */
UCLASS()
class PROJECT_SAFI_JIIVA_API UMHGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;

public:
	IOnlineSessionPtr sessionInterface;

	void CreateMySession(int32 playerCount);

	// 세션 호스트 이름
	FString mySessionName = "reppy";

	UFUNCTION()
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
};
