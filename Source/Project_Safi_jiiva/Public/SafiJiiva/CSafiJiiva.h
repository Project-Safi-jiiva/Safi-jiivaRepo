// Fill out your copyright notice in the Description page of Project Settings.
#define AttMELEE_LF 2
#define AttMELEE_RF 3
#define AttMELEE_LB 4
#define AttMELEE_RB 5


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Hunter/Hunter.h"
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

// =================== 피격용 콜리전 ===================
#pragma region Collision_Body

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* BodyColHead;


	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Socket_Body_31;
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Socket_Body_32;


	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Socket_Body_1;




	///////////// 다리
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Socket_Body_2;
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Socket_Body_3;
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Socket_Body_33;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Socket_Body_4;
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Socket_Body_5;
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Socket_Body_34;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Socket_Body_6;
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Socket_Body_7;
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Socket_Body_8;
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Socket_Body_35;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Socket_Body_9;
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Socket_Body_10;
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Socket_Body_11;
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Socket_Body_36;
#pragma endregion Collision_Body

// =================== 공격용 콜리전 ===================

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* AttCollisionBite;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* AttCollisionLF;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* AttCollisionRF;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* AttCollisionLB;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* AttCollisionRB;

	
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Socket_Body_37;

	UPROPERTY()
	TArray<UBoxComponent*> TailCollisionBoxes;


// =================== 공격 위치 판정용 콜리전===================
#pragma region Collision_Att
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* AttPosLF;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* AttPosRF;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* AttPosLB;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* AttPosRB;
#pragma endregion Collision_Att


private:	// 함수용 변수들
	float currentTime = 0.f;

public:	// 체력 등 스탯 계수
	float MAXHP = 50.f;
	float hp = MAXHP;
	float MeleeBiteDMG = 30.f;	// 물기 데미지

	int32 RepellCount = 0;
	int32 MAXRepellCount = 2;
	int32 SpecialCount = 0;
	int32 MAXSpecialCount = 5;


public:	// 사거리, 속도 등
	float MeleeAttRange = 2000.f;
	float BiteRange = 500.f;
	float SearchRange = 5000.f;
	float MaxBreathRange = 5000.f;

	float RunSpeed = 400.f;
	float WalkSpeed = 100.f;

	float idleTime = 2.f;

public: // 공격 위치
	// 왼쪽 = 2, 오른쪽 = 3, 오른쪽 뒤 = 4
	int attackPos =	0;	

public: // 상태 체크
	bool isInBattle = false;			// 전투상태인가
	// bool isFly = false;				// 비행 상태인가
	bool isImmune = false;				// 특정 상황에서는 상태이상 면역.

	//======================== 상태이상 관련 //========================
	bool isDisturbed = false;		// 상태이상에 걸렸는지 - 애니메이션 끊길 때 노티파이 삭제 대체용
									// 
	bool isKnockBack = false;		// 넉백당했는지
	bool isDead = false;

public:// 공격 관련 노티파이 제어
	bool isOnAttBite = false;		// 물기 공격중인지
	bool isFootAttack = false;		// 발 공격중인지
	bool isOnBreath = false;		// 브레스 사용중인지
	bool isOnSearch = false;		// 탐색 중인지 (탐색용 콜리전 활성화)
	bool isOnBodyPress = false;		// 바디프레스 사용중인지
	bool isOnSpecial = false;


public: // 라인 트레이스
	UFUNCTION()
	void DrawLineTrace();
	bool CheckHitLineTrace(FVector _startPos, FVector& _curPos);

public:
	void SetNormal();		// bool형 변수들 false로 전부 바꿔주는 함수	

public:
	void SetSpeed(float _value);
	//void OnDamageSafi(float _value);
	void KillSafi_Test();

	void InitBoxes();

public:	// 충돌처리

	TArray<AHunter*> HitPawn;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()	
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()	
	void OnAttackOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
