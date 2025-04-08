// Fill out your copyright notice in the Description page of Project Settings.


#include "Hunter/MoveComponent.h"
#include "AssetPath.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputAction.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "Hunter/Hunter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
UMoveComponent::UMoveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UInputAction> IA_MoveTool(AssetPaths::IA_MOVE);
	ConstructorHelpers::FObjectFinder<UInputAction> IA_TurnTool(AssetPaths::IA_TURN);
	IA_Move = IA_MoveTool.Object;
	IA_Turn = IA_TurnTool.Object;

	// ...
}


// Called when the game starts
void UMoveComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner->GetCharacterMovement()->MaxAcceleration = 900.0f;
	// ...

}


// Called every frame
void UMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

void UMoveComponent::SetupInputBinding(class UEnhancedInputComponent* InputComponent)
{
	Super::SetupInputBinding(InputComponent);

	if (InputComponent)
	{
		InputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &UMoveComponent::Move);
		InputComponent->BindAction(IA_Move, ETriggerEvent::Started, this, &UMoveComponent::MoveStart);
		InputComponent->BindAction(IA_Move, ETriggerEvent::Completed, this, &UMoveComponent::MoveEnd);
		InputComponent->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &UMoveComponent::Turn);
	}
}

void UMoveComponent::MoveStart()
{
	MoveState = EMoveState::WALK;
}

void UMoveComponent::MoveEnd()
{
	MoveState = EMoveState::STOP;
}

void UMoveComponent::Move(const FInputActionValue& Value)
{
	FVector2D Scale = Value.Get<FVector2D>();
	Direction = Scale;

	// 카메라의 전방 방향 (앞뒤 이동)
	FVector ForwardDirection = Owner->CameraComponent->GetForwardVector();
	ForwardDirection.Z = 0.0f;
	ForwardDirection.Normalize();

	// 카메라의 오른쪽 방향 (좌우 이동)
	FVector RightDirection = Owner->CameraComponent->GetRightVector();
	RightDirection.Z = 0.0f;
	RightDirection.Normalize();

	// 이동 입력 적용
	Owner->AddMovementInput(ForwardDirection, Scale.X);
	Owner->AddMovementInput(RightDirection, Scale.Y);
}

void UMoveComponent::Turn(const FInputActionValue& Value)
{
	FVector2d Scale = Value.Get<FVector2d>();
	Owner->AddControllerPitchInput(Scale.Y);
	Owner->AddControllerYawInput(Scale.X);
}

