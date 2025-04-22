// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Weapon/WeaponComponent.h"
#include "GreatSword.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
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
protected:
	virtual void QuickInputHolding() override;
	virtual void QuickInputEnd() override;
	virtual void HeavyInputHolding() override;
	virtual void HeavyStrikeNext(const struct FWeaponDataTable& CurrentData) override;
	virtual void HeavyInputEnd() override;
	virtual void UniqueInputHolding() override;
	virtual void UniqueInputEnd() override;
	virtual void checkCommand(float DeltaTime) override;
	virtual void QuickStrikeNext() override;

	virtual void ResetCombo() override;
	virtual void Dash()override;

	virtual void ModifyWeaponMoveSpeed() override;

	virtual void CancelHandler() override;
	virtual void Roll() override;

	virtual void WeaponCollitionOn() override;
	virtual void WeaponCollitionOff() override;

private:
	void PlayMontage(UAnimMontage* Montage);

	virtual void QuickAttack() override;
	virtual void HeavyAttack(const struct FWeaponDataTable& CrrentData) override;
	virtual void UniqueAttack() override;

	virtual void ChargeAttack(const struct FWeaponDataTable& CurrentData={}) override;

private:
	void JumpToNextCombo();

};
