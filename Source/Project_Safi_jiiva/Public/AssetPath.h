// AssetPaths.h
#pragma once

#include "CoreMinimal.h"

namespace AssetPaths
{
    //캐릭터 기본 세팅
    constexpr const TCHAR* HUNTER_MESH = TEXT("/Script/Engine.SkeletalMesh'/Game/LHW/Gemma/Gemma_RE_ColorSkin.Gemma_RE_ColorSkin'");
    constexpr const TCHAR* HUNTER_ANIM = TEXT("/Script/Engine.AnimBlueprint'/Game/LHW/Gemma/ABP_Gemma.ABP_Gemma_C'");
    constexpr const TCHAR* HUNTER_IMC = TEXT("/Script/EnhancedInput.InputMappingContext'/Game/LHW/Input/IMC_Hunter.IMC_Hunter'");

    //무브 컴포넌트
    constexpr const TCHAR* IA_MOVE = TEXT("/Script/EnhancedInput.InputAction'/Game/LHW/Input/IA_Move.IA_Move'");
    constexpr const TCHAR* IA_TURN = TEXT("/Script/EnhancedInput.InputAction'/Game/LHW/Input/IA_Turn.IA_Turn'");

    //웨폰 컴포넌트
    constexpr const TCHAR* IA_Dash = TEXT("/Script/EnhancedInput.InputAction'/Game/LHW/Input/IA_Dash.IA_Dash'");

    constexpr const TCHAR* IA_QuickStrike = TEXT("/Script/EnhancedInput.InputAction'/Game/LHW/Input/IA_QuickStrike.IA_QuickStrike'");

    constexpr const TCHAR* IA_HeavyStrike = TEXT("/Script/EnhancedInput.InputAction'/Game/LHW/Input/IA_HeavyStrike.IA_HeavyStrike'");

    constexpr const TCHAR* IA_UniqueStrike = TEXT("/Script/EnhancedInput.InputAction'/Game/LHW/Input/IA_UniqueStrike.IA_UniqueStrike'");
    constexpr const TCHAR* IA_ROLL = TEXT("/Script/EnhancedInput.InputAction'/Game/LHW/Input/IA_Roll.IA_Roll'");
    //데이터 에셋
    constexpr const TCHAR* WeaponDataAsset = TEXT("/Script/Project_Safi_jiiva.WeaponDataAsset'/Game/LHW/DataTable/WeaponData.WeaponData'");

    //무기 MESH
    constexpr const TCHAR* GREATSWORDMESH = TEXT("/Script/Engine.StaticMesh'/Game/LHW/Sword/GreatSword.GreatSword'");

    //무기 파티클
    constexpr const TCHAR* GREATSWORDMESHPARTICLE = TEXT("/Script/Engine.ParticleSystem'/Game/Stylized_Mobile_Effects/Particles/P_SwordSlash_1.P_SwordSlash_1'");
}