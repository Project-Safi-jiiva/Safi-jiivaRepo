// Fill out your copyright notice in the Description page of Project Settings.


#include "Hunter/HunterController.h"
#include "MHGameMode.h"
#include "Hunter/Hunter.h"

void AHunterController::BeginPlay() {
	Super::BeginPlay();
	if (HasAuthority())
		gm = Cast<AMHGameMode>(GetWorld()->GetAuthGameMode());
}

void AHunterController::ServerRPC_RespawnPlayer_Implementation()
{
    auto player = GetPawn();
    if (player) {
        GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle); // 기존 타이머 제거
        GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, [=,this]() {
                UnPossess();
                player->Destroy();
                gm->RestartPlayer(this);
                ClientRPC_InitUIWidget();

            }, 2, false);
    }
}

void AHunterController::ClientRPC_InitUIWidget_Implementation()
{
    AHunter* Hunter = Cast<AHunter>(GetPawn());
    if (Hunter && Hunter->IsLocallyControlled())
    {
        Hunter->InitUIWidget();
    }
}
