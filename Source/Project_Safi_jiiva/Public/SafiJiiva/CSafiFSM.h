// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CSafiFSM.generated.h"

UENUM()
enum class ESafiState : uint8
{
	Idle		UMETA(DisplayName = "Idle"),
	//Move		UMETA(DisplayName = "Move"),
	Attack		UMETA(DisplayName = "Attack"),
	Dead		UMETA(DisplayName = "Dead")
	// Fly			UMETA(DisplayName = "Fly") / Fly는 그냥 IsFly로 상태 체크하는게 나을듯?
};

UENUM()
enum class EAttackState : uint8
{
	None			UMETA(DisplayName = "None"),
	Roar			UMETA(DisplayName = "Roar"),
	MeleeBite		UMETA(DisplayName = "Melee_Bite"),
	MeleeBP			UMETA(DisplayName = "Melee_BodyPress"),
	NormalBreath	UMETA(DisplayName = "Normal_Breath"),
	AimedBreath		UMETA(DisplayName = "Aimed_Breath")
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

public:
	float currentTime = 0.f;
	int AttType = 0;	// 랜덤 공격 번호로 사용할 예정

private: // 기본 State 함수
	void IdleState();
	void MoveState();
	void BreathState();

public:	// AttState 함수
	void OnAttackProcess();

public: // 공격 관련 함수
	void AttRoar();
	void AttMelee();
	void AttBreath();
public:
	void TargetRotation();
	FVector SearchTarget();

public: // Notify 관련

};
