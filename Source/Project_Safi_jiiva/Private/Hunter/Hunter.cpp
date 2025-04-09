// Fill out your copyright notice in the Description page of Project Settings.

#include "Hunter/Hunter.h"
#include "Hunter/HunterAnim.h"
#include "Hunter/MoveComponent.h"

#include "AssetPath.h"
#include "Project_Safi_jiiva.h"

#include "Engine/LocalPlayer.h"
#include "UObject/ConstructorHelpers.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "EnhancedInput/Public/EnhancedInputComponent.h"
#include "EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "EnhancedInput/Public/InputMappingContext.h"
#include "Weapon/WeaponComponent.h"

// Sets default values
AHunter::AHunter()
{
	PrimaryActorTick.bCanEverTick = true;

	//컴포넌트 추가 부분
	MoveComp = CreateDefaultSubobject<UMoveComponent>(TEXT("MoveComponent"));
	WeaponComp = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponComponent"));
	//스켈레탈 메쉬 추가
	ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Hunter(AssetPaths::HUNTER_MESH);
	if (SK_Hunter.Succeeded()) GetMesh()->SetSkeletalMesh(SK_Hunter.Object);
	//애니메이션 블루프린트 추가
	ConstructorHelpers::FClassFinder<UHunterAnim> AB_Hunter(AssetPaths::HUNTER_ANIM);

	ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_HunterTool(AssetPaths::HUNTER_IMC);



	if (AB_Hunter.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(AB_Hunter.Class);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0,0,-200), FRotator(0,-90,0));
		Anim = Cast<UHunterAnim>(GetMesh()->GetAnimInstance());
		IMC_Hunter = IMC_HunterTool.Object;
	}
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	// 이동 방향이 카메라와 독립적이도록 설정
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	// 스프링 암 생성 및 설정
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 300.0f;
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bInheritPitch = true;
	SpringArmComponent->bInheritYaw = true;
	SpringArmComponent->bInheritRoll = false;


	// 카메라 생성 및 설정
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;

}

void AHunter::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PC = Cast<APlayerController>(GetController());
	UEnhancedInputLocalPlayerSubsystem* subSys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());

	if (subSys)
		subSys->AddMappingContext(IMC_Hunter, 0);

}

void AHunter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (isRun)
		GetCharacterMovement()->MaxWalkSpeed = 1000;
	else
		GetCharacterMovement()->MaxWalkSpeed = 600;
}

void AHunter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto PlayerInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if(InputBindingDeleagate.IsBound())
		InputBindingDeleagate.Broadcast(PlayerInput);

}

