#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"

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