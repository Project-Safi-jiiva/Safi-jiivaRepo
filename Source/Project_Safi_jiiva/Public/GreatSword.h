// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon/WeaponComponent.h"
#include "GreatSword.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_SAFI_JIIVA_API UGreatSword : public UWeaponComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGreatSword();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void QuickStrike() override;
	virtual void HeavyStrike() override;
	virtual void UniqueStrike() override;
	virtual void ResetCombo() override;

private:
	void PlayMontage(UAnimMontage* Montage);


private:
	int32 QuickStrikeComboIndex = 0;
	int32 HeavyStrikeComboIndex = 0;
	int32 UniqueStrikeComboIndex = 0;

};
