// Fill out your copyright notice in the Description page of Project Settings.


#include "Hunter/HunterAnimNotifyState.h"
#include "Project_Safi_jiiva.h"
#include "Hunter/Hunter.h"
#include "Hunter/HunterAnim.h"
#include "Animation/AnimSequence.h"
#include "Weapon/WeaponComponent.h"



void UHunterAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
}

void UHunterAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime);
	AHunter* Owner = Cast<AHunter>(MeshComp->GetOwner());
	if (UHunterAnim* Anim = Cast<UHunterAnim>(MeshComp->GetAnimInstance())) {
		UAnimMontage* CurrentMontage = Anim->GetCurrentActiveMontage();
		FName CurrentSection = Anim->Montage_GetCurrentSection(CurrentMontage);
		if (CurrentSection.ToString().Contains(TEXT("Attack"))){
			PRINT_LOG(TEXT("ASDSAD"));
		}
		if (CurrentSection.ToString().Contains(TEXT("NextStart"))) {
			Owner->WeaponComp->QuickStrikeNext();
		}
		if (CurrentSection.ToString().Contains(TEXT("Next"))) {
			Owner->WeaponComp->QuickStrikeNext();
			Owner->WeaponComp->HeavyStrikeNext();
		}

		if (CurrentSection.ToString().Contains(TEXT("WeaponCollitionOn"))) {
			Owner->WeaponComp->WeaponCollitionOn();
		}
		if (CurrentSection.ToString().Contains(TEXT("WeaponCollitionOff"))) {
			Owner->WeaponComp->WeaponCollitionOff();
		}
	}

}

void UHunterAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
}
