// Fill out your copyright notice in the Description page of Project Settings.


#pragma region AttDefine

enum class EAttackNumber : uint8
{
	None = 0,
	MeleeLF = 2,
	MeleeRF = 3,
	MeleeLB = 4,
	MeleeRB = 5,

	NMBreath = 6,
	AIMBreath = 7,

	Roar = 10,

	Bite = 11,
	BPress = 12
};

#define AttNONE 0
#define AttROAR 1

#define AttMELEE_LF 2
#define AttMELEE_RF 3
#define AttMELEE_LB 4
#define AttMELEE_RB 5

#define AttNMBREATH 6
#define AttAIMBREATH 7

#define AttBITE 10
#define AttBPRESS 11

#pragma endregion

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CSafiFSM.generated.h"

UENUM()
enum class ESafiState : uint8
{
	Start		UMETA(DisplayName = "Start"),
	Idle		UMETA(DisplayName = "Idle"),
	//Move		UMETA(DisplayName = "Move"),
	Turn		UMETA(DisplayName = "Turn"),		// AnimState 제어용. 코드적 기능은 Idle에 전부 있음.
	Attack		UMETA(DisplayName = "Attack"),
	Disturbed	UMETA(DisplayName = "Disturbed")	// AnimState 제어용2.
	// Fly			UMETA(DisplayName = "Fly") / Fly는 그냥 IsFly로 상태 체크하는게 나을듯?
};

UENUM()
enum class EAttackState : uint8
{
	None 				UMETA(DisplayName = "None"),
	Roar				UMETA(DisplayName = "Roar"),
	MeleeAttLF			UMETA(DisplayName = "Melee_AttLF"),
	MeleeAttRF			UMETA(DisplayName = "Melee_AttRF"),
	MeleeAttLB			UMETA(DisplayName = "Melee_AttLB"),
	MeleeAttRB			UMETA(DisplayName = "Melee_AttRB"),
	MeleeBite			UMETA(DisplayName = "Melee_Bite"),
	MeleeBPress			UMETA(DisplayName = "Melee_BodyPress"),
	NormalBreath		UMETA(DisplayName = "Normal_Breath"),
	AimedBreath			UMETA(DisplayName = "Aimed_Breath")
};

UENUM()
enum class ETurnState : uint8
{
	None 				UMETA(DisplayName = "None"),
	TurnLeft			UMETA(DisplayName = "Turn_Left"),
	TurnRight			UMETA(DisplayName = "Turn_Right"),
	// 사실 왼회전 우회전 모션 다 있긴 한데 지금은 중요해보이진 않아서 제외.
	TrunBack			UMETA(DisplayName = "Trun_Back")
};

UENUM()
enum class EDisturbState : uint8
{
	None 				UMETA(DisplayName = "None"),
	// 헷갈린다. 맞는 방향 기준이다.
	KB_Left				UMETA(DisplayName = "KB_Left"),
	KB_Right			UMETA(DisplayName = "KB_Right"),
	KB_Forward			UMETA(DisplayName = "KB_Forward"),
	KB_Backward			UMETA(DisplayName = "KB_Backward"),
	Dead				UMETA(DisplayName = "Dead")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_SAFI_JIIVA_API UCSafiFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCSafiFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	class ACSafiJiiva* me;

	UPROPERTY()
	class UCSafiAnimInstance* Anim;
	
	UPROPERTY()
	class AHunter* target;


public:
	UPROPERTY(EditDefaultsOnly, Category = FSM)
	ESafiState mState = ESafiState::Idle;

	UPROPERTY(EditDefaultsOnly, Category = FSM)
	EAttackState mAttState = EAttackState::None;

	UPROPERTY(EditDefaultsOnly, Category = FSM)
	ETurnState mTurnState = ETurnState::None;

	UPROPERTY(EditDefaultsOnly, Category = FSM)
	EDisturbState mDisturbState = EDisturbState::None;


// Define 이걸로 바꾸는게 낫나?
	//EAttackNumber AttNum = EAttackNumber::None;
public:
	float currentTime = 0.f;
	int attType = AttNONE;
	//EAttackNumber attType = EAttackNumber::None;


private: // 기본 State 함수
	void StartState();
	void IdleState();
	void MoveState();
	void BreathState();

public:	// AttState 함수
	void OnAttackProcess();		// 공격 스위치 시켜주기
	void EndAttackProcess();	// AttState None 복귀, 다음 공격 판단
	// KnockBack, Dead
	void OnDisturbedProcess();	// 상태이상 걸렸을 때 State 전부 정지, Disturbed로 전환.

public: // 공격 관련 함수
	void AttRoar();
	void AttMelee();
	void AttBreath();

	void CanMeleeAttack();				// 공격 가능 위치에 정확히 있는지 확인 , 브레스 종류 랜덤 결정.

public:
	void TargetRotation();
	void TargetRotationByAnim();		// 애니메이션으로 회전
	void TargetKnockBackByAnim();
	FVector SearchTarget();

public:
	bool isRot = false;				// 좀 한 번만 돌자 :)....
	int BFattType = 0;
};
