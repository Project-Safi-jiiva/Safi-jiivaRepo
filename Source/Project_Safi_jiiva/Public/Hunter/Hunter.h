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
	bool isRun = false;





};
