// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WeaponInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class PROJECT_SAFI_JIIVA_API IWeaponInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void QuickStrikeStart()=0;
	virtual void QuickStrikeHolding()=0;
	virtual void QuickStrikeEnd()=0;
	virtual void QuickStrikeNext()=0;

	virtual void HeavyStrikeStart() = 0;
	virtual void HeavyStrikeEnd() = 0;

	virtual void UniqueStrikeStart() = 0;
	virtual void UniqueStrikeEnd() = 0;

	virtual void JumpToNextCombo() = 0;
	virtual void ResetCombo() = 0;

	virtual void CancelHandler() {};

};
