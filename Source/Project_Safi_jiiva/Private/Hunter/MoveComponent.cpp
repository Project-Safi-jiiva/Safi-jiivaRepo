// Fill out your copyright notice in the Description page of Project Settings.


#include "Hunter/MoveComponent.h"
#include "AssetPath.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputAction.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "Hunter/Hunter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Hunter/HunterAnim.h"
#include "Weapon/WeaponComponent.h"
#include "Project_Safi_jiiva.h"
#include "Net/UnrealNetwork.h"
#include "Components/PrimitiveComponent.h"

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
	SetIsReplicatedByDefault(true);

	// ...
}

void UMoveComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner->GetCharacterMovement()->MaxAcceleration = 600.0f;
}

void UMoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (isRotation){
		Owner->SetActorRotation(FRotator(DirectionY));
	PRINT_LOG(TEXT("%f"), DirectionY);
	}

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

void UMoveComponent::MoveStart(){MoveState = EMoveState::WALK;}

void UMoveComponent::MoveEnd(){MoveState = EMoveState::STOP;}

void UMoveComponent::MoveTurn(){MoveState = EMoveState::TURN;}

void UMoveComponent::Move(const FInputActionValue& Value)
{
	FVector2D Scale = Value.Get<FVector2D>();
	Direction = Scale;
	DirectionY = Scale.Y;
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

void UMoveComponent::InputOff()
{
	if(Owner->IsLocallyControlled())
		Owner->DisableInput(Cast<APlayerController>(PC));

}

void UMoveComponent::InputOn()
{
	if (Owner->IsLocallyControlled())
		Owner->EnableInput(Cast<APlayerController>(PC));

}

void UMoveComponent::KnockBack(UPrimitiveComponent* DamageCauserComponent)
{
	if (!Owner || !DamageCauserComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("KnockBack: Owner or DamageCauserComponent is null"));
		return;
	}

	// 캐릭터와 DamageCauserComponent의 위치 가져오기
	FVector OwnerLocation = Owner->GetActorLocation();
	FVector CauserLocation = DamageCauserComponent->GetComponentLocation();

	// 넉백 방향 계산 (캐릭터에서 발로 향하는 벡터의 반대)
	FVector KnockbackDir = (OwnerLocation - CauserLocation).GetSafeNormal();

	// 수직 성분 추가 (넉백이 자연스럽게 위로 튀도록)
	KnockbackDir.Z += 0.5f;
	KnockbackDir = KnockbackDir.GetSafeNormal();

	// 넉백 세기 설정
	float KnockbackPower = 800.0f;

	// 넉백 힘 계산
	FVector KnockbackForce = KnockbackDir * KnockbackPower;

	// 현재 속도 상쇄
	FVector CurrentVelocity = Owner->GetVelocity();
	FVector LaunchVector = KnockbackForce - CurrentVelocity;

	// 캐릭터를 넉백 방향으로 날림
	Owner->LaunchCharacter(LaunchVector, true, true);

}

void UMoveComponent::EnableControllerRotaion()
{
	//Owner->bUseControllerRotationYaw = true;
	//Owner->GetCharacterMovement()->bOrientRotationToMovement = false;
	//PRINT_LOG(TEXT("START"));
	isRotation = true;
}

void UMoveComponent::DisableControllerRotaion()
{
	//Owner->bUseControllerRotationYaw = false;
	//Owner->GetCharacterMovement()->bOrientRotationToMovement = true;
	//PRINT_LOG(TEXT("END"));
	isRotation = false;

}

void UMoveComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(UMoveComponent, MoveState);
}

