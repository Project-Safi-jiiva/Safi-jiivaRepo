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

void UGreatSword::QuickStrikeNext()
{
	Super::QuickStrikeNext();
	if(!iscancel)
		SetQuickStrikeComboIndex(Owner->WeaponComp->GetQuickStrikeComboIndex() + 1);
	IsAttacking = false;
	if (isHolding)
		QuickAttack();
}

void UGreatSword::QuickInputHolding(FInputActionValue ActionValue, float ElapsedTime, float TriggeredTime, const UInputAction* SourceAction)
{
	Super::QuickInputHolding(ActionValue, ElapsedTime, TriggeredTime, SourceAction);
}

void UGreatSword::QuickInputEnd()
{
	Super::QuickInputEnd();
	if (isJumpDelay) return;
	if (isHolding) return;
	JumpToNextCombo();
}

void UGreatSword::HeavyInputHolding(const FInputActionValue& Value)
{
	Super::HeavyInputHolding(Value);
}

void UGreatSword::HeavyInputEnd()
{
	Super::HeavyInputEnd();
	//isHolding = false;
}

void UGreatSword::UniqueInputHolding(const FInputActionValue& Value)
{
	Super::UniqueInputHolding(Value);
}

void UGreatSword::UniqueInputEnd()
{
	Super::UniqueInputEnd();
}

void UGreatSword::checkCommand(float DeltaTime)
{
	Super::checkCommand(DeltaTime);
	if (FCommandInput[0] >0 || FCommandInput[1] >0 || FCommandInput[2] >0)
		CommandInputTime += DeltaTime;
	if (CommandInputTime >= 0.1) {
		//if (isCommandInput[0] && !isCommandInput[1] && !isCommandInput[2]) {
		//	QuickAttack();
		//}
		//if (!isCommandInput[0] && isCommandInput[1] && !isCommandInput[2]){
		//	HeavyAttack();
		//	}
		//if (isCommandInput[0] && isCommandInput[1] && !isCommandInput[2]){
		//	if (IsQuickAttack)
		//		HeavyAttack();
		//	else {

		//		CancelHandler();
		//		UniqueAttack();
		//	}
		//}
		IsCommandInputReset();
	}
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

void UGreatSword::Roll()
{
	FWeaponDataTable CurrentData = GetCurrentWeaponData();

	PlayMontage(CurrentData.DodgeMontage);
}

void UGreatSword::PlayMontage(UAnimMontage* Montage)
{
	if (Montage && Anim)
	{
		Anim->Montage_Play(Montage);
	}
}

void UGreatSword::QuickAttack()
{
	if (IsQuickAttack) return;
	if (iscancel)return; if (IsAttacking)return;
	FWeaponDataTable CurrentData = GetCurrentWeaponData();
	if (CurrentData.QuickStrikeMontages.Num() > 0 && Owner) {
		if (!isWeaponEquipped && Owner->GetVelocity().Size2D() <= 0) {
			PlayMontage(CurrentData.SheatheMontage);
			isWeaponEquipped = true;
			return;
		}
		if (!isWeaponEquipped && Owner->GetVelocity().Size2D() > 0) {
			PlayMontage(CurrentData.UniqueStrikeMontages[1]);
			isWeaponEquipped = true;
			return;
		}
		else {
			PlayMontage(CurrentData.QuickStrikeMontages[GetQuickStrikeComboIndex()]);
		}
	}
}

void UGreatSword::HeavyAttack()
{
	if (!isWeaponEquipped) return;
		FWeaponDataTable CurrentData = GetCurrentWeaponData();
		if (CurrentData.HeavyStrikeMontages.Num() > 0 && Owner)
		{
			PlayMontage(CurrentData.HeavyStrikeMontages[GetHeavyStrikeComboIndex()]);
		}
		return;
}

void UGreatSword::UniqueAttack()
{
	FWeaponDataTable CurrentData = GetCurrentWeaponData();
	PlayMontage(CurrentData.UniqueStrikeMontages[GetUniqueStrikeComboIndex()]);
}

