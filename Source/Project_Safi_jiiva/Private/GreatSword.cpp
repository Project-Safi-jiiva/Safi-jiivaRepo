// Fill out your copyright notice in the Description page of Project Settings.


#include "GreatSword.h"
#include "Hunter/HunterAnim.h"
#include "Project_Safi_jiiva.h"
#include "Hunter/Hunter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UGreatSword::UGreatSword()
{
	PrimaryComponentTick.bCanEverTick = true;
}
// Called when the game starts
void UGreatSword::BeginPlay()
{
	Super::BeginPlay();
}
void UGreatSword::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ModifyWeaponMoveSpeed();
	// ...
}

void UGreatSword::QuickStrikeStart()
{
	Super::QuickStrikeStart();

	if (iscancel)return;if (IsAttacking)return;
	FWeaponDataTable CurrentData = GetCurrentWeaponData();

	if (CurrentData.QuickStrikeMontages.Num() > 0 && Owner) {
		if (!isWeaponEquipped&&Owner->GetVelocity().Size2D()<=0) {
			PlayMontage(CurrentData.SheatheMontage);
			isWeaponEquipped = true;
			return;
		}
		if (!isWeaponEquipped && Owner->GetVelocity().Size2D() > 0) {
			PlayMontage(CurrentData.DodgeMontage);
			isWeaponEquipped = true;
			return;
		}
		else {
		PlayMontage(CurrentData.QuickStrikeMontages[GetQuickStrikeComboIndex()]);
		}
	}
}

void UGreatSword::QuickStrikeHolding()
{
	Super::QuickStrikeHolding();
}

void UGreatSword::QuickStrikeEnd()
{
	isHolding = false;
	Super::QuickStrikeEnd();
	if (isJumpDelay) return;
	if (isHolding) return;
	JumpToNextCombo();
}

void UGreatSword::QuickStrikeNext()
{
	Super::QuickStrikeNext();
	if(!iscancel)
		SetQuickStrikeComboIndex(Owner->WeaponComp->GetQuickStrikeComboIndex() + 1);
	IsAttacking = false;
	if (isHolding)
		QuickStrikeStart();
}

void UGreatSword::HeavyStrikeStart()
{
	Super::HeavyStrikeStart();
	if (!isWeaponEquipped) return;
	if (!IsAttacking){
		FWeaponDataTable CurrentData = GetCurrentWeaponData();
		if (CurrentData.HeavyStrikeMontages.Num() > 0 && Owner)
		{
			PlayMontage(CurrentData.HeavyStrikeMontages[GetHeavyStrikeComboIndex()]);
		}
		return;
	}
	else{
		if (iscancel)return;

		CancelHandler();
		return;
	}
}

void UGreatSword::HeavyStrikeEnd()
{
	//isHolding = false;
}

void UGreatSword::UniqueStrikeStart()
{
	Super::UniqueStrikeStart();
	if (!isWeaponEquipped) return;

	FWeaponDataTable CurrentData = GetCurrentWeaponData();
	if (CurrentData.UniqueStrikeMontages.Num() > 0 && Owner)
	{
		// 첫 번째 QuickStrike 몽타주 재생 (필요하면 인덱스 조정 가능)
		PlayMontage(CurrentData.UniqueStrikeMontages[0]);
	}
}

void UGreatSword::UniqueStrikeEnd()
{

}

void UGreatSword::ResetCombo()
{
	Super::ResetCombo();

}

void UGreatSword::Dash()
{
	Super::Dash();

	if(!IsAttacking){
	FWeaponDataTable CurrentData = GetCurrentWeaponData();
	if (CurrentData.QuickStrikeMontages.Num() > 0 && Owner){
		if (isWeaponEquipped) {
			PlayMontage(CurrentData.DrawMontage);
			isWeaponEquipped = false;
		}
	}
	}
}

void UGreatSword::ModifyWeaponMoveSpeed()
{
	if (!isWeaponEquipped) {
		Super::ModifyWeaponMoveSpeed();
	}
	else {
		Owner->GetCharacterMovement()->MaxWalkSpeed = 200;
	}
}

void UGreatSword::JumpToNextCombo()
{
	if (isHolding)return;
	CurrentMontage = Anim->GetCurrentMontage(Owner);
	if (Anim->Montage_IsPlaying(CurrentMontage)) {
		Anim->Montage_JumpToSection(FName("Attack"), CurrentMontage);
		Owner->WeaponComp->SetHeavyStrikeComboIndex(0);
	}
}

void UGreatSword::CancelHandler()
{
	if (!isHolding)return;
	FWeaponDataTable CurrentData = GetCurrentWeaponData();
	if (CurrentData.HeavyStrikeMontages.Num() > 0 && Owner)
	{
		PlayMontage(CurrentData.HeavyStrikeMontages[1]);
	}
	SetHeavyStrikeComboIndex(0);
}

void UGreatSword::PlayMontage(UAnimMontage* Montage)
{
	if (Montage && Anim)
	{
		Anim->Montage_Play(Montage);
	}
}

