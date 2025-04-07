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
		case ESafiState::Idle		: { IdleState(); }	break;
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
	
	// 일정 시간이 지나면 MoveState로
	if (currentTime > me->idleTime)
	{
		mState = ESafiState::Move;
		Anim->aState = mState;

		currentTime = 0.f;
	}


	// 방향은 랜덤

	// Safi의 Tick에서 or 한 패턴이 끝나면 사거리 내 player의 존재 여부 확인
	// 플레이어가 없을 경우 다시 Idle
	// 플레이어가 존재할 경우 다음 공격 패턴으로


	if (dir.Size() < me->AttackRange)
	{
		OnAttackProcess();
		//	currentTime = 0.f;
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
	currentTime += GetWorld()->DeltaTimeSeconds;

	if (target != nullptr)
	{
		TargetRotation();
		FVector dir = SearchTarget();
		me->AddMovementInput(dir);
	}

	if (currentTime > me->idleTime)
	{	

		mState = ESafiState::Idle;
		Anim->aState = mState;

		currentTime = 0.f;
	}

	/*
	TargetRotation();
	FVector dir = SearchTarget();

	me->AddMovementInput(dir);
	*/

	//여기도 커런트타임 경과 후 사거리에 따라 다음 공격을 결정하기로 함.
}

void UCSafiFSM::OnAttackProcess()
{
	//공격 상태로의 전환
	mState = ESafiState::Attack;
	Anim->aState = mState;

	//어떤 공격을 할 지 판별




	//mAttState = EAttackState::Breath;
	//Anim->aAttState = mAttState;

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

