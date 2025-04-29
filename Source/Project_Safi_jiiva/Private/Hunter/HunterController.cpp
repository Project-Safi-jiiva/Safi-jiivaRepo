// Fill out your copyright notice in the Description page of Project Settings.


#include "Hunter/HunterController.h"
#include "MHGameMode.h"

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
            if (IsValid(this) && IsValid(player) && gm) {
                UnPossess();
                player->Destroy();
                gm->RestartPlayer(this);
            }

            }, 2, false);
    }
}
