// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputActionValue.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputAction.h"
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
	virtual void QuickInputStart() {};
	virtual void QuickInputHolding(){};
	virtual void QuickInputEnd() {};

	virtual void HeavyInputStart() {};
	virtual void HeavyInputHolding(){};
	virtual void HeavyInputEnd() {};

	virtual void UniqueInputStart() {};
	virtual void UniqueInputHolding(){};
	virtual void UniqueInputEnd() {};

	virtual void checkCommand(float DeltaTime) = 0;
	virtual void QuickStrikeNext() {};
	virtual void JumpToNextCombo() {}


	virtual void ResetCombo() = 0;

	virtual void CancelHandler() {};

};
