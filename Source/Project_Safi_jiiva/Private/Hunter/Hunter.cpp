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
#include "GreatSword.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AHunter::AHunter()
{
	PrimaryActorTick.bCanEverTick = true;

	//������Ʈ �߰� �κ�
	MoveComp = CreateDefaultSubobject<UMoveComponent>(TEXT("MoveComponent"));
	//WeaponComp = CreateDefaultSubobject<UWeaponComponent>(TEXT("WeaponSample"));
	//���̷�Ż �޽� �߰�
	ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Hunter(AssetPaths::HUNTER_MESH);
	if (SK_Hunter.Succeeded()) GetMesh()->SetSkeletalMesh(SK_Hunter.Object);
	//�ִϸ��̼� ��������Ʈ �߰�
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


	// �̵� ������ ī�޶�� �������̵��� ����
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	// ������ �� ���� �� ����
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 300.0f;
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bInheritPitch = true;
	SpringArmComponent->bInheritYaw = true;
	SpringArmComponent->bInheritRoll = false;


	// ī�޶� ���� �� ����
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;
	GetCapsuleComponent()->SetCollisionProfileName(FName("Pawn2"));

	SpringArmComponent->bDoCollisionTest = false;
}

void AHunter::BeginPlay()
{
	Super::BeginPlay();
	ChangeWeapon(EWeaponType::GREATSWORD);
	Anim = Cast<UHunterAnim>(GetMesh()->GetAnimInstance());
	APlayerController* PC = Cast<APlayerController>(GetController());
	if(PC){
	UEnhancedInputLocalPlayerSubsystem* subSys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());

	if (subSys)
		subSys->AddMappingContext(IMC_Hunter, 0);
	}
}

void AHunter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	NetLog();

	if (Anim&& Anim->Montage_IsPlaying(nullptr)&&isHit)
		Anim->Montage_Stop(0.1f);

}

void AHunter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto PlayerInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if(InputBindingDeleagate.IsBound())
		InputBindingDeleagate.Broadcast(PlayerInput);

}

float AHunter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (WeaponComp->isTacle) return 0;
	PRINT_LOG(TEXT("%f"),Damage);
	ServerRPC_HitEvent();
	return Damage;
}

void AHunter::ChangeWeapon(EWeaponType NewWeaponType)
{
	if (WeaponComp)
	{
		WeaponComp->DestroyComponent();
		WeaponComp = nullptr;
	}

	switch (NewWeaponType)
	{
	case EWeaponType::GREATSWORD:
		WeaponComp = NewObject<UGreatSword>(this, TEXT("WeaponComponent"));
		break;
	default:
		WeaponComp = NewObject<UWeaponComponent>(this, TEXT("WeaponComponent"));
		break;
	}

	if (WeaponComp)
	{
		WeaponComp->SetWeaponType(NewWeaponType);
		WeaponComp->RegisterComponent();
		WeaponComp->isWeaponEquipped=false;
		// ��������Ʈ�� �˸�
		if (InputBindingDeleagate.IsBound())
		{
			InputBindingDeleagate.Broadcast(Cast<UEnhancedInputComponent>(InputComponent));
		}
	}
}

void AHunter::NetLog()
{
	const FString conStr = GetNetConnection() != nullptr ? TEXT("Valid Connection") : TEXT("Invalid Connection");
	const FString ownerName = GetOwner() != nullptr ? GetOwner()->GetName() : TEXT("No Owner");

	const FString logStr = FString::Printf(TEXT("Connection : %s \nOwner Name : %s \nLocalRole : %s \nRemote Role : %s"), *conStr, *ownerName, *LOCAL_ROLE, *REMOTE_ROLE);

	DrawDebugString(GetWorld(), GetActorLocation() + FVector::UpVector * 100.0f, logStr, nullptr, FColor::White, 0, true);
}

void AHunter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AHunter, isRun);
	DOREPLIFETIME(AHunter, isHit);

}
//대쉬 함수
void AHunter::ServerRPC_Dash_Implementation()
{
	MulticastRPC_Dash();
}

void AHunter::MulticastRPC_Dash_Implementation()
{
	WeaponComp->Dash();
	WeaponComp->ModifyWeaponMoveSpeed();
}

void AHunter::ServerRPC_DashEnd_Implementation()
{
	MulticastRPC_DashEnd();
}

void AHunter::MulticastRPC_DashEnd_Implementation()
{
	WeaponComp->DashEnd();
	WeaponComp->ModifyWeaponMoveSpeed();
}

//약공격 함수
void AHunter::ServerRPC_QuickStart_Implementation()
{
	WeaponComp->QuickInputStart();
}

void AHunter::ServerRPC_QuickHolding_Implementation()
{
	WeaponComp->QuickInputHolding();

}

void AHunter::ServerRPC_QuickEnd_Implementation()
{
	WeaponComp->QuickInputEnd();
}

void AHunter::ServerRPC_QuickAttack_Implementation()
{
	MulticasrRPC_QuickAttack();
}

void AHunter::MulticasrRPC_QuickAttack_Implementation()
{
	WeaponComp->QuickAttack();
}

void AHunter::ServerRPC_HeavyStart_Implementation()
{
	WeaponComp->HeavyInputStart();
}

void AHunter::ServerRPC_HeavyHolding_Implementation()
{
	WeaponComp->HeavyInputHolding();
}

void AHunter::ServerRPC_HeavyEnd_Implementation()
{
	WeaponComp->HeavyInputEnd();
}

//강공격 함수
void AHunter::ServerRPC_HeavyAttack_Implementation()
{
	MulticasrRPC_HeavyAttack();
}

void AHunter::MulticasrRPC_HeavyAttack_Implementation()
{
	WeaponComp->HeavyAttack();
}
//특수공격 함수
void AHunter::ServerRPC_UniqueAttack_Implementation()
{
	MulticasrRPC_UniqueAttack();
}

void AHunter::MulticasrRPC_UniqueAttack_Implementation()
{
	WeaponComp->UniqueAttack();
}

void AHunter::ServerRPC_ChargeAttack_Implementation()
{
	MulticasrRPC_ChargeAttack();
}

void AHunter::MulticasrRPC_ChargeAttack_Implementation()
{
	WeaponComp->ChargeAttack();
}

//void AHunter::ServerRPC_CommandInputReset_Implementation() { WeaponComp->IsCommandInputReset(); }

//void AHunter::MulticastRPC_CommandInputReset_Implementation(){WeaponComp->IsCommandInputReset();}

void AHunter::ServerRPC_SetIsAttacking_Implementation(bool IsAttack) { WeaponComp->SetIsAttacking(IsAttack); }


void AHunter::ServerRPC_SetIsQuickAttack_Implementation(bool IsQuick) { WeaponComp->SetIsQuickAttack(IsQuick); }

void AHunter::ServerRPC_SetIsHeavyAttack_Implementation(bool IsHeavy) { WeaponComp->SetIsHeavyAttack(IsHeavy); }

void AHunter::ServerRPC_SetIsUniqueAttack_Implementation(bool IsUnique) { WeaponComp->SetIsUniqueAttack(IsUnique); }

void AHunter::ServerRPC_SetIsTacle_Implementation(bool IsTacle) { WeaponComp->SetIsTacle(IsTacle); }

void AHunter::ServerRPC_SetIsHolding_Implementation(bool IsHolding) { WeaponComp->SetIsHolding(IsHolding); }

void AHunter::ServerRPC_SetIsJumpDelay_Implementation(bool IsJumpDelay){WeaponComp->SetIsJumpDelay(IsJumpDelay);}

void AHunter::ServerRPC_SetAllowRoll_Implementation(bool AllowRoll) { WeaponComp->SetAllowRoll(AllowRoll); }

void AHunter::ServerRPC_AttachWeaponToHand_Implementation()
{
	MulticastRPC_AttachWeaponToHand();
}

void AHunter::MulticastRPC_AttachWeaponToHand_Implementation()
{
	WeaponComp->AttachWeaponToHand();
}

void AHunter::ServerRPC_AttachWeaponToOwner_Implementation()
{
	MulticastRPC_AttachWeaponToOwner();
}

void AHunter::MulticastRPC_AttachWeaponToOwner_Implementation()
{
	WeaponComp->AttachWeaponToOwner();
}

void AHunter::ServerRPC_SetQuickAddIndex_Implementation(int32 AddIndex)
{
	MulticastRPC_SetQuickAddIndex(AddIndex);
}

void AHunter::MulticastRPC_SetQuickAddIndex_Implementation(int32 AddIndex)
{
	WeaponComp->SetQuickStrikeComboIndex(AddIndex);

}

void AHunter::ServerPRC_SetHeavyAddIndex_Implementation(int32 AddIndex)
{
	MulticastRPC_SetHeavyAddIndex(AddIndex);
}

void AHunter::MulticastRPC_SetHeavyAddIndex_Implementation(int32 AddIndex)
{
	WeaponComp->SetHeavyStrikeComboIndex(AddIndex);

}
void AHunter::ServerPRC_SetUniqueAddIndex_Implementation(int32 AddIndex)
{
	WeaponComp->SetUniqueStrikeComboIndex(AddIndex);
}

void AHunter::ServerRPC_QuickStrikeNext_Implementation()
{
	MulticastRPC_QuickStrikeNext();
}

void AHunter::MulticastRPC_QuickStrikeNext_Implementation()
{
	WeaponComp->QuickStrikeNext();
}

void AHunter::ServerRPC_HeavyStrikeNext_Implementation()
{
	MulticastRPC_HeavyStrikeNext();
}

void AHunter::MulticastRPC_HeavyStrikeNext_Implementation()
{
	WeaponComp->HeavyStrikeNext();
}

void AHunter::ServerRPC_ResetCombo_Implementation()
{
	NetMulticastRPC_ResetCombo();
}

void AHunter::NetMulticastRPC_ResetCombo_Implementation()
{
	WeaponComp->ResetCombo();

}

void AHunter::ServerRPC_Roll_Implementation()
{
	NetMulticastRPC_Roll();
}

void AHunter::NetMulticastRPC_Roll_Implementation()
{
	WeaponComp->Roll();
}

void AHunter::ServerRPC_JumpToNextCombo_Implementation()
{
	NetMulticastRPC_JumpToNextCombo();
}

void AHunter::NetMulticastRPC_JumpToNextCombo_Implementation()
{
	WeaponComp->JumpToNextCombo();
}

void AHunter::ServerRPC_HitEvent_Implementation()
{
	MoveComp->MoveState = EMoveState::HIT;
	isHit = true;
	MoveComp->InputOff();
	GetCapsuleComponent()->SetCollisionProfileName(FName("Pawn"));

	MoveComp->KnockBack();
	WeaponComp->ResetCombo();
	FTimerHandle Handler;
	auto OnInput = [this]()
		{
			MoveComp->InputOn(); MoveComp->MoveState = EMoveState::IDLE;
			isHit = false;
			GetCapsuleComponent()->SetCollisionProfileName(FName("Pawn2"));
		};
	GetWorld()->GetTimerManager().SetTimer(Handler, OnInput, 2.3, false);
	ServerRPC_SetAllowRoll(true);
	ServerRPC_SetIsAttacking(false);
	ServerRPC_SetIsJumpDelay(false);
}

