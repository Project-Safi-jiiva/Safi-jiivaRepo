// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hunter/BaseComponent.h"
#include "WeaponInterface.h"
#include "EWeaponType.h"
#include "WeaponDataTable.h"
#include "WeaponDataAsset.h"
#include "WeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_SAFI_JIIVA_API UWeaponComponent
	: public UBaseComponent
	, public IWeaponInterface
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


protected:
	// 기본 생성자

	//인터페이스 구현
	virtual void QuickStrike() override;

	virtual void HeavyStrike() override;

	virtual void UniqueStrike() override;

	virtual void ResetCombo() override;

	//부모 상속

	virtual void SetupInputBinding(class UEnhancedInputComponent* InputComponent) override;

public:
	void SetWeaponType(EWeaponType NewWeaponType);

private:
	void LoadWeaponData();
protected:
	void Dash();
	void DashEnd();

public:

	bool IsAttacking;

	bool bNextAttackQueued;

	//EAttackType NextAttackType;

	class UAnimMontage* CurrentMontage;

	FTimerHandle ComboTimerHandle;

public:
	class UInputAction* IA_Dash;
	class UInputAction* IA_QuickStrike;
	class UInputAction* IA_HeavyStrike;
	class UInputAction* IA_UniqueStrike;

protected:
	FWeaponDataTable GetCurrentWeaponData() const;

private:
    // 데이터 테이블 참조
    UPROPERTY(EditAnywhere, Category = "Weapon")
    UWeaponDataAsset* WeaponDataTable;

    // 모든 무기 데이터를 저장하는 TMap
    UPROPERTY()
    TMap<EWeaponType, FWeaponDataTable> WeaponDataMap;

	EWeaponType WeaponType;


};
