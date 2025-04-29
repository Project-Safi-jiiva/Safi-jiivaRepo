// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HunterController.generated.h"

/**
 *
 */
UCLASS()
class PROJECT_SAFI_JIIVA_API AHunterController : public APlayerController
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	class AMHGameMode* gm;
	FTimerHandle RespawnTimerHandle;
public:
	UFUNCTION(Server, Reliable)
	void ServerRPC_RespawnPlayer();

	UPROPERTY(EditDefaultsOnly, Category = UI)
	TSubclassOf<class UHunterMainWidget> mainUIWidget;
	// mainUIWidget 으로 부터 만들어진 인스턴스
	UPROPERTY()
	class UHunterMainWidget* mainUI;

};
