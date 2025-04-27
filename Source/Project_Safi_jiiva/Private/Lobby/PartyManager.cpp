// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/PartyManager.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Project_Safi_jiiva.h"

void UPartyManager::AddMember(const FString& PlayerName, AHunterController* PC)
{
    if (PartyMembers.Num() >= MaxPartySize)
    {
        PRINTLOG_NET(TEXT("pull Party"));
        return;
    }

    FPartyMember NewMember;
    NewMember.PlayerName = PlayerName;
    NewMember.bIsReady = false;
    NewMember.PlayerController = PC;

    PartyMembers.Add(NewMember);
    OnPartyUpdated.Broadcast();
}

void UPartyManager::RemoveMember(const FString& PlayerName)
{
    PartyMembers.RemoveAll([&](const FPartyMember& Member)
        {
            return Member.PlayerName == PlayerName;
        });

    OnPartyUpdated.Broadcast();
}

void UPartyManager::SetReady(const FString& PlayerName, bool bReady)
{
    for (FPartyMember& Member : PartyMembers)
    {
        if (Member.PlayerName == PlayerName)
        {
            Member.bIsReady = bReady;
            break;
        }
    }

    OnPartyUpdated.Broadcast();
}

void UPartyManager::ForceStartGame()
{
    // 준비 안한 사람 킥
    for (int32 i = PartyMembers.Num() - 1; i >= 0; --i)
    {
        if (!PartyMembers[i].bIsReady)
        {
            if (PartyMembers[i].PlayerController)
            {
                PRINTLOG_NET(TEXT("%s is Not Ready Kick."), *PartyMembers[i].PlayerName);

                // 킥맵으로 보내거나 세션 끊기
                PartyMembers[i].PlayerController->ClientTravel(TEXT("/Game/LHW/Map/KickMap"), ETravelType::TRAVEL_Absolute);

                // 또는 강제 접속 종료
                // PartyMembers[i].PlayerController->Destroy();
            }

            PartyMembers.RemoveAt(i);
        }
    }

    if (PartyMembers.Num() > 0)
    {
        GetWorld()->ServerTravel(TEXT("/Game/LHW/Map/KJY_TestMap?listen"));
    }
    else
    {
        PRINTLOG_NET(TEXT("Not Ready."));
    }
}

void UPartyManager::ResetParty()
{
    PartyMembers.Empty();
    OnPartyUpdated.Broadcast();
}