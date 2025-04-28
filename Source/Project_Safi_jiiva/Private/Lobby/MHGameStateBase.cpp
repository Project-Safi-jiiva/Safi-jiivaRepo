// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/MHGameStateBase.h"
#include "Project_Safi_jiiva.h"
#include "Net/UnrealNetwork.h"

AMHGameStateBase::AMHGameStateBase()
{
    bReplicates = true;
}
void AMHGameStateBase::AddPlayerToParty_Implementation(const FString& PartyName, const FPartyMember& Member)
{
    NetMulticast_AddPlayerToParty(PartyName, Member);
}

void AMHGameStateBase::NetMulticast_AddPlayerToParty_Implementation(const FString& PartyName, const FPartyMember& Member)
{
    if (HasAuthority())
    {
        int32 PartyIndex = GetPartyIndexByName(PartyName);
        if (PartyIndex != -1)
        {
            FParty& Party = Parties[PartyIndex];
            if (!Party.Members.ContainsByPredicate([&](const FPartyMember& M) { return M.PlayerNetId == Member.PlayerNetId; }))
            {
                Party.Members.Add(Member);
                Party.MemberReadyStates.Add(false);

                // 배열을 새로 할당해서 Replication 트리거
                TArray<FParty> NewParties = Parties;
                Parties = NewParties;
            }
        }
    }
}


void AMHGameStateBase::CreateParty_Implementation(const FString& PartyName, const FPartyMember& Host)
{
    if (HasAuthority())
    {
        if (GetPartyIndexByName(PartyName) != -1)
        {
            PRINTLOG_NET(TEXT("Party %s already exists"), *PartyName);
            return;
        }

        FParty NewParty;
        NewParty.PartyName = PartyName;
        NewParty.Members.Add(Host);
        NewParty.MemberReadyStates.Add(false);

        TArray<FParty> NewParties = Parties; // 복사본 만든다
        NewParties.Add(NewParty);
        Parties = NewParties; // 할당해서 Replication 일으킨다

        PRINTLOG_NET(TEXT("Party Created: %s with Host: %s"), *PartyName, *Host.PlayerName);
    }
}

void AMHGameStateBase::SetPlayerReadyState_Implementation(const FString& PartyName, const FPartyMember& Member, bool bIsReady)
{
    //if (HasAuthority())
    {
        int32 PartyIndex = GetPartyIndexByName(PartyName);
        if (PartyIndex != -1)
        {
            FParty& Party = Parties[PartyIndex];
            int32 MemberIndex = Party.Members.IndexOfByPredicate([&](const FPartyMember& M) {
                return M.PlayerNetId == Member.PlayerNetId;
                });
            if (MemberIndex != INDEX_NONE)
            {
                Party.MemberReadyStates[MemberIndex] = bIsReady;
                UE_LOG(LogTemp, Log, TEXT("Player %s ready state set to %d in Party: %s"), *Member.PlayerName, bIsReady, *PartyName);
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Player %s not found in Party: %s"), *Member.PlayerName, *PartyName);
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Party %s not found"), *PartyName);
        }
    }
}


int32 AMHGameStateBase::GetPartyIndexByName(const FString& PartyName)
{
    for (int32 i = 0; i < Parties.Num(); ++i)
    {
        if (Parties[i].PartyName.Contains(PartyName))
        {
            return i;
        }
    }
    return -1; // 파티 없음
}

FParty AMHGameStateBase::GetPartyByIndex(int32 Index)
{
    if (Parties.IsValidIndex(Index))
    {
        return Parties[Index];
    }
    return FParty(); // 빈 FParty 반환
}

void AMHGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(AMHGameStateBase, Parties);
}

void AMHGameStateBase::OnRep_Parties()
{
    PRINTLOG_NET(TEXT("DDDDDDD:%d :DDDDDDDDDDDDDDDDDDDDDDDD"), Parties.Num());

}