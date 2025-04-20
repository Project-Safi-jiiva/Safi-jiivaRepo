// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Hunter/BaseComponent.h"
#include "WeaponInterface.h"
#include "EWeaponType.h"
#include "WeaponDataTable.h"
#include "WeaponDataAsset.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputActionValue.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "WeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_SAFI_JIIVA_API UWeaponComponent
	: public UBaseComponent
	, public IWeaponInterface
{
	GENERATED_BODY()

public:
	UWeaponComponent();

	//베이스 오버라이드 함수
protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//인풋 함수 모음
public:
	virtual void InputDash();
	void InputDashEnd();
	void InputQuickStart();
	void InputQuickHolding();
	void InputQuickEnd();
	void InputHeavyStart();
	void InputHeavyHolding();
	void InputHeavyEnd();
	void InputUniqueStart();
	void InputUniqueHolding();
	void InputUniqueEnd();
	void InputRoll();


	//기능 구현 함수 모음
	//데이터 로드
	void LoadWeaponData();
	//대쉬
	virtual void Dash();
	virtual void DashEnd();
	//약공격
	virtual void QuickInputStart();
	virtual void QuickInputHolding();
	virtual void QuickInputEnd();
	virtual void QuickAttack() {};
	//강공격
	virtual void HeavyInputStart();
	virtual void HeavyInputHolding();
	virtual void HeavyInputEnd();
	virtual void HeavyAttack() {};
	virtual void UniqueAttack() {};
	virtual void ChargeAttack() {};


public:
	virtual void ResetCombo() override;

protected:

	virtual void UniqueInputStart() override;
	virtual void UniqueInputHolding()override;
	virtual void UniqueInputEnd() override;

	virtual void checkCommand(float DeltaTime) override;
public:
	virtual void QuickStrikeNext() override;
	virtual void HeavyStrikeNext();
	virtual void JumpToNextCombo() override;
	virtual void CancelHandler() override;
	//부모 상속

	virtual void SetupInputBinding(class UEnhancedInputComponent* InputComponent) override;
	virtual void ModifyWeaponMoveSpeed() override;

public:
	void SpawnWeaponActor();
	bool SpawnNewWeaponActor(const FWeaponDataTable& WeaponData);
	void DestroyEquippedWeapon();
	void AttachWeaponToOwner();
	void AttachWeaponToHand();
	void InitializeWeaponActor(AActor* WeaponActor, const FWeaponDataTable& WeaponData);
	virtual void Roll();
public:
	AActor* EquippedWeapon;

protected:
	bool bNextAttackQueued;

	class UAnimMontage* CurrentMontage;

	FTimerHandle ComboTimerHandle;


public:
	class UInputAction* IA_Dash;
	class UInputAction* IA_QuickStrike;
	class UInputAction* IA_HeavyStrike;
	class UInputAction* IA_UniqueStrike;
	class UInputAction* IA_Roll;

protected:
	FWeaponDataTable GetCurrentWeaponData() const;

private:
    // 데이터 테이블 참조
    UPROPERTY(EditAnywhere, Category = "Weapon")
    UWeaponDataAsset* WeaponDataTable;

    // 모든 무기 데이터를 저장하는 TMap
public:
	int32 GetQuickStrikeComboIndex() const { return QuickStrikeComboIndex; }
	void SetQuickStrikeComboIndex(int32 NewIndex) {QuickStrikeComboIndex = NewIndex;}


	int32 GetHeavyStrikeComboIndex() const { return HeavyStrikeComboIndex; }
	void SetHeavyStrikeComboIndex(int32 NewIndex) {HeavyStrikeComboIndex = NewIndex;}

	int32 GetUniqueStrikeComboIndex() const { return UniqueStrikeComboIndex; }
	void SetUniqueStrikeComboIndex(int32 NewIndex) { UniqueStrikeComboIndex = NewIndex; }

	EWeaponType GetWeaponType() { return WeaponType; };
	void SetWeaponType(EWeaponType NewWeaponType) { WeaponType = NewWeaponType; }

	bool GetisWeaponEquipped() { return isWeaponEquipped; }



		void IsCommandInputReset();
		public:
		virtual void WeaponCollitionOn();
		virtual void WeaponCollitionOff();
		UFUNCTION()
		float SetDamage();
		float Damage=0.0f;

		//커맨드 판단 변수 선언
		TMap<EWeaponType, FWeaponDataTable> WeaponDataMap;
	protected:
		UPROPERTY(Replicated)
		int32 QuickStrikeComboIndex = 0;
		UPROPERTY(Replicated)
		int32 HeavyStrikeComboIndex = 0;
		UPROPERTY(Replicated)
		int32 UniqueStrikeComboIndex = 0;
		UPROPERTY(Replicated)
		EWeaponType WeaponType;
		UPROPERTY(Replicated)
		float CommandInputTime = 0.0f;
		UPROPERTY(Replicated)
		TArray<float> FCommandInput = { 0,0,0 };
		UPROPERTY(Replicated)
		TArray<bool> isCommandInput = { false,false,false };
	public:
		UPROPERTY(ReplicatedUsing=OnRep_IsWeaponEquipped)
		bool isWeaponEquipped = false;
		UFUNCTION()
		void OnRep_IsWeaponEquipped();
		UPROPERTY(Replicated)
		bool IsAttacking =false;
		UPROPERTY(Replicated)
		bool isJumpDelay = false;
		UPROPERTY(Replicated)
		bool iscancel = false;
		UPROPERTY(Replicated)
		bool IsQuickAttack =false;
		UPROPERTY(Replicated)
		bool IsHeavyAttack =false;
		UPROPERTY(Replicated)
		bool IsUniqueAttack =false;
		UPROPERTY(Replicated)
		bool AllowRoll = false;
		UPROPERTY(Replicated)
		bool isTacle = false;
		UPROPERTY(Replicated)
		bool isHolding = false;
	public:
		void SetIsAttacking(bool NewIsAttacking) { IsAttacking = NewIsAttacking; }
		void SetIsQuickAttack(bool NewIsQuickAttack) { IsQuickAttack = NewIsQuickAttack; }
		void SetIsHeavyAttack(bool NewIsHeavyAttack) { IsHeavyAttack = NewIsHeavyAttack; }
		void SetIsUniqueAttack(bool NewIsUniqueAttack) { IsUniqueAttack = NewIsUniqueAttack; }
		void SetIsTacle(bool NewIsTacle) { isTacle = NewIsTacle; }
		void SetIsHolding(bool NewIsHolding) { isHolding = NewIsHolding; }
		void SetIsJumpDelay(bool NewIsJumpDelay) { isJumpDelay = NewIsJumpDelay; }
		void SetAllowRoll(bool NewAllowRoll) { AllowRoll = NewAllowRoll; }

//////////////////////서버/////////////////////
		void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
};
