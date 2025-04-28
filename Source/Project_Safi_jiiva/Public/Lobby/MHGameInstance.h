// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionSettings.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"
#include "MHGameInstance.generated.h"

USTRUCT(BlueprintType)
struct FSessionInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString roomName;
	UPROPERTY(BlueprintReadOnly)
	FString hostName;
	UPROPERTY(BlueprintReadOnly)
	FString playerCount;
	UPROPERTY(BlueprintReadOnly)
	int32 pingSpeed;
	UPROPERTY(BlueprintReadOnly)
	int32 index;

	inline FString ToString()
	{
		return FString::Printf(TEXT("[%d] %s : %s - %s, %dms"), index, *roomName, *hostName, *playerCount, pingSpeed);
	}
};
/**
 *
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSearchSignature, const FSessionInfo&, sessionInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCreateSessionCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJoinSessionCompleted);
UCLASS()
class PROJECT_SAFI_JIIVA_API UMHGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;

	FSearchSignature onSearchCompleted;

	FOnCreateSessionCompleted OnCreateSessionCompleted;
	FOnJoinSessionCompleted OnJoinSessionCompleted;

public:
	IOnlineSessionPtr sessionInterface;

	void CreateMySession();

	// 세션 호스트 이름
	FString mySessionName = "Hunter";

	UFUNCTION()
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	// 방검색
	TSharedPtr<FOnlineSessionSearch> sessionSearch;

	void FindOtherSession();

	void OnFindSessionsComplete(bool bWasSuccessful);
	FString GenerateRandomRoomName(int32 Length = 8);

	//세션 입장
	void JoinSelectedSession(int32 index);

	// 세션입장 콜백
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);



// PartyManager 액터
	UPROPERTY()
    class AMHGameStateBase* LobbyGameState;
	UFUNCTION()
    void HandleJoinSessionRequested(int32 SessionIndex, const FString& RoomName);

	int32 PendingSessionIndex;
	FString PendingRoomName;
};
