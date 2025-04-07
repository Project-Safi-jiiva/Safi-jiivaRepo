// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 *
 */
UENUM(BlueprintType)
enum class EWeaponType:uint8
{
	NONE UMETA(DisplayName = "NONE"),
	GREATSWORD UMETA(DisplayName = "GREATSWORD"),
	BOW UMETA(DisplayName = "BOW"),
	MAX
};