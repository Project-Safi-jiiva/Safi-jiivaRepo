// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SafiJiiva/CSafiFSM.h"
#include "CSafiAnimInstance.generated.h"


UCLASS()
class PROJECT_SAFI_JIIVA_API UCSafiAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	class ACSafiJiiva*	me;
	class UCSafiFSM*	FSM;

public:	//FSM State 관련
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FSM")
	ESafiState aState = ESafiState::Idle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "FSM")
	EAttackState aAttState = EAttackState::None;
	
public:	// 수치값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoveValue")
	float speed = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoveValue")
	float direction = 0.f;

public:	// AnimNotify 관련 -------- UFUNCTION() 잊지말것!!!

	UFUNCTION() 
	void AnimNotify_Roar_END();
};
