// Fill out your copyright notice in the Description page of Project Settings.


#include "SafiJiiva/CSafiFSM.h"
#include "Kismet/GameplayStatics.h"
#include "Hunter/Hunter.h"

#include "SafiJiiva/CSafiAnimInstance.h"
#include "SafiJiiva/CSafiJiiva.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UCSafiFSM::UCSafiFSM()
{
	PrimaryComponentTick.bCanEverTick = true;

		
}


// Called when the game starts
void UCSafiFSM::BeginPlay()
{
	Super::BeginPlay();

	AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AHunter::StaticClass());
	if(!actor) { return; }
	target = Cast<AHunter>(actor);

	me = Cast<ACSafiJiiva>(GetOwner());
	if(!me) { return; }

	Anim = Cast<UCSafiAnimInstance>(me->GetMesh()->GetAnimInstance());


	me->SetSpeed(me->WalkSpeed);
}


// Called every frame
void UCSafiFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#pragma region LogMessageState
	FString logMsgState = UEnum::GetValueAsString(mState);
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Red, logMsgState);

	FString logMsgAtt = UEnum::GetValueAsString(mAttState);
	GEngine->AddOnScreenDebugMessage(1, 1, FColor::Green, logMsgAtt);
#pragma endregion

	switch(mState)
	{
		case ESafiState::Idle		: { /*IdleState();*/ }	break;
		case ESafiState::Move		: {  }	break;
		case ESafiState::Attack		: {  }	break;
		case ESafiState::Dead		: {  }	break;
	}
	
	switch(mAttState)
	{
		case EAttackState::None			: {  }	break;
		case EAttackState::Breath		: {  }	break;
		case EAttackState::AimedBreath	: {  }	break;
		case EAttackState::CloseAttack	: {  }	break;
	
	}
}

void UCSafiFSM::IdleState()
{
	FVector dir = SearchTarget();

	currentTime += GetWorld()->DeltaTimeSeconds;

	if (dir.Size() < me->AttackRange)
	{
		currentTime = 0.f;
	}
	
	// 공격 프로세스 고민해보기.
	// 탐지 -> 포효해야함
	if( me->isInBattle == true )
	{
		//여기는 커런트타임 경과 후 사거리에 따라 다음 공격을 결정하기로 함.
		me->SetSpeed(me->RunSpeed);
		currentTime = 0.f;
	}

}

void UCSafiFSM::MoveState()
{
	TargetRotation();
	FVector dir = SearchTarget();

	me->AddMovementInput(dir);

	//여기도 커런트타임 경과 후 사거리에 따라 다음 공격을 결정하기로 함.
}

void UCSafiFSM::OnAttackProcess()
{x	

}

void UCSafiFSM::TargetRotation()
{
	float DeltaTime = GetWorld()->DeltaTimeSeconds;
	FVector dir = SearchTarget();

	FRotator TargetRotation = dir.Rotation();
	FRotator CurrentRotation = me->GetActorRotation();

	FRotator NewRotation = FMath::RInterpTo(CurrentRotation , TargetRotation, DeltaTime, 1.0f);

	NewRotation.Yaw = TargetRotation.Yaw;
	me->SetActorRotation(NewRotation);
}

FVector UCSafiFSM::SearchTarget()
{
	if (target == nullptr || me == nullptr){ return FVector::ZeroVector; }

	FVector destination		= target->GetActorLocation();
	FVector dir				= destination - me->GetActorLocation();

	if (dir.Size() < me->SearchRange)
	{
		me->isInBattle = true;
	}

	return dir;
}

