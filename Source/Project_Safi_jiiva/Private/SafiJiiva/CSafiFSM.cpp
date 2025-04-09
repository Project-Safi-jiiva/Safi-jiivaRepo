// Fill out your copyright notice in the Description page of Project Settings.


#include "SafiJiiva/CSafiFSM.h"
#include "Kismet/GameplayStatics.h"
#include "Hunter/Hunter.h"

#include "SafiJiiva/CSafiAnimInstance.h"
#include "SafiJiiva/CSafiJiiva.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"

// Sets default values for this component's properties
UCSafiFSM::UCSafiFSM()
{
	PrimaryComponentTick.bCanEverTick = true;

		
}


// Called when the game starts
void UCSafiFSM::BeginPlay()
{
	Super::BeginPlay();

	// AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AHunter::StaticClass());
	// if(!actor) { return; }
	// target = Cast<AHunter>(actor);

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
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Blue, logMsgState);

	FString logMsgAtt = UEnum::GetValueAsString(mAttState);
	GEngine->AddOnScreenDebugMessage(1, 1, FColor::Green, logMsgAtt);

	// ==========================================================================

	// bool형 변수 상태 출력

	FColor inBattleColor = me->isInBattle ? FColor::Red : FColor::White;
	FString logMsgInBattle = FString::Printf(TEXT("isInBattle: %s"), me->isInBattle ? TEXT("True") : TEXT("False"));
	GEngine->AddOnScreenDebugMessage(3, 1, inBattleColor, logMsgInBattle);

	// isFly 상태 출력 (True일 때 빨간색)
	FColor flyColor = me->isFly ? FColor::Red : FColor::White;
	FString logMsgFly = FString::Printf(TEXT("isFly: %s"), me->isFly ? TEXT("True") : TEXT("False"));
	GEngine->AddOnScreenDebugMessage(4, 1, flyColor, logMsgFly);

	// isImmune 상태 출력 (True일 때 빨간색)
	FColor immuneColor = me->isImmune ? FColor::Red : FColor::White;
	FString logMsgImmune = FString::Printf(TEXT("isImmune: %s"), me->isImmune ? TEXT("True") : TEXT("False"));
	GEngine->AddOnScreenDebugMessage(5, 1, immuneColor, logMsgImmune);

	// isDisturbed 상태 출력 (True일 때 빨간색)
	FColor disturbedColor = me->isDisturbed ? FColor::Red : FColor::White;
	FString logMsgDisturbed = FString::Printf(TEXT("isDisturbed: %s"), me->isDisturbed ? TEXT("True") : TEXT("False"));
	GEngine->AddOnScreenDebugMessage(6, 1, disturbedColor, logMsgDisturbed);

	// isBreath 상태 출력 (True일 때 빨간색)
	FColor breathColor = me->isBreath ? FColor::Red : FColor::White;
	FString logMsgBreath = FString::Printf(TEXT("isBreath: %s"), me->isBreath ? TEXT("True") : TEXT("False"));
	GEngine->AddOnScreenDebugMessage(7, 1, breathColor, logMsgBreath);

	// isRepelled 상태 출력 (True일 때 빨간색)
	FColor repelledColor = me->isRepelled ? FColor::Red : FColor::White;
	FString logMsgRepelled = FString::Printf(TEXT("isRepelled: %s"), me->isRepelled ? TEXT("True") : TEXT("False"));
	GEngine->AddOnScreenDebugMessage(8, 1, repelledColor, logMsgRepelled);

#pragma endregion

	switch(mState)
	{
		case ESafiState::Idle		: { IdleState(); }	break;
		//case ESafiState::Move		: {  }	break;
		case ESafiState::Attack		: {  }	break;
		case ESafiState::Dead		: {  }	break;
	}
	
	switch(mAttState)
	{
		case EAttackState::None			: {  }	break;
		case EAttackState::Roar			: {  }	break;

		case EAttackState::MeleeBite	: { AttMelee(); }	break;
		case EAttackState::MeleeBPress	: {  }	break;

		case EAttackState::NormalBreath	: {  }	break;
		case EAttackState::AimedBreath	: {  }	break;
	
	}
}

// 타겟 사망시에 IdleState로 돌아옴
void UCSafiFSM::IdleState()
{
	// ======== 탐지 -> 포효 -> 개전 ========

//탐지파트
	AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AHunter::StaticClass());
	if (!actor) { return; }
	target = Cast<AHunter>(actor);

	FVector dir = SearchTarget();

	//타겟과의 사거리가 색적범위보다 멀다면 타겟을 삭제
	if ( dir.Size() > me->SearchRange)
	{
		target = nullptr;
		currentTime += GetWorld()->DeltaTimeSeconds;
		if (currentTime >me->idleTime)
		{
			return;

		}
			
		return;
	}

	// 위 조건을 지나옴 = 색적범위 안쪽
	me->isInBattle = true;


// 포효
	// 포효 처리 및 공격 패턴으로 전환
	mState = ESafiState::Attack;
	Anim->aState = mState;

	AttRoar();
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

}

void UCSafiFSM::BreathState()
{

}

void UCSafiFSM::AttRoar()
{
	me->isImmune = true;
	mAttState = EAttackState::Roar;
	Anim->aAttState = mAttState;
	// 포효 공격판정 실행

	// 노티파이 종료시 OnAttackProcess 호출	- 수행완료
	// 노티파이 종료시 이뮨 해제 - 수행완료
}

void UCSafiFSM::AttMelee()
{
	// 해놓고 분리하던가 하자

	// 노티파이로 isOnAttBite = true 활성화

	// isOnAttBite 상태라면 Collision_1 활성화
	if (me->isOnAttBite)
	{
		me->Collision_1->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	// 노티파이로 isOnAttBite = false

	// 공격이 끝날땐 정리 프로세스
}

void UCSafiFSM::AttBreath()
{

}

// mState를 Attack으로 변경 / 다음 공격에 대한 판단 
void UCSafiFSM::OnAttackProcess()
{
	FVector dir = SearchTarget();

	//공격 상태로의 전환
	mState = ESafiState::Attack;
	Anim->aState = mState;

	//어떤 공격을 할 지 판별

	//근접 공격 사거리 안쪽에 있다면 팔, 다리중 가까운 쪽으로 공격
	if (dir.Size() < me->MeleeAttRange)
	{
		mAttState = EAttackState::MeleeBite;
		Anim->aAttState = mAttState;
	}

	//물기 구현중이라 브레스 잠시 막음
	/*
	// 근접공격 사거리 바깥쪽이라면 브레스 패턴으로
	else 
	{ 
		mAttState = EAttackState::AimedBreath;
		Anim->aAttState = mAttState;
	}
	*/

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

