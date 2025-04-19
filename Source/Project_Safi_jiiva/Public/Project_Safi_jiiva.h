#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Logging/LogMacros.h"

// 1. 일반 로그 매크로
#define PRINT_LOG(Message, ...) \
    UE_LOG(LogTemp, Log, TEXT("[LOG] " Message), ##__VA_ARGS__)
// 사용 예시 : PRINT_LOG("플레이어 점수: %d", Score);
//
// 2. 인게임 로그 매크로 (화면 출력 포함)
#define PRINT_INGAME_LOG(Duration, Color, Message, ...) \
    do { \
        FString FormattedMsg = FString::Printf(TEXT("[INGAME] " Message), ##__VA_ARGS__); \
        UE_LOG(LogTemp, Log, TEXT("%s"), *FormattedMsg); \
        if (GEngine) { \
            GEngine->AddOnScreenDebugMessage(-1, Duration, Color, FormattedMsg); \
        } \
    } while (0)
// 사용 예시 : PRINT_INGAME_LOG(5.0f, FColor::Green, "플레이어 점수: %d", Score);

// 3. 캐릭터 상태 로그 매크로
#define PRINT_CHARACTER_STATUS(Character, Message, ...) \
    do { \
        if (Character) { \
            FString StatusMsg = FString::Printf( \
                TEXT("[STATUS] %s - HP: %.1f, Location: %s"), \
                *Character->CharacterName, Character->Health, *Character->GetActorLocation().ToString() \
            ); \
            UE_LOG(LogTemp, Log, TEXT("%s - " Message), *StatusMsg, ##__VA_ARGS__); \
        } else { \
            UE_LOG(LogTemp, Warning, TEXT("[STATUS] Character is null!")); \
        } \
    } while (0)
// 사용 예시 : PRINT_CHARACTER_STATUS(MyCharacter, "플레이어 상태 업데이트: %s", *MyCharacter->GetName());

#define LOCAL_ROLE UEnum::GetValueAsString(GetLocalRole())
#define REMOTE_ROLE UEnum::GetValueAsString(GetRemoteRole())

#define PRINT_NET_LOG \
{ \
	const FString conStr = GetNetConnection() != nullptr ? TEXT("Valid Connection") : TEXT("Invalid Connection"); \
	const FString ownerName = GetOwner() != nullptr ? GetOwner()->GetName() : TEXT("No Owner"); \
	const FString logStr = FString::Printf(TEXT("Connection : %s\nOwner Name : %s\nLocal Role : %s\nRemote Role : %s"), \
		*conStr, *ownerName, *LOCAL_ROLE, *REMOTE_ROLE); \
	DrawDebugString(GetWorld(), GetActorLocation() + FVector::UpVector * 100.0f, logStr, nullptr, FColor::White, 0, true, 1); \
}


DECLARE_LOG_CATEGORY_EXTERN(Project_Safi_jiiva_LOG, Log, All);
#define LOCAL_ROLE (UEnum::GetValueAsString<ENetRole>(GetLocalRole()))
#define REMOTE_ROLE (UEnum::GetValueAsString<ENetRole>(GetRemoteRole()))
#define NETMODE (GetWorld()->GetNetMode() == ENetMode::NM_Client ? TEXT("CLIENT") : GetWorld()->GetNetMode() == NM_Standalone ? TEXT("Standalone") : TEXT("Server"))
#define CALLINFO (FString(__FUNCTION__) + TEXT("( ") + FString::FromInt(__LINE__) + TEXT(" )"))
#define PRINTLOG_NET(fmt, ...) UE_LOG(Project_Safi_jiiva_LOG, Warning, TEXT("[%s]%s : %s"), NETMODE, *CALLINFO, *FString::Printf(fmt, ##__VA_ARGS__))
