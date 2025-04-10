// Fill out your copyright notice in the Description page of Project Settings.


#include "GreatSword.h"
#include "Hunter/HunterAnim.h"
#include "Project_Safi_jiiva.h"
#include "Hunter/Hunter.h"

// Sets default values for this component's properties
UGreatSword::UGreatSword()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}
// Called when the game starts
void UGreatSword::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UGreatSword::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UGreatSword::QuickStrikeStart()
{
	Super::QuickStrikeStart();
	//if (IsAttacking)return;

	FWeaponDataTable CurrentData = GetCurrentWeaponData();
	if (CurrentData.QuickStrikeMontages.Num() > 0 && Owner)
	{
		PlayMontage(CurrentData.QuickStrikeMontages[GetQuickStrikeComboIndex()]);
		SetHeavyStrikeComboIndex(1);
	}
}

void UGreatSword::QuickStrikeEnd()
{
	CurrentMontage = Anim->GetCurrentMontage(Owner);
	if (Anim->Montage_IsPlaying(CurrentMontage)) {
		Anim->Montage_JumpToSection(FName("Attack"), CurrentMontage);
		Owner->WeaponComp->SetHeavyStrikeComboIndex(0);

	}
}

void UGreatSword::HeavyStrikeStart()
{
	//if (IsAttacking&& GetHeavyStrikeComboIndex()!=1)return;

	FWeaponDataTable CurrentData = GetCurrentWeaponData();
	if (CurrentData.HeavyStrikeMontages.Num() > 0 && Owner)
	{
		PlayMontage(CurrentData.HeavyStrikeMontages[GetHeavyStrikeComboIndex()]);
	}
}

void UGreatSword::HeavyStrikeEnd()
{

}

void UGreatSword::UniqueStrikeStart()
{
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
	// PlayMontage(WeaponData->QuickStrikeMontages[QuickStrikeComboIndex]);
	// PlayMontage(WeaponData->HeavyStrikeMontages[HeavyStrikeComboIndex]);
	// PlayMontage(WeaponData->UniqueStrikeMontages[UniqueStrikeComboIndex]);

}

void UGreatSword::PlayMontage(UAnimMontage* Montage)
{
	if (Montage && Anim)
	{
		Anim->Montage_Play(Montage);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to play montage: Invalid Hunter or Montage"));
	}
}

