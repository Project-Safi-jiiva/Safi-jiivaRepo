// Fill out your copyright notice in the Description page of Project Settings.


#include "SafiJiiva/CSafiAnimInstance.h"
#include "SafiJiiva/CSafiJiiva.h"
#include "SafiJiiva/CSafiFSM.h"

void UCSafiAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	// FString logMsgState = UEnum::GetValueAsString(aState);
	// GEngine->AddOnScreenDebugMessage(0, 1, FColor::Yellow, logMsgState);
	// 
	// FString logMsgAtt = UEnum::GetValueAsString(aAttState);
	// GEngine->AddOnScreenDebugMessage(1, 1, FColor::Green, logMsgAtt);
	// 
	// FString logMsgTurn = UEnum::GetValueAsString(aTurnState);
	// GEngine->AddOnScreenDebugMessage(2, 1, FColor::Yellow, logMsgTurn);


	me = Cast<ACSafiJiiva>(TryGetPawnOwner());
	if (!me) { return; }

	FSM = me->FindComponentByClass<UCSafiFSM>();


	FVector velocity = me->GetVelocity();
	FVector forwardVector = me->GetActorForwardVector();


	speed = FVector::DotProduct(velocity, forwardVector);

	FVector rightVector = me->GetActorRightVector();
	direction = FVector::DotProduct(velocity, rightVector);
}


// ===================== 노티파이 추가하는 파트 =====================

void UCSafiAnimInstance::AnimNotify_Roar_END()
{
	if( !me ){ return; }

	// 노티파이 종료시 EndAttackProcess 호출
	FSM->EndAttackProcess();

	// 노티파이 종료시 이뮨 해제
	me->isImmune = false;

}

void UCSafiAnimInstance::AnimNotify_AttBite_Start()
{
	if (!me) { return; }
	me->isOnAttBite = true;

}
void UCSafiAnimInstance::AnimNotify_AttBite_End()
{
	if (!me) { return; }
	me->isOnAttBite = false;
}

void UCSafiAnimInstance::AnimNotify_Attack_End()
{
	FSM->EndAttackProcess();
}


void UCSafiAnimInstance::AnimNotify_Search_End()
{
	me->isOnSearch = false;
	FSM->EndAttackProcess();
	FSM->OnAttackProcess();
}