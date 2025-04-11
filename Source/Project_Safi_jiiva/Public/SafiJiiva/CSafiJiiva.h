// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CSafiJiiva.generated.h"


UCLASS()
class PROJECT_SAFI_JIIVA_API ACSafiJiiva : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACSafiJiiva();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	//virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public: //기본 세팅 파트

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FSM)
	class UCSafiFSM* FSM;

	UPROPERTY()
	class UCSafiAnimInstance* Anim;

	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* SafiComponent;

	UPROPERTY(EditDefaultsOnly)
	class UArrowComponent* FireArrowComp;

	UPROPERTY(EditDefaultsOnly)
	class UArrowComponent* LineArrowComp;


public:	// 소켓 추가 파트
#pragma region CollisionSocketPart

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Collision_1;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Collision_2;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Collision_3;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Collision_4;

#pragma endregion

private:	// 함수용 변수들
	float currentTime = 0.f;

public:	// 체력 등 스탯 계수
	float MAXHP = 100.f;
	float hp = MAXHP;
	float MeleeBiteDMG = 10.f;	// 물기 데미지


public:	// 사거리, 속도 등
	float MeleeAttRange = 1000.f;
	float SearchRange = 3000.f;

	float RunSpeed = 400.f;
	float WalkSpeed = 100.f;

	float idleTime = 3.f;

public: // 공격 위치
	// 왼쪽 = 2, 오른쪽 = 3, 오른쪽 뒤 = 4
	int attackPos =	0;	

public: // 상태 체크
	bool isInBattle = false;		// 전투상태인가
	bool isFly = false;				// 비행 상태인가
	bool isImmune = false;			// 특정 상황에서는 상태이상 면역.

	//======================== 상태이상 관련 //========================
	bool isDisturbed = false;		// 상태이상에 걸렸는지 - 애니메이션 끊길 때 노티파이 삭제 대체용
	bool isRepelled = false;		// 넉백당했는지

public:// 공격 관련 노티파이 제어
	bool isOnAttBite = false;		// 물기 공격중인지
	bool isBreath = false;			// 브레스 사용중인지
	bool isOnSearch = false;		// 탐색 중인지 (탐색용 콜리전 활성화)

public: // 라인 트레이스
	bool bLineTracing = false;


	UFUNCTION()
	void DrawLineTrace();
	bool CheckHitLineTrace(FVector _startPos, FVector& _curPos);

public:// bool형 변수들 false로 전부 바꿔주는 함수
	void SetNormal();	

public:
	void SetSpeed(float _value);
	void OnDamageSafi(float _value);
	void KillSafi_Test();

public:	// 충돌처리
	UFUNCTION()	
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
