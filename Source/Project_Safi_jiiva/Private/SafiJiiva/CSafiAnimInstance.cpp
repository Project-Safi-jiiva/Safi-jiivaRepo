// Fill out your copyright notice in the Description page of Project Settings.


#include "SafiJiiva/CSafiAnimInstance.h"
#include "SafiJiiva/CSafiJiiva.h"

void UCSafiAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
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

	// 노티파이 종료시 OnAttackProcess 호출
	FSM->OnAttackProcess();
	// 노티파이 종료시 이뮨 해제
	me->isImmune = false;
}

void UCSafiAnimInstance::AnimNotify_AttBiteSwitch()
{
	if (!me) { return; }
	me->isOnAttBite = !me->isOnAttBite;

	UE_LOG(LogTemp, Warning, TEXT("Bite_Test_Anim"));
}

