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
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Yellow, logMsgState);

	FString logMsgAtt = UEnum::GetValueAsString(mAttState);
	GEngine->AddOnScreenDebugMessage(1, 1, FColor::Green, logMsgAtt);

	FString logMsgTurn = UEnum::GetValueAsString(mTurnState);
	GEngine->AddOnScreenDebugMessage(2, 1, FColor::Yellow, logMsgTurn);

	// ==========================================================================

	// bool형 변수 상태 출력

// isInBattle 상태 출력 (True일 때 빨간색)
	FColor inBattleColor = me->isInBattle ? FColor::Red : FColor::White;
	FString logMsgInBattle = FString::Printf(TEXT("isInBattle: %s"), me->isInBattle ? TEXT("True") : TEXT("False"));
	GEngine->AddOnScreenDebugMessage(5, 1, inBattleColor, logMsgInBattle);

	// isFly 상태 출력 (True일 때 빨간색)
	FColor flyColor = me->isFly ? FColor::Red : FColor::White;
	FString logMsgFly = FString::Printf(TEXT("isFly: %s"), me->isFly ? TEXT("True") : TEXT("False"));
	GEngine->AddOnScreenDebugMessage(6, 1, flyColor, logMsgFly);

	// isImmune 상태 출력 (True일 때 빨간색)
	FColor immuneColor = me->isImmune ? FColor::Red : FColor::White;
	FString logMsgImmune = FString::Printf(TEXT("isImmune: %s"), me->isImmune ? TEXT("True") : TEXT("False"));
	GEngine->AddOnScreenDebugMessage(7, 1, immuneColor, logMsgImmune);

	// isDisturbed 상태 출력 (True일 때 빨간색)
	FColor disturbedColor = me->isDisturbed ? FColor::Red : FColor::White;
	FString logMsgDisturbed = FString::Printf(TEXT("isDisturbed: %s"), me->isDisturbed ? TEXT("True") : TEXT("False"));
	GEngine->AddOnScreenDebugMessage(8, 1, disturbedColor, logMsgDisturbed);

	// isBreath 상태 출력 (True일 때 빨간색)
	FColor breathColor = me->isBreath ? FColor::Red : FColor::White;
	FString logMsgBreath = FString::Printf(TEXT("isBreath: %s"), me->isBreath ? TEXT("True") : TEXT("False"));
	GEngine->AddOnScreenDebugMessage(9, 1, breathColor, logMsgBreath);

	// isRepelled 상태 출력 (True일 때 빨간색)
	FColor repelledColor = me->isRepelled ? FColor::Red : FColor::White;
	FString logMsgRepelled = FString::Printf(TEXT("isRepelled: %s"), me->isRepelled ? TEXT("True") : TEXT("False"));
	GEngine->AddOnScreenDebugMessage(10, 1, repelledColor, logMsgRepelled);

	// Collision_1 활성화 상태 출력 (True일 때 빨간색)
	FColor collisionColor = me->Collision_1->IsCollisionEnabled() ? FColor::Red : FColor::White;
	FString logMsgCollision = FString::Printf(TEXT("Collision_1 is enabled: %s"), me->Collision_1->IsCollisionEnabled() ? TEXT("True") : TEXT("False"));
	GEngine->AddOnScreenDebugMessage(11, 1, collisionColor, logMsgCollision);

#pragma endregion

	switch(mState)
	{
		case ESafiState::Idle		: { IdleState(); }	break;
		//case ESafiState::Move		: {  }	break;
		case ESafiState::Turn		: { CanMeleeAttack(); }	break;
		case ESafiState::Attack		: {  }	break;
		case ESafiState::Dead		: {  }	break;
	}
	
	switch(mAttState)
	{
		case EAttackState::None			: { }	break;
		case EAttackState::Roar			: { }	break;

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

void UCSafiFSM::CanMeleeAttack()
{

	// 해당 위치에 있다면 AttType 근접공격으로 return;
	if (me->attackPos == 2)
	{
		attType = AttMELEE_LF;
		OnAttackProcess();
		return;
	}

	if (me->attackPos == 3)
	{
		attType = AttMELEE_RF;
		OnAttackProcess();
		return;
	}

	if (me->attackPos == 4)
	{
		attType = AttMELEE_RB;
		OnAttackProcess();
		return;
	}

	// 해당 위치에 적이 없는 경우는 노티파이로 EndAttackProcess	- 수행 완료
	// ㄴ 여기에서 브레스 종류 결정하기
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

	// 노티파이 종료시 EndAttackProcess 호출	- 수행완료
	// 노티파이 종료시 이뮨 해제				- 수행완료
}

void UCSafiFSM::AttMelee()
{
	// 노티파이로 isOnAttBite = true 활성화  - 수행완료

	// isOnAttBite 상태라면 Collision_1 활성화, 비활성화 - 캐릭터 자체 틱으로 옮김  - 수행완료


	// 노티파이로 isOnAttBite = false	- 수행완료

	// 노티파이로 공격이 끝날땐 정리 프로세스 - 수행완료
}

void UCSafiFSM::AttBreath()
{

}

// mState를 Attack으로 변경 / 공격 스위치
void UCSafiFSM::OnAttackProcess()
{
	me->isOnSearch = false;
	FVector dir = SearchTarget();

	//공격 상태로의 전환
	if (mState != ESafiState::Attack)
	{
		mState = ESafiState::Attack;
		Anim->aState = mState;
	}

	switch (attType)
	{
	case AttNONE:
		mAttState = EAttackState::None;
		break;

	case AttROAR:
		mAttState = EAttackState::Roar;
		break;

	case AttMELEE_LF:
		mAttState = EAttackState::MeleeAttLF;
		break;

	case AttMELEE_RF:
		mAttState = EAttackState::MeleeAttRF;
		break;

	case AttMELEE_RB:
		mAttState = EAttackState::MeleeAttRB;
		break;

	case AttMELEE_LB:
		mAttState = EAttackState::MeleeAttLB;
		break;

	case AttNMBREATH:
		mAttState = EAttackState::NormalBreath;
		break;

	case AttAIMBREATH:
		mAttState = EAttackState::AimedBreath;
		break;
	}
	Anim->aAttState = mAttState;

}


void UCSafiFSM::EndAttackProcess()
{
	FVector dir = SearchTarget();

	mAttState = EAttackState::None;
	Anim->aAttState = mAttState;
	

	//어떤 공격을 할 지 판별 	// 공격 타입 번호만 정해주기

	// 근접 공격 사거리 안쪽에 있다면 위치에 따라 공격
	// 범위 내에 없다면 브레스 패턴으로	*우선은 정면 브레스만

	me->isOnSearch = true;	// 공격할 때 꺼주기	- OnAttackProcess에 false 해줌


	//if 돌아야 한다면 플레이어 방향으로 회전, 아닐시 return; - TargetRotationByAnim 에서 return
	TargetRotationByAnim();

	if (dir.Size() > me->MeleeAttRange)
	{
		mAttState = EAttackState::AimedBreath;
		Anim->aAttState = mAttState;
		return;
	}

	
	/*
	if (정면일 경우)
	{
		둘 중 하나
		물기 / 정면브레스 / 바디프레스
	}
	*/

	//OnAttackProcess();		// 회전 테스트 중이라 잠시 막음.
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

void UCSafiFSM::TargetRotationByAnim()
{
	FVector dir = SearchTarget();
	FVector dirLocal = me->GetActorTransform().InverseTransformVectorNoScale(dir);	// 로컬 방향 계산


	// 캐릭터가 앞에 있음. 회전 불필요. return.
	// 나중에 idle이나 백스텝 밟으면서 rotation만 조금 돌리게 수정할 것.

	//if (dirLocal.X > 0)	{ return; }		// 앞에 있을 경우

	mState = ESafiState::Turn;
	Anim->aState = mState;

	//=============================
	if (dirLocal.X < 0)	// 캐릭터가 뒤에 있음
	{
		// 회전 후방으로.
		mTurnState = ETurnState::TrunBack;
		Anim->aTurnState = mTurnState;

	}

	else if (dirLocal.Y < 0 && dirLocal.X > 0)	// 캐릭터가 좌측이지만 뒤는 아님
	{
		// 회전 좌측으로
		mTurnState = ETurnState::TurnLeft;
		Anim->aTurnState = mTurnState;
	}


	else if (dirLocal.Y > 0 && dirLocal.X > 0)	// 캐릭터가 우측이지만 뒤는 아님
	{
		// 회전 우측으로
		mTurnState = ETurnState::TurnRight;
		Anim->aTurnState = mTurnState;
	}

	// 적이 해당 위치에 있다면 공격으로 전환 -> TrunState( Turn Tick에서 )


	// 노티파이로 isOnSearch 꺼주기			- 수행완료
	//	ㄴ OnAttackProcess도 같이			- 수행완료
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

