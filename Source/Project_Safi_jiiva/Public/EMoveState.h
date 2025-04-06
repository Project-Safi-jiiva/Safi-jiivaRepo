// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 *
 */
UENUM(BlueprintType)
enum class EMoveState:uint8
{
	IDLE UMETA(DisplayName = "IDLE"),
	START UMETA(DisplayName = "START"),
	WALK UMETA(DisplayName = "RUN"),
	RUN  UMETA(DisplayName = "START"),
	STOP UMETA(DisplayName = "STOP"),
	MAX
};