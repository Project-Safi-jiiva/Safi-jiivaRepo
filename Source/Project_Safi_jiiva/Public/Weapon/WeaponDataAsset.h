// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EWeaponType.h"
#include "WeaponDataTable.h"
#include "WeaponDataAsset.generated.h"

/**
 *
 */
UCLASS()
class PROJECT_SAFI_JIIVA_API UWeaponDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Data")
	TMap<EWeaponType, FWeaponDataTable> WeaponDataMap;

};
