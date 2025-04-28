// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/LobbyGameMode.h"
#include "Project_Safi_jiiva.h"
#include "Net/UnrealNetwork.h"
#include "Lobby/MHGameStateBase.h"


ALobbyGameMode::ALobbyGameMode()
{
	bUseSeamlessTravel = false;
    GameStateClass = AMHGameStateBase::StaticClass();
    bReplicates = true;
    SetReplicates(true);
}
