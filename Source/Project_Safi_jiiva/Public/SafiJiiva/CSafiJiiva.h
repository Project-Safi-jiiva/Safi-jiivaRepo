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

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public: //기본 세팅 파트
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = FSM)
	class UCSafiFSM* FSM;

	UPROPERTY()
	class UCSafiAnimInstance* Anim;

	UPROPERTY(EditDefaultsOnly)
	class USkeletalMeshComponent* SafiComponent;

	UPROPERTY(EditDefaultsOnly)
	class UArrowComponent* FireArrowComp;

public:	// 소켓 추가 파트
#pragma region CollisionSocketPart

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* Collision_1;

#pragma endregion

private:	// 함수용 변수들
	float currentTime = 0.f;

private:	// 체력 등 스탯 계수
	float MAXHP = 100.f;
	float hp = MAXHP;

private: // 상태 체크
	bool isDisturbed = false;		// 상태이상에 걸렸는지
	bool isBreath = false;			// 브레스 사용중인지
	bool isRepelled = false;		// 넉백당했는지

	void SetNormal();	// bool형 변수들 false로 전부 바꿔주는 함수

public:
	void OnDamageSafi(float _value);
	void KillSafi_Test();

public:	// 충돌처리
	UFUNCTION()	
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
