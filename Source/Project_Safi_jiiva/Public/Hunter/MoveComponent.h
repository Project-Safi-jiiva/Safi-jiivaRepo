// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseComponent.h"
#include "Project_Safi_jiiva.h"
#include "EMoveState.h"
#include "MoveComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECT_SAFI_JIIVA_API UMoveComponent : public UBaseComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMoveComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


protected:
	virtual void SetupInputBinding(class UEnhancedInputComponent* InputComponent) override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Move;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* IA_Turn;
private:

	void MoveStart();
	void MoveEnd();
	void Move(const struct FInputActionValue& Value);

	void Turn(const struct FInputActionValue& Value);

	float InputAngle = 0.0f;

	float AngleDifference = 0.0f;

	float CurrentCharacterAngle = 0.0f;

	FVector2D Direction;

public:
	EMoveState MoveState = EMoveState::IDLE;

	float CalculateDirection(const FVector& Velocity, const FRotator& BaseRotation) const;
};
