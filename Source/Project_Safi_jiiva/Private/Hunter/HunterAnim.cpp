// Fill out your copyright notice in the Description page of Project Settings.


#include "Hunter/HunterAnim.h"
#include "Hunter/Hunter.h"
#include "Project_Safi_jiiva.h"
#include "Hunter/MoveComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon/WeaponComponent.h"
#include "Components/CapsuleComponent.h"

void UHunterAnim::NativeBeginPlay()
{
	Owner = Cast<AHunter>(TryGetPawnOwner());
    OnPlayMontageNotifyBegin.AddDynamic(this, &UHunterAnim::OnMontageNotifyBegin);
    OnPlayMontageNotifyEnd.AddDynamic(this, &UHunterAnim::OnMontageNotifyEnd);

}

void UHunterAnim::NativeUpdateAnimation(float DeltaTime)
{
    SetBluePrintValues();

	switch (MoveState)
	{
    case EMoveState::IDLE:break;

    case EMoveState::START: {
        if (isStart)return;
        isStart = true;
        WalkAngleStart = CalculateDirection(Owner->GetVelocity());

    }break;
	case EMoveState::WALK: {
        if (CalculateDirection(Owner->GetVelocity())>100|| CalculateDirection(Owner->GetVelocity()) < -100) {
            WalkAngleStart = CalculateDirection(Owner->GetVelocity());

            MoveState = EMoveState::TURN;
        }
    }break;
        case EMoveState::STOP: {
        isStart = false;
        SpeedLerpTime = 0; }break;
	default:
		break;
	}
}

void UHunterAnim::OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
    if (Owner->IsLocallyControlled()){
        //Owner->DelayTime = 0.15f;
        if (NotifyName == FName(TEXT("Delay"))) { Owner->DelayTime = 0.5f; PRINTLOG_NET(TEXT("Delay")); }
    }
    if (!Owner->IsLocallyControlled())return;
    //if (!Owner->HasAuthority())return;
    Owner->ServerRPC_SetIsJumpDelay(true);
    Owner->ServerRPC_SetIsAttacking(true);

    if (NotifyName == FName(TEXT("AttackEnd"))) { Owner->ServerRPC_SetIsAttacking(false); }
    //노티파이 공통 부분
    if (NotifyName == FName(TEXT("ComboEnd"))) {
        Owner->ServerRPC_ResetCombo();
    }
    //약공격 노티파이
    if (NotifyName == FName(TEXT("QuickAttackStart"))) { Owner->ServerRPC_SetIsQuickAttack(true); }
    if (NotifyName == FName(TEXT("QuickAttackAddIndex"))) {
        Owner->ServerRPC_SetQuickAddIndex(Owner->WeaponComp->GetQuickStrikeComboIndex() + 1);
    }

    if (NotifyName == FName(TEXT("QuickAttackEnd"))) { Owner->ServerRPC_SetIsQuickAttack(false); }

    ////강공격 노티파이
    if (NotifyName == FName(TEXT("HeavyAttackAddIndex"))) {
        Owner->ServerPRC_SetHeavyAddIndex(Owner->WeaponComp->GetHeavyStrikeComboIndex() + 1);
    }
    if (NotifyName == FName(TEXT("HeavyAttackStart"))) { Owner->ServerRPC_SetIsHeavyAttack(true); }

    //특수공격 노티파이
    if (NotifyName == FName(TEXT("UniqueAttackStart"))) { Owner->ServerRPC_SetIsUniqueAttack(true); }

    //구르기, 캔슬 노티파이
    if (NotifyName == FName(TEXT("Roll"))) { Owner->ServerRPC_SetAllowRoll(false); }
    if (NotifyName == FName(TEXT("iscancelEnd"))) { Owner->WeaponComp->iscancel = false;}


    //중복 입력 방지 부분
    if (NotifyName == FName(TEXT("DelayEnd"))) {
            Owner->ServerRPC_SetIsJumpDelay(false);
            Owner->ServerRPC_JumpToNextCombo();

    }
    if (NotifyName == FName(TEXT("JumpDelay"))) {
            Owner->ServerRPC_SetIsJumpDelay(true);
    }
    //무기 붙이고 때기
    if (NotifyName == FName(TEXT("Attach"))) { Owner->ServerRPC_AttachWeaponToHand();}
    if (NotifyName == FName(TEXT("Detach"))) {Owner->ServerRPC_AttachWeaponToOwner();}
    if (NotifyName == FName(TEXT("WeaponCollitionOn"))) {Owner->ServerRPC_OnWeaponComp();}
    if (NotifyName == FName(TEXT("WeaponCollitionOff"))) {Owner->ServerRPC_OffWeaponComp(); }
    if (NotifyName == FName(TEXT("TacleOn"))) { Owner->ServerRPC_SetIsTacle(true); }


	if (NotifyName == FName(TEXT("OffCollision"))) {
		Owner->GetCapsuleComponent()->SetCollisionProfileName(FName("Pawn"));
	}
	if (NotifyName == FName(TEXT("OnCollision"))) {
		Owner->GetCapsuleComponent()->SetCollisionProfileName(FName("Pawn2"));
	}
    if (NotifyName == FName(TEXT("Roll"))) {
        Owner->ServerRPC_SetAllowRoll(true);
    }
    if (NotifyName == FName(TEXT("AttackStart"))) {
        Owner->ServerRPC_SetAllowRoll(false);
    }
    if (NotifyName == FName(TEXT("AttackEnd"))) {
        Owner->ServerRPC_SetQuickAddIndex(0);
        Owner->ServerPRC_SetHeavyAddIndex(0);
        Owner->ServerRPC_SetIsJumpDelay(false);
        Owner->ServerRPC_SetIsQuickAttack(false);
        Owner->ServerRPC_SetIsHeavyAttack(false);
        Owner->ServerRPC_SetIsUniqueAttack(false);
        Owner->ServerRPC_SetIsTacle(false);
        Owner->ServerRPC_SetAllowRoll(true);
        Owner->ServerRPC_SetIsAttacking(false);
    }

}

void UHunterAnim::OnMontageNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload){
    if (!Owner->IsLocallyControlled())return;
    Owner->ServerRPC_SetAllowRoll(true);
    Owner->ServerRPC_SetIsAttacking(false);
    Owner->ServerRPC_SetIsJumpDelay(false);

}

void UHunterAnim::SetBluePrintValues()
{
    if (!Owner)return;
    Speed = Owner->GetVelocity().Size2D();
    WalkAngle = CalculateDirection(Owner->GetVelocity(), Owner->GetControlRotation());
    MoveState = Owner->MoveComp->MoveState;
    WeaponType = Owner->WeaponComp->GetWeaponType();
    isWeaponEquipped = Owner->WeaponComp->GetisWeaponEquipped();
    isRun = Owner->isRun;
}

float UHunterAnim::CalculateDirection(const FVector& Velocity) const
{
    if (!Velocity.IsNearlyZero())
    {
        AActor* OwnerActor = GetOwningActor();
        if (!OwnerActor)
        {
            return 0.0f;
        }

        FRotator CharacterRotation = FRotator(0.0f, OwnerActor->GetActorRotation().Yaw, 0.0f);
        FMatrix RotMatrix = FRotationMatrix(CharacterRotation);

        FVector ForwardVector = RotMatrix.GetScaledAxis(EAxis::X);
        ForwardVector.Z = 0.0f;
        ForwardVector.Normalize();

        FVector RightVector = RotMatrix.GetScaledAxis(EAxis::Y);
        RightVector.Z = 0.0f;
        RightVector.Normalize();

        FVector NormalizedVel = Velocity.GetSafeNormal2D();

        float ForwardCosAngle = FVector::DotProduct(ForwardVector, NormalizedVel);
        float ForwardDeltaDegree = FMath::RadiansToDegrees(FMath::Acos(ForwardCosAngle));

        float RightCosAngle = FVector::DotProduct(RightVector, NormalizedVel);
        if (RightCosAngle < 0)
        {
            ForwardDeltaDegree *= -1; // 오른쪽이면 음수
        }

        // 음수를 유지하며 스냅
        float SnappedDegree;
        float AbsDegree = FMath::Abs(ForwardDeltaDegree); // 스냅용 절댓값
        if (AbsDegree <= 22.5f)
            SnappedDegree = 0.0f;
        else if (AbsDegree <= 67.5f)
            SnappedDegree = 45.0f;
        else if (AbsDegree <= 112.5f)
            SnappedDegree = 90.0f;
        else if (AbsDegree <= 157.5f)
            SnappedDegree = 135.0f;
        else
            SnappedDegree = 180.0f;

        // 원래 부호 복원
        if (ForwardDeltaDegree < 0)
        {
            SnappedDegree *= -1;
        }

        return SnappedDegree; // -180 ~ 180도 반환 가능
    }

    return 0.0f;
}

float UHunterAnim::CalculateDirection(const FVector& Velocity, const FRotator& BaseRotation) const
{
    if (!Velocity.IsNearlyZero())
    {
        // 캐릭터의 Yaw만 사용 (Pitch와 Roll 제거)
        FRotator CharacterRotation = FRotator(0.0f, BaseRotation.Yaw, 0.0f);
        FMatrix RotMatrix = FRotationMatrix(CharacterRotation);

        // 2D 평면에서 Forward와 Right 벡터 계산
        FVector ForwardVector = RotMatrix.GetScaledAxis(EAxis::X);
        ForwardVector.Z = 0.0f; // Z축 제거
        ForwardVector.Normalize();

        FVector RightVector = RotMatrix.GetScaledAxis(EAxis::Y);
        RightVector.Z = 0.0f; // Z축 제거
        RightVector.Normalize();

        // 속도를 2D로 정규화
        FVector NormalizedVel = Velocity.GetSafeNormal2D();

        // Forward 벡터와 속도의 내적
        float ForwardCosAngle = FVector::DotProduct(ForwardVector, NormalizedVel);
        float ForwardDeltaDegree = FMath::RadiansToDegrees(FMath::Acos(ForwardCosAngle));

        // Right 벡터로 방향 결정 (좌/우 반전)
        float RightCosAngle = FVector::DotProduct(RightVector, NormalizedVel);
        if (RightCosAngle < 0)
        {
            ForwardDeltaDegree *= -1; // 오른쪽이면 음수
        }

        return ForwardDeltaDegree; // -180 ~ 180도
    }

    return 0.0f;
}
UAnimMontage* UHunterAnim::GetCurrentMontage(AHunter* Character)
{
    UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance();
    if (AnimInstance)
    {
        // 현재 재생 중인 몽타주 가져오기
        for (FAnimMontageInstance* MontageInstance : AnimInstance->MontageInstances)
        {
            if (MontageInstance && MontageInstance->IsPlaying())
            {
                return MontageInstance->Montage; // 현재 재생 중인 몽타주 반환
            }
        }
    }
    return nullptr; // 재생 중인 몽타주가 없으면 nullptr 반환
}