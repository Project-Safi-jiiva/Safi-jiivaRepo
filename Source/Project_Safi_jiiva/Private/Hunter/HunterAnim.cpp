// Fill out your copyright notice in the Description page of Project Settings.


#include "Hunter/HunterAnim.h"
#include "Hunter/Hunter.h"
#include "Project_Safi_jiiva.h"
#include "Hunter/MoveComponent.h"

void UHunterAnim::NativeBeginPlay()
{
	Owner = Cast<AHunter>(TryGetPawnOwner());

}
void UHunterAnim::NativeUpdateAnimation(float DeltaTime)
{
	if (!Owner)return;
	Speed=Owner->GetVelocity().Size2D();
	WalkAngle = CalculateDirection(Owner->GetVelocity(), Owner->GetControlRotation());

	PRINT_LOG(TEXT("Speed : %f"), WalkAngle);
    MoveState = Owner->MoveComp->MoveState;
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
