// Fill out your copyright notice in the Description page of Project Settings.

// ** 주의! **
// 무페토지바 각도가 빠르게 회전한다면 Idle 회전값 조건이 잘못 걸려있는 것임. 조정하면 돌아옴.
// Idle에 있는 각도별 모션판정은 Server 기준으로 판정하게 하기.	- 잘못하면 유저별로 몹 모션 다르게 나옴


#include "SafiJiiva/CSafiFSM.h"
#include "Kismet/GameplayStatics.h"
#include "Hunter/Hunter.h"

#include "SafiJiiva/CSafiAnimInstance.h"
#include "SafiJiiva/CSafiJiiva.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UCSafiFSM::UCSafiFSM()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true); // 또는 SetIsReplicated(true);
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


	// LogMessageState : Only 로그 출력용 (통째로 지워도 됨)
#pragma region LogMessageState

	/// ==================================== 에러 테스트용 계산 ====================================
	FVector dir = SearchTarget();
	FRotator targetRot = dir.Rotation();
	FRotator currentRot = me->GetActorRotation();
	float length = dir.Size();
	float targetYaw = FMath::Abs(FMath::FindDeltaAngleDegrees(currentRot.Yaw, targetRot.Yaw));


	/// ============================================================================================


	FString logMsgState = UEnum::GetValueAsString(mState);
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Yellow, logMsgState);

	FString logMsgAtt = UEnum::GetValueAsString(mAttState);
	GEngine->AddOnScreenDebugMessage(1, 1, FColor::Green, logMsgAtt);

	FString logMsgTurn = UEnum::GetValueAsString(mTurnState);
	GEngine->AddOnScreenDebugMessage(2, 1, FColor::Yellow, logMsgTurn);

	FString logBFattType = FString::Printf(TEXT("BFattType: %d"), BFattType);
	GEngine->AddOnScreenDebugMessage(3, 1, FColor::Red, logBFattType);

	FString logattType = FString::Printf(TEXT("attType: %d"), attType);
	GEngine->AddOnScreenDebugMessage(4, 1, FColor::Yellow, logattType);


	// GEngine->AddOnScreenDebugMessage(3, 1, FColor::Emerald, FString::Printf(TEXT("distance: %f"),length));
	// GEngine->AddOnScreenDebugMessage(4, 1, FColor::Emerald, FString::Printf(TEXT("targetYaw: %f"), targetYaw));
	// ==========================================================================

	// bool형 변수 상태 출력

// isInBattle 상태 출력 (True일 때 빨간색)
	FColor inBattleColor = me->isInBattle ? FColor::Red : FColor::White;
	FString logMsgInBattle = FString::Printf(TEXT("isInBattle: %s"), me->isInBattle ? TEXT("True") : TEXT("False"));
	GEngine->AddOnScreenDebugMessage(5, 1, inBattleColor, logMsgInBattle);

	// isDisturbed 상태 출력 (True일 때 빨간색)
	FColor disturbedColor = me->isDisturbed ? FColor::Red : FColor::White;
	FString logMsgDisturbed = FString::Printf(TEXT("isDisturbed: %s"), me->isDisturbed ? TEXT("True") : TEXT("False"));
	GEngine->AddOnScreenDebugMessage(8, 1, disturbedColor, logMsgDisturbed);

	// isBreath 상태 출력 (True일 때 빨간색)
	FColor breathColor = me->isOnBreath ? FColor::Red : FColor::White;
	FString logMsgBreath = FString::Printf(TEXT("isBreath: %s"), me->isOnBreath ? TEXT("True") : TEXT("False"));
	GEngine->AddOnScreenDebugMessage(9, 1, breathColor, logMsgBreath);

	// isKnockBack 상태 출력 (True일 때 빨간색)
	FColor repelledColor = me->isKnockBack ? FColor::Red : FColor::White;
	FString logMsgRepelled = FString::Printf(TEXT("isRepelled: %s"), me->isKnockBack ? TEXT("True") : TEXT("False"));
	GEngine->AddOnScreenDebugMessage(10, 1, repelledColor, logMsgRepelled);


	// Collision_1 활성화 상태 출력 (True일 때 빨간색)
	FColor collisionColor = me->AttCollisionBite->IsCollisionEnabled() ? FColor::Red : FColor::White;
	FString logMsgCollision = FString::Printf(TEXT("Collision_1 is enabled: %s"), me->AttCollisionBite->IsCollisionEnabled() ? TEXT("True") : TEXT("False"));
	GEngine->AddOnScreenDebugMessage(11, 1, collisionColor, logMsgCollision);

#pragma endregion

	//state 변경
	switch(mState)
	{
		case ESafiState::Start		: { StartState(); }	break;
		case ESafiState::Idle		: { IdleState(); }	break;
		//case ESafiState::Move		: {  }	break;
		case ESafiState::Turn		: { CanMeleeAttack(); }	break;
		case ESafiState::Attack		: {  }	break;
		case ESafiState::Disturbed	: {  }	break;
	}
	
	switch(mAttState)
	{
		case EAttackState::None			: { }	break;
		case EAttackState::Roar			: { }	break;

		case EAttackState::MeleeBite	: { }	break;
		case EAttackState::MeleeBPress	: {  }	break;

		case EAttackState::NormalBreath : { AttBreath(); }	break;
		case EAttackState::AimedBreath	: { AttBreath(); }	break;
	}
}


void UCSafiFSM::StartState()
{
	// ======== 탐지 -> 포효 -> 개전 ========

//탐지파트
	AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AHunter::StaticClass());
	if (!actor) { return; }
	target = Cast<AHunter>(actor);

	FVector dir = SearchTarget();

	//타겟과의 사거리가 색적범위보다 멀다면 타겟을 삭제
	if (dir.Size() > me->SearchRange)
	{
		target = nullptr;
		currentTime += GetWorld()->DeltaTimeSeconds;
		if (currentTime > me->idleTime)
		{
			return;
		}

		return;
	}

	// 위 조건을 지나옴 = 색적범위 안쪽
	me->isInBattle = true;


	// 포효
	// 포효 처리 및 공격 패턴으로 전환
	//  mState = ESafiState::Attack;
	//  OnRep_SafiState();
	ServerSetActState(ESafiState::Attack);
	AttRoar();
}

// 타겟 사망시에 IdleState로 돌아옴
void UCSafiFSM::IdleState()
{
	// AnimState에서 애니메이션 분기 만들어 놓을 것.	
	// ㄴ isInBattle이 true일 땐 일반 idle이 아니라 팔 움직이는 idle (회전용)		- 수행 완료

	if (me->isInBattle == false)	// 처음 조우할땐 StartState로 감
	{
		StartState();
		return;
	}

	CanMeleeAttack();	// 공격 수행시 노티파이로 attackPos = 0;		- 미수행

	// 공격 판단부분도 Idle에서 수행
	FRotator targetRot = SearchTarget().Rotation();
	FRotator currentRot = me->GetActorRotation();
	float targetYaw = FMath::Abs(FMath::FindDeltaAngleDegrees(currentRot.Yaw, targetRot.Yaw));


	// 이 부분은 따로 떼서 서버처리 하는게 나을듯 (각 차이 나면 모션 다르게 나올 수 있음.)
	// 돌아야 하는 값이 60도 미만이라면 TargetRotationByAnim으로 회전


	// currentTime += GetWorld()->DeltaTimeSeconds;
	// if (currentTime <= me->idleTime) { return; }

	// 돌아야 한다면 플레이어 방향으로 회전, 아닐시 return; - TargetRotationByAnim 에서 return;


	// 돌아야 하는 값이 60도 이상이라면 TargetRotationByAnim으로 회전
	// 뭘 하고싶었던거지... 재점검하기
	if (targetYaw <= 3.f && !isRot)
	{
		me->SetActorRotation(targetRot); // 최종 방향 고정
		isRot = false; // 회전 완료
		// mState = ESafiState::Attack; // 즉시 공격 상태로 전환
		// OnRep_SafiState();
		ServerSetActState(ESafiState::Attack);
		CanMeleeAttack();
		OnAttackProcess();
		return;
	}

	// 회전이 필요한 경우
	if (targetYaw >= 60.0f && !isRot)
	{
		isRot = true;
		TargetRotationByAnim(); // 큰 각도 회전은 애니메이션으로 처리
	}
	else if (isRot)
	{
		TargetRotation(); // 부드러운 회전
	}
	
}

void UCSafiFSM::CanMeleeAttack()	// SetAttackType으로 이름 바꾸고 근접공격 파트만 빼는것도 나쁘지 않을듯.
{
	FVector dir = SearchTarget();
	// BFattType = attType;


	// 어떤 공격을 할 지 판별 	// 공격 타입 번호만 정해주기	
	// 회전중에 다리에 걸릴 경우에는 바로 내리친다.


	// 여기랑 섞어서 if문 돌리는것도 해보기.
	if (me->attackPos == AttMELEE_LF || me->attackPos == AttMELEE_RF || me->attackPos == AttMELEE_LB || me->attackPos == AttMELEE_RB)
	{
		// 이전 공격타입이 동일한 경우엔 바디 프레스로 전환

		attType = me->attackPos;
		if (BFattType == attType){ attType = AttBPRESS; }

		//mTurnState = ETurnState::None;
		//OnRep_TurnState();
		ServerSetTurnState(ETurnState::None);

		OnAttackProcess();
	}
	
	// 우선 공격 사거리 체크, 사거리보다 멀리 있다면 브레스
	else if (dir.Size() < me->MeleeAttRange )
	{
		int iMelee = FMath::RandRange(AttBITE, AttBITE + 1);
		while (BFattType == iMelee)
		{
			iMelee = FMath::RandRange(AttBITE, AttBITE + 1);
		}
		attType = iMelee;
	}

	else
	{
		int iBreathType = FMath::RandRange(AttNMBREATH, AttNMBREATH + 1);
		while (BFattType == iBreathType)
		{
			iBreathType = FMath::RandRange(AttNMBREATH, AttNMBREATH + 1);
		}
		attType = iBreathType;
	}

	// 해당 위치에 적이 없는 경우는 노티파이로 EndAttackProcess	- 수행 완료
	// ㄴ 여기에서 브레스 종류 결정하기
}

/*
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
		OnRep_SafiState();

		currentTime = 0.f;
	}

}
*/

void UCSafiFSM::AttRoar()
{
	// me->isImmune = true;

	// mAttState = EAttackState::Roar;
	// OnRep_AttState();
	ServerSetAttState(EAttackState::Roar);


	// 포효 공격판정 실행						- *** 이건 플레이어 함수 불러와야 할 듯?
	// ㄴ> 여기서 하지 말고 노티파이로 할 것

	// 노티파이 종료시 EndAttackProcess 호출	- 수행완료

}

/*
void UCSafiFSM::AttMelee()
{
	// 노티파이로 isOnAttBite = true 활성화  - 수행완료
	// isOnAttBite 상태라면 Collision_1 활성화, 비활성화 - 캐릭터 자체 틱으로 옮김  - 수행완료
	// 노티파이로 isOnAttBite = false	- 수행완료
	// 노티파이로 공격이 끝날땐 정리 프로세스 - 수행완료
}
*/
void UCSafiFSM::AttBreath()
{
	// 브레스 사용중에만 실행
	if (me->isOnBreath == false){ return; }

	FVector Start = me->FireArrowComp->GetComponentLocation();
	FVector Forward = (SearchTarget() - Start).GetSafeNormal();
	
	// FVector Start = me->FireArrowComp->GetComponentLocation();
	// FVector Forward = me->FireArrowComp->GetForwardVector();


	float MaxDistance = me->MaxBreathRange; 
	FVector End = Start + Forward * MaxDistance * 5000.f;

	// 트레이스 파라미터
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(me);
	TraceParams.AddIgnoredComponent(me->GetMesh());

	FHitResult Hit;

	bool bHit = GetWorld()->SweepSingleByChannel( Hit,Start,End,FQuat::Identity, ECC_GameTraceChannel1, FCollisionShape::MakeSphere(50.f), TraceParams );


// ================== 디버그용 =====================================

	DrawDebugSphere(GetWorld(), Start, 50.f, 12, FColor::Green, false, 0.05f);
	DrawDebugLine(GetWorld(), Start, bHit ? Hit.Location : End, FColor::Red, false, 2.0f);
	if (bHit)
	{
		DrawDebugSphere(GetWorld(), Hit.Location, 50.f, 12, FColor::Blue, false, 2.0f);
	}

// ================== 디버그용 =====================================

	// 충돌한 대상이 있는지 체크
	if (!bHit){ return; }

	AActor* HitActor = Hit.GetActor();
	if (!HitActor)
	{ 
		AHunter* hunter = Cast<AHunter>(HitActor);
		if (hunter)
		{
			//Hunter->SetDamage(_value);
		}
		return; 
	}



}

// mState를 Attack으로 변경 / 공격스위치
void UCSafiFSM::OnAttackProcess()
{
	isRot = false;
	me->isOnSearch = false;
	FVector dir = SearchTarget();


// ======================== 스위치 ======================== 
// 
	// attType에 따라 mAttState를 변경만 해준다.
	switch (attType)
	{
	case AttNONE:
		//mAttState = EAttackState::None;
		ServerSetAttState(EAttackState::None);
		break;

	case AttROAR:
		//mAttState = EAttackState::Roar;
		ServerSetAttState(EAttackState::Roar);
		break;
//========================== 팔다리 공격 부분 ==========================
	case AttMELEE_LF:
		//mAttState = EAttackState::MeleeAttLF;
		ServerSetAttState(EAttackState::MeleeAttLF);
		break;

	case AttMELEE_RF:
		//mAttState = EAttackState::MeleeAttRF;
		ServerSetAttState(EAttackState::MeleeAttRF);
		break;

	case AttMELEE_RB:
		//mAttState = EAttackState::MeleeAttRB;
		ServerSetAttState(EAttackState::MeleeAttRB);
		break;

	case AttMELEE_LB:
		//mAttState = EAttackState::MeleeAttLB;
		ServerSetAttState(EAttackState::MeleeAttLB);
		break;
//========================== 브레스 부분 ==========================
	case AttNMBREATH:
		//mAttState = EAttackState::NormalBreath;
		ServerSetAttState(EAttackState::NormalBreath);
		break;
	case AttAIMBREATH:
		//mAttState = EAttackState::AimedBreath;
		ServerSetAttState(EAttackState::AimedBreath);
		break;
//========================== 근접공격 부분 ==========================
	case AttBITE:
		//mAttState = EAttackState::MeleeBite;
		ServerSetAttState(EAttackState::MeleeBite);
		break;
	case AttBPRESS:
		//mAttState = EAttackState::MeleeBPress;
		ServerSetAttState(EAttackState::MeleeBPress);
		break;
	} 
	// OnRep_AttState();


// ========================================================

	//공격 상태로의 전환, 수행
	if (mState != ESafiState::Attack)
	{
		// mState = ESafiState::Attack;
		// OnRep_SafiState();
		ServerSetActState(ESafiState::Attack);
	}

	// 여기여기여기
	// attType = AttNONE;	// 공격 타입 확정 후 초기화
	 me->attackPos = 0;	// 공격 후엔 팔공격도 초기화
}


void UCSafiFSM::EndAttackProcess()
{
	// mAttState = EAttackState::None;
	// OnRep_AttState();
	ServerSetAttState(EAttackState::None);
	// mState = ESafiState::Idle;
	// OnRep_SafiState();
	ServerSetActState(ESafiState::Idle);

	me->isOnSearch = true;	// 공격할 때 꺼주기	- OnAttackProcess에 false 해줌



	//CanMeleeAttack();		// 공격 가능 대상 있다면 바로 공격

	//if 돌아야 한다면 플레이어 방향으로 회전, 아닐시 return;		- 수행완료
	// ㄴ> 회전 적게해야할지 많이해야할지를 판단때려줌.				- 수행완료
	// ㄴ> Idle에서 수행.											- 수행완료

	BFattType = attType;
}

void UCSafiFSM::OnDisturbedProcess()
{
	//Disturbed 외엔 전부 None으로
	//mState = ESafiState::Disturbed;
	//OnRep_SafiState();
	ServerSetActState(ESafiState::Disturbed);

	//mAttState = EAttackState::None;
	//OnRep_AttState();
	ServerSetAttState(EAttackState::None);

	//mTurnState = ETurnState::None;
	//OnRep_TurnState();
	ServerSetTurnState(ETurnState::None);

	// 각도 측정 및 넉백()

	// 조건에 따라 스위치
	if (me->isDead == true)
	{
		// mDisturbState = EDisturbState::Dead;
		// OnRep_DisturbState();
		ServerSetDisturbState(EDisturbState::Dead);

		return;								// Dead 일경우 하위 상황 판단할 필요가 없음
	}


	if (me->isKnockBack == true)
	{ 
		TargetKnockBackByAnim();
	}

}


void UCSafiFSM::TargetRotation()
{
	float DeltaTime = GetWorld()->DeltaTimeSeconds;
	FVector dir = SearchTarget();

	FRotator TargetRotation = dir.Rotation();
	FRotator CurrentRotation = me->GetActorRotation();

	float targetYaw = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, TargetRotation.Yaw));
	if (targetYaw <= 3.f)
	{
		me->SetActorRotation(TargetRotation);
		isRot = false;
		OnAttackProcess();
		return;
	}

	FRotator NewRotation = FMath::RInterpTo(CurrentRotation , TargetRotation, DeltaTime, 2.f);


	me->SetActorRotation(NewRotation);
}

void UCSafiFSM::TargetRotationByAnim()
{
	FVector dir = SearchTarget();
	FVector dirLocal = me->GetActorTransform().InverseTransformVectorNoScale(dir);	// 로컬 방향 계산

	
	// 캐릭터가 앞에 있음. 회전 불필요. return.
	// 나중에 idle이나 백스텝 밟으면서 rotation만 조금 돌리게 수정할 것.

	//if (dirLocal.X > 0)	{ return; }		// 앞에 있을 경우

	// mState = ESafiState::Turn;
	// OnRep_SafiState();
	ServerSetActState(ESafiState::Turn);

	//=============================
	if (dirLocal.X < 0)	// 캐릭터가 뒤에 있음
	{
		// 회전 후방으로.
		// mTurnState = ETurnState::TrunBack;
		// OnRep_TurnState();
		ServerSetTurnState(ETurnState::TrunBack);
	}

	else if (dirLocal.Y < 0 && dirLocal.X > 0)	// 캐릭터가 좌측이지만 뒤는 아님
	{
		// 회전 좌측으로
		// mTurnState = ETurnState::TurnLeft;
		// OnRep_TurnState();
		ServerSetTurnState(ETurnState::TurnLeft);
	}


	else if (dirLocal.Y > 0 && dirLocal.X > 0)	// 캐릭터가 우측이지만 뒤는 아님
	{
		// 회전 우측으로
		// mTurnState = ETurnState::TurnRight;
		// OnRep_TurnState();
		ServerSetTurnState(ETurnState::TurnRight);
	}

	// 적이 해당 위치에 있다면 공격으로 전환 -> TrunState( Turn Tick에서 )		- 미수행


	// 노티파이로 isOnSearch 꺼주기			- 수행완료
	//	ㄴ OnAttackProcess도 같이			- 수행완료
}

void UCSafiFSM::TargetKnockBackByAnim()
{
	FVector dir = SearchTarget();
	FVector dirLocal = me->GetActorTransform().InverseTransformVectorNoScale(dir);

	// 캐릭터가 뒤에 있음
	if (dirLocal.X < 0)	{ mDisturbState = EDisturbState::KB_Backward; } // 넉백 돌면서 후방으로
	// 캐릭터가 앞에 있음 
	if (dirLocal.X > 0) { mDisturbState = EDisturbState::KB_Forward; } // 앞에서 뒤로 밀려남
	// 캐릭터가 좌측이지만 뒤는 아님
	else if (dirLocal.Y < 0 && dirLocal.X > 0) { mDisturbState = EDisturbState::KB_Left; } // 회전 좌측으로
	// 캐릭터가 우측이지만 뒤는 아님
	else if (dirLocal.Y > 0 && dirLocal.X > 0) { mDisturbState = EDisturbState::KB_Right; } // 회전 우측으로

	OnRep_DisturbState();	// 동기화
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
 















// ===================================== 서버용 ======================================================

void UCSafiFSM::OnRep_SafiState()
{
	if (Anim) Anim->aState = mState;
}

void UCSafiFSM::OnRep_AttState()
{
	if (Anim) Anim->aAttState = mAttState;
}

void UCSafiFSM::OnRep_TurnState()
{
	if (Anim) Anim->aTurnState = mTurnState;
}

void UCSafiFSM::OnRep_DisturbState()
{
	if (Anim) Anim->aDisturbState = mDisturbState;
}

void UCSafiFSM::ServerSetActState_Implementation(ESafiState _newState)
{
	mState = _newState;
	OnRep_SafiState();
}

// 서버

void UCSafiFSM::ServerSetAttState_Implementation(EAttackState _newAttState)
{
	mAttState = _newAttState;
	OnRep_AttState();
}

void UCSafiFSM::ServerSetTurnState_Implementation(ETurnState _newTurnState)
{
	mTurnState = _newTurnState;
	OnRep_TurnState();
}

void UCSafiFSM::ServerSetDisturbState_Implementation(EDisturbState _newDistState)
{
	mDisturbState = _newDistState;
	OnRep_DisturbState();
}

void UCSafiFSM::SetActState(ESafiState _newState)
{
	//if (GetOwner()->HasAuthority() && mState != _newState)
	if (GetOwnerRole() == ROLE_Authority)
	{
		mState = _newState;
		// 뭔가의 추가 내용
	}
	else
	{
		ServerSetActState(_newState);
	}

}

void UCSafiFSM::SetAttState(EAttackState _newAttState)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		mAttState = _newAttState;
		// 뭔가의 추가 내용
	}
	else
	{
		ServerSetAttState(_newAttState);
	}
}

void UCSafiFSM::SetTurnState(ETurnState _newTurnState)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		mTurnState = _newTurnState;
		// 뭔가의 추가 내용
	}
	else
	{
		ServerSetTurnState(_newTurnState);
	}
}

void UCSafiFSM::SetDisturbState(EDisturbState _newDistState)
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		mDisturbState = _newDistState;
		// 뭔가의 추가 내용
	}
	else
	{
		ServerSetDisturbState(_newDistState);
	}
}

void UCSafiFSM::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCSafiFSM, mState);
	DOREPLIFETIME(UCSafiFSM, mAttState);
	DOREPLIFETIME(UCSafiFSM, mTurnState);
	DOREPLIFETIME(UCSafiFSM, mDisturbState);
}

