// Fill out your copyright notice in the Description page of Project Settings.


#include "Hunter/MoveComponent.h"
#include "AssetPath.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputAction.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"

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
		InputComponent->BindAction(IA_Turn, ETriggerEvent::Triggered, this, &UMoveComponent::Turn);
	}
}

void UMoveComponent::Move(const FInputActionValue& Value)
{
	FVector Direction = Value.Get<FVector>();
	PRINT_LOG(TEXT("Move Direction: %s"), *Direction.ToString());
}

void UMoveComponent::Turn(const FInputActionValue& Value)
{
	FVector Direction = Value.Get<FVector>();
	PRINT_LOG(TEXT("Turn Direction: %s"), *Direction.ToString());

}

