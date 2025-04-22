// Fill out your copyright notice in the Description page of Project Settings.


#include "GreatSword.h"
#include "Hunter/HunterAnim.h"
#include "Project_Safi_jiiva.h"
#include "Hunter/Hunter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Hunter/MoveComponent.h"
#include "GreatSwordActor.h"

// Sets default values for this component's properties
UGreatSword::UGreatSword()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);

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
	if (IsQuickAttack)return;
	if (FCommandInput[0] <= 0.2)return;
	Owner->ServerRPC_ChargeAttack();
}

void UGreatSword::QuickInputHolding()
{
	Super::QuickInputHolding();
}

void UGreatSword::QuickInputEnd()
{
	Super::QuickInputEnd();
	if (isJumpDelay) return;
		JumpToNextCombo();
}

void UGreatSword::HeavyInputHolding()
{
	Super::HeavyInputHolding();
}

void UGreatSword::HeavyStrikeNext(const struct FWeaponDataTable& CurrentData)
{
	if (!CurrentData.WeaponActorClass) {
		PlayMontage(GetCurrentWeaponData().HeavyStrikeMontages[HeavyStrikeComboIndex]);
	}
	else {
		PlayMontage(CurrentData.HeavyStrikeMontages[HeavyStrikeComboIndex]);
	}
}

void UGreatSword::HeavyInputEnd()
{
	Super::HeavyInputEnd();
}

void UGreatSword::UniqueInputHolding()
{
	Super::UniqueInputHolding();
}

void UGreatSword::UniqueInputEnd()
{
	Super::UniqueInputEnd();
}

void UGreatSword::checkCommand(float DeltaTime)
{
	Super::checkCommand(DeltaTime);
	FWeaponDataTable CurrentData = GetCurrentWeaponData();
	if (isCommandInput[0] || isCommandInput[1] || isCommandInput[2])
		CommandInputTime += DeltaTime;
	if (CommandInputTime >= 0.15) {
		if (isCommandInput[0] && !isCommandInput[1] && !isCommandInput[2]&&!IsAttacking) {
			Owner->ServerRPC_QuickAttack();
		}
		if (!isCommandInput[0] && isCommandInput[1] && !isCommandInput[2] &&!IsAttacking){
			Owner->ServerRPC_HeavyAttack();
			}
		if (isCommandInput[0] && isCommandInput[1] && !isCommandInput[2]){
			if (FCommandInput[0] >= 0.2&&IsAttacking){

				Owner->ServerRPC_CancelHandler();

				//Owner->ServerPRC_SetHeavyAddIndex();
				if(GetQuickStrikeComboIndex()<2)
					Owner->ServerRPC_SetQuickAddIndex(GetQuickStrikeComboIndex() + 1);
				FCommandInput[0] = 0;
				IsCommandInputReset();
			}
			else {
				Owner->ServerRPC_UniqueAttack();



			}
		}
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
		FWeaponDataTable CurrentData = GetCurrentWeaponData();
		if (CurrentData.QuickStrikeMontages.Num() > 0 && Owner) {
			if (isWeaponEquipped) {
				PlayMontage(CurrentData.DrawMontage);
					isWeaponEquipped = false;
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
	Super::JumpToNextCombo();
	if (isCommandInput[0])return;
	CurrentMontage = Anim->GetCurrentMontage(Owner);
	//if (Anim->Montage_IsPlaying(CurrentMontage)) {
		Anim->Montage_JumpToSection(FName("Attack"), CurrentMontage);
		Owner->WeaponComp->SetHeavyStrikeComboIndex(0);
		return;
	//}
}

void UGreatSword::CancelHandler(const struct FWeaponDataTable& CurrentData)
{
	if (!CurrentData.WeaponActorClass) {
		PlayMontage(GetCurrentWeaponData().HeavyStrikeMontages[1]);
	}
	else {
		PlayMontage(CurrentData.HeavyStrikeMontages[1]);
	}

}

void UGreatSword::Roll()
{
		AllowRoll = false;

		FWeaponDataTable CurrentData = GetCurrentWeaponData();
		PlayMontage(CurrentData.DodgeMontage);

}

void UGreatSword::WeaponCollitionOn()
{
	if (Owner && Owner->HasAuthority()){

	AGreatSwordActor* Weapon = Cast<AGreatSwordActor>(EquippedWeapon);
	Weapon->SwordMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

}

void UGreatSword::WeaponCollitionOff()
{
	if (Owner && Owner->HasAuthority()) {

		AGreatSwordActor* Weapon = Cast<AGreatSwordActor>(EquippedWeapon);
		Weapon->SwordMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Weapon->HitPawn.Empty();
	}
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
	Super::QuickAttack();
	FWeaponDataTable CurrentData = GetCurrentWeaponData();
	if (CurrentData.QuickStrikeMontages.Num() > 0 && Owner) {
		 if (!isWeaponEquipped && Owner->GetVelocity().Size2D() <= 0) {
			PlayMontage(CurrentData.SheatheMontage);
			//if(Owner->HasAuthority())
				isWeaponEquipped = true;
			return;
		}
		 if (!isWeaponEquipped && Owner->GetVelocity().Size2D() > 0) {
			PlayMontage(CurrentData.UniqueStrikeMontages[1]);
			//if (Owner->HasAuthority())
				isWeaponEquipped = true;
			return;
		}
		 if (isWeaponEquipped) {
			 PRINT_LOG(TEXT("QuickAttack : %d"), IsAttacking);
			 if (IsAttacking) return;
			 ChargeAttack();
		 }
	}
}

void UGreatSword::HeavyAttack(const struct FWeaponDataTable& CrrentData)
{
	if (!isWeaponEquipped) return;
	if (IsAttacking) return;
	if (CrrentData.HeavyStrikeMontages.Num() > 0 && Owner) {
		if (!CrrentData.WeaponActorClass) {
			PlayMontage(GetCurrentWeaponData().HeavyStrikeMontages[GetHeavyStrikeComboIndex()]);
		}
		else {
			PRINTLOG_NET(TEXT("HeavyAttack : %d"), GetHeavyStrikeComboIndex());
			PlayMontage(CrrentData.HeavyStrikeMontages[GetHeavyStrikeComboIndex()]);
		}

	}
		return;
}

void UGreatSword::UniqueAttack()
{
	if (!isWeaponEquipped) return;
	if (IsAttacking) return;

	FWeaponDataTable CurrentData = GetCurrentWeaponData();
	PlayMontage(CurrentData.UniqueStrikeMontages[GetUniqueStrikeComboIndex()]);
}

void UGreatSword::ChargeAttack(const struct FWeaponDataTable& CurrentData)
{
	if (!CurrentData.WeaponActorClass) {
		PlayMontage(GetCurrentWeaponData().QuickStrikeMontages[GetQuickStrikeComboIndex()]);
	}
	else {
		PlayMontage(CurrentData.QuickStrikeMontages[GetQuickStrikeComboIndex()]);
	}
}

