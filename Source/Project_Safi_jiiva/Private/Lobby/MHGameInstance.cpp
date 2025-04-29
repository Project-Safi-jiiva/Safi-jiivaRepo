// Fill out your copyright notice in the Description page of Project Settings.


#include "Lobby/MHGameInstance.h"
#include "Online/CoreOnlineFwd.h"
#include "Project_Safi_jiiva.h"
#include "../../../../Plugins/Online/OnlineSubsystem/Source/Public/Interfaces/OnlineSessionInterface.h"
#include "../../../../Plugins/Online/OnlineBase/Source/Public/Online/OnlineSessionNames.h"
#include "Hunter/HunterController.h"
#include "Lobby/MHGameStateBase.h"

void UMHGameInstance::Init()
{
	Super::Init();
	if (auto subsys = IOnlineSubsystem::Get()) {
		//서브시스템으로부터 세션 인터페이스 가져오기
		sessionInterface = subsys->GetSessionInterface();
		sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMHGameInstance::OnCreateSessionComplete);
		sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMHGameInstance::OnFindSessionsComplete);
		sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMHGameInstance::OnJoinSessionComplete);
	}
	mySessionName.Append(FString::Printf(TEXT("%_d_%d"), FMath::Rand32(), FDateTime::Now().GetMillisecond()));
}

void UMHGameInstance::CreateMySession()
{
	// 세션설정 변수
	FOnlineSessionSettings sessionSettings;

	// 1. Dedicated Server 접속 여부
	sessionSettings.bIsDedicated = false;

	// 2. 랜선(로컬)매칭을 할지 steam 매칭을 할지 여부
	FName subsysName = IOnlineSubsystem::Get()->GetSubsystemName();
	sessionSettings.bIsLANMatch = (subsysName == "NULL");

	// 3. 매칭이 온라인을 통해 노출될지 여부
	// false 이면 초대를 통해서만 입장이 가능
	// SendSessionInviteToFriend() 함수를 통해 친구를 초대할 수 있다.
	sessionSettings.bShouldAdvertise = true;

	// 4. 온라인 상태(Presence) 정보를 활용할지 여부
	sessionSettings.bUsesPresence = true;
	sessionSettings.bUseLobbiesIfAvailable = true;

	// 5. 게임진행중에 참여 허가할지 여부
	sessionSettings.bAllowJoinViaPresence = true;
	sessionSettings.bAllowJoinInProgress = true;

	// 6. 세션에 참여할 수 있는 공개(public) 연결의 최대 허용 수
	sessionSettings.NumPublicConnections = 4;

	// 7. 커스텀 룸네임 설정
	FString RandomRoomName = GenerateRandomRoomName(10); // 10자리 랜덤 문자열
	sessionSettings.Set(FName("ROOM_NAME"), RandomRoomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// 8. 호스트 네임 설정
	sessionSettings.Set(FName("HOST_NAME"), mySessionName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	// netID
	FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();

	PRINT_LOG(TEXT("Create Session Strat : %s"), *mySessionName);
	sessionInterface->CreateSession(*netID, FName(mySessionName), sessionSettings);
}

void UMHGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	PRINTLOG_NET(TEXT("SessionName : %s, bWasSuccessful : %d"), *SessionName.ToString(), bWasSuccessful);
	if (bWasSuccessful)
	{
		if(!LobbyGameState and GetWorld())
			LobbyGameState = Cast<AMHGameStateBase>(GetWorld()->GetGameState());

		FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();


		FString RoomName;
		if (!sessionInterface->GetSessionSettings(SessionName)->Get(FName("ROOM_NAME"), RoomName))
		{
			PRINT_LOG(TEXT("Failed to get ROOM_NAME for session: %s"), *SessionName.ToString());
			return;
		}
		GetWorld()->ServerTravel(TEXT("/Game/LHW/Map/KJY_TestMap?listen"));
		OnCreateSessionCompleted.Broadcast();
	}
}

void UMHGameInstance::FindOtherSession()
{
	sessionSearch = MakeShareable(new FOnlineSessionSearch());

	// 1. 세션 검색 조건 설정
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	// 2. Lan 여부
	sessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == FName("NULL");

	// 3. 최대 검색 세션 수
	sessionSearch->MaxSearchResults = 10;

	// 4. 세션검색
	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
}

void UMHGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	// 찾기 실패시
	if (!bWasSuccessful)
	{
		PRINT_LOG(TEXT("Session search failed..."));
		return;
	}

	// 세션검색결과 배열
	auto results = sessionSearch->SearchResults;
	PRINT_LOG(TEXT("Search Result Count : %d"), results.Num());

		FSessionInfo sessionInfo;
	for (int i = 0; i < results.Num(); ++i)
	{
		auto sr = results[i];

		if (sr.IsValid() == false) continue;

		// 세션정보 구조체선언
		sessionInfo.index = i;

		sr.Session.SessionSettings.Get(FName("ROOM_NAME"), sessionInfo.roomName);

		sr.Session.SessionSettings.Get(FName("HOST_NAME"), sessionInfo.hostName);
		// 입장가능한 플레이어 수
		int32 maxPlayerCount = sr.Session.SessionSettings.NumPublicConnections;
		// 현재 입장한 플레이어 수 ( 최대 - 현재 입장 가능한 수 )
		// NumOpenPublicConnections 스팀에서만 정상적으로 값이 들어온다.
		int32 currentPlayerCount = maxPlayerCount - sr.Session.NumOpenPublicConnections;
		sessionInfo.playerCount = FString::Printf(TEXT("(%d/%d)"), currentPlayerCount, maxPlayerCount);
		// 핑정보 (스팀에서는 9999로 나온다)
		int32 pingSpeed = sr.PingInMs;
		PRINT_LOG(TEXT("%s"), *sessionInfo.ToString());

		onSearchCompleted.Broadcast(sessionInfo);
	}
}

FString UMHGameInstance::GenerateRandomRoomName(int32 Length /*= 8*/)
{
	const FString Characters = TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
	FString RandomString;

	for (int32 i = 0; i < Length; ++i)
	{
		int32 RandomIndex = FMath::RandRange(0, Characters.Len() - 1);
		RandomString.AppendChar(Characters[RandomIndex]);
	}

	return RandomString;
}

void UMHGameInstance::JoinSelectedSession(int32 index)
{
	auto sr = sessionSearch->SearchResults;

	sr[index].Session.SessionSettings.bUseLobbiesIfAvailable = true;
	sr[index].Session.SessionSettings.bUsesPresence = true;

	sessionInterface->JoinSession(0, FName(mySessionName), sr[index]);
}

void UMHGameInstance::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	if (result == EOnJoinSessionCompleteResult::Success)
	{
		if (!LobbyGameState and GetWorld())
			LobbyGameState = Cast<AMHGameStateBase>(GetWorld()->GetGameState());
		AHunterController* pc = Cast<AHunterController>(GetWorld()->GetFirstPlayerController());
		FString url;
		sessionInterface->GetResolvedConnectString(sessionName, url);
		FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();



		FString RoomName;
		if (!sessionInterface->GetSessionSettings(sessionName)->Get(FName("ROOM_NAME"), RoomName))
		{
			PRINTLOG_NET(TEXT("Failed to get ROOM_NAME : %s"),*sessionName.ToString());
			return;
		}
		pc->ClientTravel(url, ETravelType::TRAVEL_Absolute);
	}
	else
	{
		PRINT_LOG(TEXT("Join Session failed : %d"), result);
	}
}

void UMHGameInstance::HandleJoinSessionRequested(int32 SessionIndex, const FString& RoomName)
{
		PRINT_LOG(TEXT("HandleJoinSessionRequested: Index=%d, RoomName=%s"), SessionIndex, *RoomName);
		PendingSessionIndex = SessionIndex;
		PendingRoomName = RoomName;
}
