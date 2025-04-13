// Fill out your copyright notice in the Description page of Project Settings.


#include "Hunter/HunterAnim.h"
#include "Hunter/Hunter.h"
#include "Project_Safi_jiiva.h"
#include "Hunter/MoveComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Weapon/WeaponComponent.h"

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
    Owner->WeaponComp->isJumpDelay = true;
    if (NotifyName == FName(TEXT("QuickStrikeStart"))) { Owner->WeaponComp->IsAttacking = true; }
    if (NotifyName == FName(TEXT("IsAttackingOff"))) { Owner->WeaponComp->IsAttacking = false; Owner->WeaponComp->SetQuickStrikeComboIndex(1); }
    if (NotifyName == FName(TEXT("QuickStrikeEnd"))) {Owner->WeaponComp->QuickStrikeNext();}
    if (NotifyName == FName(TEXT("DelayStart"))) { Owner->WeaponComp->isJumpDelay = true; }
    if (NotifyName == FName(TEXT("DelayEnd"))) {
        Owner->WeaponComp->isJumpDelay = false;
		Owner->WeaponComp->JumpToNextCombo();
    }
    if (NotifyName == FName(TEXT("Next2"))) {Owner->WeaponComp->SetHeavyStrikeComboIndex(Owner->WeaponComp->GetHeavyStrikeComboIndex() + 1);}

    if (NotifyName == FName(TEXT("ComboEnd"))) { Owner->WeaponComp->ResetCombo(); PRINT_LOG(TEXT("TS")); }

    if (NotifyName == FName(TEXT("iscancelStart"))) { Owner->WeaponComp->iscancel = true; }
    if (NotifyName == FName(TEXT("iscancelEnd"))) { Owner->WeaponComp->iscancel = false; }

    if (NotifyName == FName(TEXT("Attach"))) {Owner->WeaponComp->AttachWeaponToHand();}
    if (NotifyName == FName(TEXT("Detach"))) {Owner->WeaponComp->AttachWeaponToOwner();}
}

void UHunterAnim::OnMontageNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload){}

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