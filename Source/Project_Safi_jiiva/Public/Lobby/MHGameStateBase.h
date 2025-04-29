// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MHGameStateBase.generated.h"
USTRUCT(BlueprintType)
struct FPartyMember
{
    GENERATED_BODY()
public:

    UPROPERTY(BlueprintReadOnly)
    FString PlayerName;

    UPROPERTY(BlueprintReadOnly)
    FUniqueNetIdRepl PlayerNetId; // 네트워크 ID
};

USTRUCT(BlueprintType)
struct FParty
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadOnly)
    FString PartyName; // 파티 이름 (ROOM_NAME)

    UPROPERTY(BlueprintReadOnly)
    TArray<FPartyMember> Members; // 파티원 목록

    UPROPERTY(BlueprintReadOnly)
    TArray<bool> MemberReadyStates; // 파티원 준비 여부
};

UCLASS()
class PROJECT_SAFI_JIIVA_API AMHGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
    AMHGameStateBase();
		public:
    UFUNCTION(BlueprintCallable, Server, Reliable)
    void CreateParty(const FString& PartyName, const FPartyMember& Host);

    // 파티에 플레이어 추가
    UFUNCTION(BlueprintCallable, Server, Reliable)
	void AddPlayerToParty(const FString& PartyName, const FPartyMember& Member);    
    UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void NetMulticast_AddPlayerToParty(const FString& PartyName, const FPartyMember& Member);


    // 파티원 준비 상태 설정
    UFUNCTION(BlueprintCallable, Server, Reliable)
    void SetPlayerReadyState(const FString& PartyName, const FPartyMember& Member, bool bIsReady);

    // 파티 인덱스 조회
    UFUNCTION(BlueprintCallable)
    int32 GetPartyIndexByName(const FString& PartyName);

    // 인덱스로 파티 조회
    UFUNCTION(BlueprintCallable)
    FParty GetPartyByIndex(int32 Index);

    // 모든 파티 목록 반환
    UFUNCTION(BlueprintCallable)
    const TArray<FParty>& GetAllParties() const { return Parties; }

protected:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
    UPROPERTY(ReplicatedUsing=OnRep_Parties)
    TArray<FParty> Parties;
    UFUNCTION()
    void OnRep_Parties();
};
