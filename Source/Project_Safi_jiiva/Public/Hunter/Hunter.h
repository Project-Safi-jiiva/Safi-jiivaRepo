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
	UFUNCTION(Server, Reliable)
	void ServerRPC_Dash();
	UFUNCTION(Server, Reliable)
	void ServerRPC_DashEnd();

	UFUNCTION(Server, Reliable)
	void ServerRPC_QuickStart();
	UFUNCTION(Server, Reliable)
	void ServerRPC_QuickHolding();
	UFUNCTION(Server, Reliable)
	void ServerRPC_QuickEnd();

	UFUNCTION(Server, Reliable)
	void ServerRPC_QuickAttack();
	UFUNCTION(NetMulticast, Reliable)
	void MulticasrRPC_QuickAttack();

	UFUNCTION(Server, Reliable)
	void ServerRPC_HeavyStart();
	UFUNCTION(Server, Reliable)
	void ServerRPC_HeavyHolding();
	UFUNCTION(Server, Reliable)
	void ServerRPC_HeavyEnd();

	UFUNCTION(Server, Reliable)
	void ServerRPC_HeavyAttack();
	UFUNCTION(NetMulticast, Reliable)
	void MulticasrRPC_HeavyAttack();

	UFUNCTION(Server, Reliable)
	void ServerRPC_UniqueAttack();
	UFUNCTION(NetMulticast, Reliable)
	void MulticasrRPC_UniqueAttack();

	UFUNCTION(Server,Reliable)
	void ServerRPC_ChargeAttack();
	UFUNCTION(NetMulticast, Reliable)
	void MulticasrRPC_ChargeAttack();


	//UFUNCTION(Server, Reliable)
	//void ServerRPC_CommandInputReset();

	//UFUNCTION(NetMulticast, Reliable)
	//void MulticastRPC_CommandInputReset();

	UFUNCTION(NetMulticast, Reliable)
	void ServerRPC_SetIsAttacking(bool IsAttack);
	UFUNCTION(NetMulticast, Reliable)
	void ServerRPC_SetIsQuickAttack(bool IsQuick);
	UFUNCTION(NetMulticast, Reliable)
	void ServerRPC_SetIsHeavyAttack(bool IsHeavy);
	UFUNCTION(NetMulticast, Reliable)
	void ServerRPC_SetIsUniqueAttack(bool IsUnique);
	UFUNCTION(NetMulticast, Reliable)
	void ServerRPC_SetIsTacle(bool IsTacle);
	UFUNCTION(NetMulticast, Reliable)
	void ServerRPC_SetIsHolding(bool IsHolding);
	UFUNCTION(NetMulticast, Reliable)
	void ServerRPC_SetIsJumpDelay(bool IsJumpDelay);
	UFUNCTION(NetMulticast, Reliable)
	void ServerRPC_SetAllowRoll(bool AllowRoll);

	UFUNCTION(Server, Reliable)
	void ServerRPC_AttachWeaponToHand();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_AttachWeaponToHand();
	UFUNCTION(Server, Reliable)
	void ServerRPC_AttachWeaponToOwner();
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_AttachWeaponToOwner();









};
