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
