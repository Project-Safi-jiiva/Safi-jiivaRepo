// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "MoveComp")
	class UMoveComponent* MoveComp;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* IMC_Hunter;

	class UHunterAnim* Anim;
	class USkeletalMeshComponent* SkeletalMeshComp;

};
