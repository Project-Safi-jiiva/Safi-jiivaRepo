// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hunter/BaseComponent.h"
#include "WeaponInterface.h"
#include "EWeaponType.h"
#include "WeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_SAFI_JIIVA_API UWeaponComponent : public UBaseComponent,public IWeaponInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	EWeaponType WeaponType;

protected:
	// 기본 생성자

	//인터페이스 구현
	virtual void QuickStrike() override;

	virtual void HeavyStrike() override;

	virtual void UniqueStrike() override;

	virtual void ResetCombo() override;

public:

	bool IsAttacking;

	bool bNextAttackQueued;

	//EAttackType NextAttackType;

	class UAnimMontage* CurrentMontage;

	FTimerHandle ComboTimerHandle;

};
