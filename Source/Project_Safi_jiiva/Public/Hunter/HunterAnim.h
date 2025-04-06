// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EMoveState.h"
#include "HunterAnim.generated.h"

/**
 *
 */
UCLASS()
class PROJECT_SAFI_JIIVA_API UHunterAnim : public UAnimInstance
{
	GENERATED_BODY()

private:

	virtual void NativeUpdateAnimation(float DeltaTime) override;

	virtual void NativeBeginPlay();

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Movement")
	float Speed;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Movement")
	float WalkAngle;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Movement")
	EMoveState MoveState;
	class AHunter* Owner;
private:
	float CalculateDirection(const FVector& Velocity, const FRotator& BaseRotation) const;


};
