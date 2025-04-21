// Fill out your copyright notice in the Description page of Project Settings.


#include "SafiJiiva/CSafiAnimInstance.h"
#include "SafiJiiva/CSafiJiiva.h"
#include "SafiJiiva/CSafiFSM.h"

void UCSafiAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{

// 고장날 때 쓰는 용도
#pragma region LogMSG
	// FString logMsgState = UEnum::GetValueAsString(aState);
	// GEngine->AddOnScreenDebugMessage(0, 1, FColor::Yellow, logMsgState);
	// 
	// FString logMsgAtt = UEnum::GetValueAsString(aAttState);
	// GEngine->AddOnScreenDebugMessage(1, 1, FColor::Green, logMsgAtt);
	// 
	// FString logMsgTurn = UEnum::GetValueAsString(aTurnState);
	// GEngine->AddOnScreenDebugMessage(2, 1, FColor::Yellow, logMsgTurn);
#pragma endregion LogMSG

	me = Cast<ACSafiJiiva>(TryGetPawnOwner());
	if (!me) { return; }

	FSM = me->FindComponentByClass<UCSafiFSM>();


	FVector velocity = me->GetVelocity();
	FVector forwardVector = me->GetActorForwardVector();


	speed = FVector::DotProduct(velocity, forwardVector);

	FVector rightVector = me->GetActorRightVector();
	direction = FVector::DotProduct(velocity, rightVector);


	isInBattleA = me->isInBattle;
	isDisturbedA = me->isDisturbed;
}

// ===================== 노티파이 추가하는 파트 =====================

void UCSafiAnimInstance::AnimNotify_Roar_END()
{

	if( !me ){ return; }

	FSM->mState = ESafiState::Idle;
	//aState = FSM->mState;
	FSM->OnRep_SafiState();

	me->isDisturbed = false;

	// 노티파이 종료시 EndAttackProcess 호출
	FSM->EndAttackProcess();

}

void UCSafiAnimInstance::AnimNotify_AttBite_Start()
{
	if (!me || !FSM) { return; }
	me->isOnAttBite = true;
}
void UCSafiAnimInstance::AnimNotify_AttBite_End()
{
	if (!me || !FSM) { return; }
	me->isOnAttBite = false;
}

void UCSafiAnimInstance::AnimNotify_AttHandAttack_Start()
{
	if (!me || !FSM) { return; }
	me->isFootAttack = true;
}

void UCSafiAnimInstance::AnimNotify_AttHandAttack_End()
{
	if (!me || !FSM) { return; }
	me->isFootAttack = false;
}

void UCSafiAnimInstance::AnimNotify_AttBodyPress_Start()
{
	if (!me || !FSM) { return; }
	me->isOnBodyPress = true;
}

void UCSafiAnimInstance::AnimNotify_AttBodyPress_End()
{
	if (!me || !FSM) { return; }
	me->isOnBodyPress = false;
}

void UCSafiAnimInstance::AnimNotify_AttBreath_Start()
{
	if (!me || !FSM) { return; }
	me->isOnBreath = true;
}

void UCSafiAnimInstance::AnimNotify_AttBreath_End()
{
	if (!me || !FSM) { return; }
	me->isOnBreath = false;
}

void UCSafiAnimInstance::AnimNotify_Attack_End()
{
	if (!me || !FSM) { return; }
	FSM->EndAttackProcess();
	me->HitPawn.Empty();
	me->attackPos = 0;
}


void UCSafiAnimInstance::AnimNotify_Search_End()
{
	if (!me || !FSM) { return; }
	me->isOnSearch = false;

	FSM->mTurnState = ETurnState::None;
	//aTurnState = FSM->mTurnState;
	FSM->OnRep_TurnState();

	// Idle 복귀
	FSM->mState = ESafiState::Idle;
	//aState = FSM->mState;
	FSM->OnRep_SafiState();

	me->HitPawn.Empty();	// 혹시 몰라서...?
}