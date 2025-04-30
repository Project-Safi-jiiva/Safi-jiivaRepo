// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MHGameMode.generated.h"

/**
 *
 */
UCLASS()
class PROJECT_SAFI_JIIVA_API AMHGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	int32 QuestLife = 3;

	void setQuestLife();
	int32 GetQuestLife();


};
