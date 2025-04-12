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

	virtual void QuickStrikeStart() override;
	virtual void QuickStrikeHolding() override;
	virtual void QuickStrikeEnd() override;
	virtual void QuickStrikeNext() override;
	virtual void HeavyStrikeStart() override;
	virtual void HeavyStrikeEnd() override;
	virtual void UniqueStrikeStart() override;
	virtual void UniqueStrikeEnd() override;
	virtual void ResetCombo() override;
	virtual void Dash() override;
	virtual void ModifyWeaponMoveSpeed() override;

	virtual void JumpToNextCombo() override;

	virtual void CancelHandler() override;

private:
	void PlayMontage(UAnimMontage* Montage);




};
