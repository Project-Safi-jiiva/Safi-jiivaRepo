// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Weapon/EWeaponType.h"
#include "Hunter.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FInputBindingDeleagate, class UEnhancedInputComponent*)

UCLASS()
class PROJECT_SAFI_JIIVA_API AHunter : public ACharacter
{
	GENERATED_BODY()
private:
	const float MaxHP=200;
	const float MaxStamina=200;

	float HP = MaxHP;
	float Stamina = MaxStamina;

public:
	// Sets default values for this character's properties
	AHunter();
public:
	FInputBindingDeleagate InputBindingDeleagate;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

protected:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:
	UPROPERTY(EditDefaultsOnly, Category = "MoveComp")
	class UMoveComponent* MoveComp;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
    void ChangeWeapon(EWeaponType NewWeaponType);

	UPROPERTY(EditAnywhere,Category="Camera")
	class UCameraComponent* CameraComponent;

	UPROPERTY(EditDefaultsOnly, Category = "MoveComp")
	class UWeaponComponent* WeaponComp;
protected:
	class UHunterAnim* Anim;

	UPROPERTY(EditAnywhere, Category = "Camera")
	class USpringArmComponent* SpringArmComponent;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* IMC_Hunter;

	class USkeletalMeshComponent* SkeletalMeshComp;

public:
	UPROPERTY(Replicated)
	bool isRun = false;

	UPROPERTY(Replicated)
	bool isHit = false;
	//서버 설정 함수

	void NetLog();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	///////////////////////////서버 함수//////////////////////////

	//대쉬
	UFUNCTION(Server, Reliable)
	void ServerRPC_Dash();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_Dash();
	UFUNCTION(Server, Reliable)
	void ServerRPC_DashEnd();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_DashEnd();

	//약공격 입력
	UFUNCTION(Server, Reliable)
	void ServerRPC_QuickStart();
	UFUNCTION(Server, Reliable)
	void ServerRPC_QuickHolding();
	UFUNCTION(Server, Reliable)
	void ServerRPC_QuickEnd();
	//약공격
	UFUNCTION(Server, Reliable)
	void ServerRPC_QuickAttack();
	UFUNCTION(NetMulticast, Reliable)
	void MulticasrRPC_QuickAttack();
	//강공격 입력
	UFUNCTION(Server, Reliable)
	void ServerRPC_HeavyStart();
	UFUNCTION(Server, Reliable)
	void ServerRPC_HeavyHolding();
	UFUNCTION(Server, Reliable)
	void ServerRPC_HeavyEnd();
	//강공격
	UFUNCTION(Server, Reliable)
	void ServerRPC_HeavyAttack();
	UFUNCTION(NetMulticast, Reliable)
	void MulticasrRPC_HeavyAttack(const struct FWeaponDataTable& CurrentData);
	//특수공격
	UFUNCTION(Server, Reliable)
	void ServerRPC_UniqueAttack();
	UFUNCTION(NetMulticast, Reliable)
	void MulticasrRPC_UniqueAttack();
	//차지 공격
	UFUNCTION(Server,Reliable)
	void ServerRPC_ChargeAttack();
	UFUNCTION(NetMulticast, Reliable)
	void MulticasrRPC_ChargeAttack(const struct FWeaponDataTable& CurrentData);


	//UFUNCTION(Server, Reliable)
	//void ServerRPC_CommandInputReset();

	//UFUNCTION(NetMulticast, Reliable)
	//void MulticastRPC_CommandInputReset();

	//공격 중인지 판단
	UFUNCTION(Server, Reliable)
	void ServerRPC_SetIsAttacking(bool IsAttack);

	//약공격 판단
	UFUNCTION(Server, Reliable)
	void ServerRPC_SetIsQuickAttack(bool IsQuick);
	//강공격 판단
	UFUNCTION(Server, Reliable)
	void ServerRPC_SetIsHeavyAttack(bool IsHeavy);
	//특수 공격 판단
	UFUNCTION(Server, Reliable)
	void ServerRPC_SetIsUniqueAttack(bool IsUnique);
	//태클 판단
	UFUNCTION(Server, Reliable)
	void ServerRPC_SetIsTacle(bool IsTacle);
	UFUNCTION(Server, Reliable)
	void ServerRPC_SetIsHolding(bool IsHolding);
	//딜레이 판단
	UFUNCTION(Server, Reliable)
	void ServerRPC_SetIsJumpDelay(bool IsJumpDelay);
	//구르기 허용 여부 판단
	UFUNCTION(Server, Reliable)
	void ServerRPC_SetAllowRoll(bool AllowRoll);


	//무기 손에 붙이기
	UFUNCTION(Server, Reliable)
	void ServerRPC_AttachWeaponToHand();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_AttachWeaponToHand();
	//무기 손에서 때고 등에 붙이기
	UFUNCTION(Server, Reliable)
	void ServerRPC_AttachWeaponToOwner();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_AttachWeaponToOwner();

	//약공격 인덱스 올리기
	UFUNCTION(Server, Reliable)
	void ServerRPC_SetQuickAddIndex(int32 AddIndex);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_SetQuickAddIndex(int32 AddIndex);
	//강공격 인덱스 올리기
	UFUNCTION(Server, Reliable)
	void ServerPRC_SetHeavyAddIndex();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_SetHeavyAddIndex();
	//특수공격 인덱스 올리기
	UFUNCTION(Server, Reliable)
	void ServerPRC_SetUniqueAddIndex(int32 AddIndex);
	//공격중 약공격 연계
	UFUNCTION(Server, Reliable)
	void ServerRPC_QuickStrikeNext();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_QuickStrikeNext();
	//공격중 강공격 연계
	UFUNCTION(Server, Reliable)
	void ServerRPC_HeavyStrikeNext();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_HeavyStrikeNext(const struct FWeaponDataTable& CurrentData);

	//콤보 리셋
	UFUNCTION(Server, Reliable)
	void ServerRPC_ResetCombo();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastRPC_ResetCombo();

	//구르기 실행 함수
	UFUNCTION(Server, Reliable)
	void ServerRPC_Roll();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastRPC_Roll();
	//다음 콤보로 점프
	UFUNCTION(Server, Reliable)
	void ServerRPC_JumpToNextCombo();
	UFUNCTION(NetMulticast, Reliable)
	void NetMulticastRPC_JumpToNextCombo();
	//힛 판정
	UFUNCTION(Server,Reliable)
	void ServerRPC_HitEvent();

	UFUNCTION(Server, Reliable)
	void ServerRPC_OnWeaponComp();

	UFUNCTION(Server, Reliable)
	void ServerRPC_OffWeaponComp();












};
