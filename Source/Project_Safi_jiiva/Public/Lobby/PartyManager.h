// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Hunter/HunterController.h"
#include "PartyManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPartyUpdated);

USTRUCT()
struct FPartyMember
{
    GENERATED_BODY()

    FString PlayerName;
    bool bIsReady = false;
    APlayerController* PlayerController = nullptr;

    FString ToString() const
    {
        return FString::Printf(TEXT("[%s] Ready: %d"), *PlayerName, bIsReady);
    }
};

UCLASS()
class PROJECT_SAFI_JIIVA_API UPartyManager : public UObject
{
    GENERATED_BODY()

public:
    void AddMember(const FString& PlayerName, AHunterController* PC);
    void RemoveMember(const FString& PlayerName);
    void SetReady(const FString& PlayerName, bool bReady);
    void ForceStartGame();
    void ResetParty();

    UPROPERTY(BlueprintAssignable)
    FOnPartyUpdated OnPartyUpdated;

private:
    TArray<FPartyMember> PartyMembers;
    int32 MaxPartySize = 4; // 파티 최대 4명
};
