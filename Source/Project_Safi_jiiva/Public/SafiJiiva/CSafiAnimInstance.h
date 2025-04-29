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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
	ETurnState aTurnState = ETurnState::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = FSM)
	EDisturbState aDisturbState = EDisturbState::None;
	
public:	// 수치값
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoveValue")
	float speed = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoveValue")
	float direction = 0.f;

public:	// 전투 관련 상태
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoveValue")
	bool isInBattleA = false;		// 전투 상태인지 확인.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MoveValue")
	bool isDisturbedA = false;		// 상태이상 걸렸을 때 튕겨줄거임.

public:	// AnimNotify 관련 -------- UFUNCTION() 잊지말것!!!
	UFUNCTION() 
	void AnimNotify_Roar_END();


	// 물기 노티파이
	UFUNCTION()
	void AnimNotify_AttBite_Start();
	UFUNCTION()
	void AnimNotify_AttBite_End();

	// 물기 노티파이
	UFUNCTION()
	void AnimNotify_AttHandAttack_Start();
	UFUNCTION()
	void AnimNotify_AttHandAttack_End();


	// 바디프레스 노티파이
	UFUNCTION()
	void AnimNotify_AttBodyPress_Start();
	UFUNCTION()
	void AnimNotify_AttBodyPress_End();


	// 브레스 노티파이
	UFUNCTION()
	void AnimNotify_AttBreath_Start();
	UFUNCTION()
	void AnimNotify_AttBreath_End();


	UFUNCTION()
	void AnimNotify_Attack_End();	// 공격 종료시 사용하는 범용함수. EndAttackProcess 호출

	UFUNCTION()
	void AnimNotify_Search_End();

	UFUNCTION()
	void AnimNotify_Att_Special_Spawn();

};
