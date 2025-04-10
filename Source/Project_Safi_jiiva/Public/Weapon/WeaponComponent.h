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


	virtual void ResetCombo() override;
protected:
	// 기본 생성자

	//인터페이스 구현
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void QuickStrikeStart() override;
	virtual void QuickStrikeEnd() override;

	virtual void HeavyStrikeStart() override;
	virtual void HeavyStrikeEnd() override;

	virtual void UniqueStrikeStart() override;
	virtual void UniqueStrikeEnd() override;


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

	bool IsAttacking =false;

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
private:
	int32 QuickStrikeComboIndex = 0;
	int32 HeavyStrikeComboIndex = 0;
	int32 UniqueStrikeComboIndex = 0;
public:
	int32 GetQuickStrikeComboIndex() const { return QuickStrikeComboIndex; }
	void SetQuickStrikeComboIndex(int32 NewIndex) {QuickStrikeComboIndex = NewIndex;}

	int32 GetHeavyStrikeComboIndex() const { return HeavyStrikeComboIndex; }
	void SetHeavyStrikeComboIndex(int32 NewIndex) { HeavyStrikeComboIndex = NewIndex; }

	int32 GetUniqueStrikeComboIndex() const { return UniqueStrikeComboIndex; }
	void SetUniqueStrikeComboIndex(int32 NewIndex) { UniqueStrikeComboIndex = NewIndex; }


};
