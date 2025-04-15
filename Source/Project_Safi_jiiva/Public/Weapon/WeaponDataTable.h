// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "WeaponDataTable.generated.h"

USTRUCT(BlueprintType)
struct FWeaponDataTable : public FTableRowBase
{
    GENERATED_BODY()

    //무기 엑터
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Actor")
    TSubclassOf<AActor> WeaponActorClass;

    // 무기 데미지 (기본 데미지)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
    float BaseDamage;

    // QuickStrike 몽타주 (3개)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
    TArray<UAnimMontage*> QuickStrikeMontages;

    // HeavyStrike 몽타주 (3개)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
    TArray<UAnimMontage*> HeavyStrikeMontages;

    // UniqueStrike 몽타주 (3개)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
    TArray<UAnimMontage*> UniqueStrikeMontages;

    // 발도 몽타주 (1개)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
    UAnimMontage* DrawMontage;

    // 납도 몽타주 (1개)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
    UAnimMontage* SheatheMontage;

    // QuickStrike 데미지 배율 (3개)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
    TArray<float> QuickStrikeDamageMultipliers;

    // HeavyStrike 데미지 배율 (3개)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
    TArray<float> HeavyStrikeDamageMultipliers;

    // UniqueStrike 데미지 배율 (3개)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
    TArray<float> UniqueStrikeDamageMultipliers;

    // 가드 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
    UAnimMontage* GuardMontage;

    // 구르기 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
    UAnimMontage* DodgeMontage;

    // 피격 애니메이션
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animations")
    UAnimMontage* HitReactionMontage;

    // 추가된 항목: 스태미나 소모량 (Quick, Heavy, Unique 공격별로 배열)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
    TArray<float> StaminaCosts;

    // 추가된 항목: 효과음 (여러 개를 배열로)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    TArray<USoundBase*> AttackSounds;

    // 추가된 항목: 파티클 이펙트 (여러 개를 배열로)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
    TArray<UParticleSystem*> HitEffects;
};