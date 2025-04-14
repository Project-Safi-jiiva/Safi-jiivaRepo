// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "EMoveState.h"
#include "Weapon/EWeaponType.h"
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
	UFUNCTION()
	void OnMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void OnMontageNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointNotifyPayload);

	void SetBluePrintValues();

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Movement")
	float Speed;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Movement")
	float WalkAngleStart;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkAngle;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Movement")
	EMoveState MoveState = EMoveState::IDLE;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Movement")
	EWeaponType WeaponType=EWeaponType::NONE;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Movement")
	bool isRun;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Movement")
	bool isWeaponEquipped;

	class AHunter* Owner;
	UAnimMontage* GetCurrentMontage(AHunter* Character);
private:
	float CalculateDirection(const FVector& Velocity) const;
	float CalculateDirection(const FVector& Velocity, const FRotator& BaseRotation) const;
	bool bIsLerpingSpeed;
	float SpeedLerpTime;
	float SpeedLerpDuration=0;
	bool isStart = false;



};
