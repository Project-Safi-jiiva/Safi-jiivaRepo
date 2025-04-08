// AssetPaths.h
#pragma once

#include "CoreMinimal.h"

namespace AssetPaths
{
    //캐릭터 기본 세팅
    constexpr const TCHAR* HUNTER_MESH = TEXT("/Script/Engine.SkeletalMesh'/Game/LHW/asdasd/Gemma.Gemma'");
    constexpr const TCHAR* HUNTER_ANIM = TEXT("/Script/Engine.AnimBlueprint'/Game/LHW/Anim/ABP_Gemma.ABP_Gemma_C'");
    constexpr const TCHAR* HUNTER_IMC = TEXT("/Script/EnhancedInput.InputMappingContext'/Game/LHW/Input/IMC_Hunter.IMC_Hunter'");

    //무브 컴포넌트
    constexpr const TCHAR* IA_MOVE = TEXT("/Script/EnhancedInput.InputAction'/Game/LHW/Input/IA_Move.IA_Move'");
    constexpr const TCHAR* IA_TURN = TEXT("/Script/EnhancedInput.InputAction'/Game/LHW/Input/IA_Turn.IA_Turn'");

    //웨폰 컴포넌트
    constexpr const TCHAR* IA_Dash = TEXT("/Script/EnhancedInput.InputAction'/Game/LHW/Input/IA_Dash.IA_Dash'");

    constexpr const TCHAR* IA_QuickStrike = TEXT("/Script/EnhancedInput.InputAction'/Game/LHW/Input/IA_QuickStrike.IA_QuickStrike'");

    constexpr const TCHAR* IA_HeavyStrike = TEXT("/Script/EnhancedInput.InputAction'/Game/LHW/Input/IA_HeavyStrike.IA_HeavyStrike'");

    constexpr const TCHAR* IA_UniqueStrike = TEXT("/Script/EnhancedInput.InputAction'/Game/LHW/Input/IA_UniqueStrike.IA_UniqueStrike'");
}