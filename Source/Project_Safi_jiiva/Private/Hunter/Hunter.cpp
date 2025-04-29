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
#include "Hunter/HunterController.h"
#include "Widget/HunterMainWidget.h"
#include "Widget/QuestBoardWidget.h"

void AHunter::SetHP(float value)
{
	hp = value;
	OnRep_HP();

}

float AHunter::GetHP()
{
	return hp;
}

void AHunter::OnRep_HP()
{
		PRINTLOG_NET(TEXT("HP : %f"), HP);
		if (IsLocallyControlled()) {
			PRINTLOG_NET(TEXT("HP : %f"), HP);
			float v = FMath::Clamp(hp / 200, 0.0f, 1.0f);
			if (MainWidget)
				MainWidget->uiHp = v;
			else
				PRINTLOG_NET(TEXT("mainwidget null!"));
		}
}

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
	static ConstructorHelpers::FClassFinder<UHunterMainWidget> WidgetClassFinder(AssetPaths::HUNTER_MAINWIDGET);
	MainWidgetClass = WidgetClassFinder.Class;

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
	AHunterController* PC = Cast<AHunterController>(GetController());
	if(PC){
	UEnhancedInputLocalPlayerSubsystem* subSys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());

	if (subSys)
		subSys->AddMappingContext(IMC_Hunter, 0);
	}
	if (IsLocallyControlled() && HasAuthority() == false)
	{
		// UI 위젯 초기화
		InitUIWidget();
	}
}

void AHunter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	NetLog();

	if (Anim&& Anim->Montage_IsPlaying(nullptr)&&isHit)
		Anim->Montage_Stop(0.1f);
	SetStamina(0);


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
	UPrimitiveComponent* CauserComponent = Cast<UPrimitiveComponent>(DamageCauser);
	if (!CauserComponent)
	{
		// DamageCauser가 액터인 경우, 콜리전 컴포넌트를 찾음
		CauserComponent = DamageCauser->FindComponentByClass<UPrimitiveComponent>();
		PRINTLOG_NET(TEXT("SERVER?"));
		HitEvent(CauserComponent, Damage);
	}



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
	DOREPLIFETIME(AHunter, hp);
	DOREPLIFETIME(AHunter, Stamina);
	DOREPLIFETIME(AHunter, StaminaDelay);

}

void AHunter::SetStamina(float StaminaCost)
{
	if (StaminaDelay)return;
	if(StaminaCost<=0){
		if (isRun) {
			if (GetVelocity().Size() <= 0)return;
			if (Stamina >= 0) {
				Stamina -= 2;
			}

		}
		else {
			if (Stamina <= 200) {
				Stamina += 2;
			}
			else {
				Stamina = 200;
			}
		}
	}
	else {
		Stamina -= StaminaCost;
	}
	if (Stamina <= 0) {
		StaminaDelay = true;
		FTimerHandle handler;
		GetWorld()->GetTimerManager().SetTimer(handler, [&]() {StaminaDelay = false; }, 1, false);
	}

	float v = FMath::Clamp(Stamina / 200, 0.0f, 1.0f);
	if (MainWidget)
		MainWidget->uiSp = v;
}

float AHunter::GetStamina()
{
	return Stamina;
}

//대쉬 함수
void AHunter::ServerRPC_Dash_Implementation()
{
	MulticastRPC_Dash();

}

void AHunter::MulticastRPC_Dash_Implementation()
{
	if(WeaponComp){
	WeaponComp->ModifyWeaponMoveSpeed();
	WeaponComp->Dash();
	}
}

void AHunter::ServerRPC_DashEnd_Implementation()
{
	MulticastRPC_DashEnd();


}

void AHunter::MulticastRPC_DashEnd_Implementation()
{
	if (!WeaponComp) return;

	WeaponComp->ModifyWeaponMoveSpeed();
	WeaponComp->DashEnd();
}

//약공격 함수
void AHunter::ServerRPC_QuickStart_Implementation()
{
	if (!WeaponComp) return;

	WeaponComp->QuickInputStart();
}

void AHunter::ServerRPC_QuickHolding_Implementation()
{
	if (!WeaponComp) return;

	WeaponComp->QuickInputHolding();

}

void AHunter::ServerRPC_QuickEnd_Implementation()
{
	if (!WeaponComp) return;

	WeaponComp->QuickInputEnd();
}

void AHunter::ServerRPC_QuickAttack_Implementation()
{
	MulticasrRPC_QuickAttack();
}

void AHunter::MulticasrRPC_QuickAttack_Implementation()
{
	if (!WeaponComp) return;

	WeaponComp->QuickAttack();
}

void AHunter::ServerRPC_HeavyStart_Implementation()
{
	if (!WeaponComp) return;

	WeaponComp->HeavyInputStart();
}

void AHunter::ServerRPC_HeavyHolding_Implementation()
{
	if (!WeaponComp) return;

	WeaponComp->HeavyInputHolding();
}

void AHunter::ServerRPC_HeavyEnd_Implementation()
{
	if (!WeaponComp) return;

	WeaponComp->HeavyInputEnd();
}

//강공격 함수
void AHunter::ServerRPC_HeavyAttack_Implementation()
{
	if (!WeaponComp) return;

	MulticasrRPC_HeavyAttack(WeaponComp->GetCurrentWeaponData());
}

void AHunter::MulticasrRPC_HeavyAttack_Implementation(const struct FWeaponDataTable& CurrentData)
{
	if (!WeaponComp) return;

	WeaponComp->HeavyAttack(CurrentData);
}
//특수공격 함수
void AHunter::ServerRPC_UniqueAttack_Implementation()
{
	MulticasrRPC_UniqueAttack();
}

void AHunter::MulticasrRPC_UniqueAttack_Implementation()
{
	if (!WeaponComp) return;

	WeaponComp->UniqueAttack();
}

void AHunter::ServerRPC_ChargeAttack_Implementation()
{
	if (!WeaponComp) return;

	MulticasrRPC_ChargeAttack(WeaponComp->GetCurrentWeaponData());

}

void AHunter::MulticasrRPC_ChargeAttack_Implementation(const struct FWeaponDataTable& CurrentData)
{
	if (!WeaponComp) return;

	WeaponComp->ChargeAttack(CurrentData);
}


//void AHunter::ServerRPC_CommandInputReset_Implementation() { WeaponComp->IsCommandInputReset(); }

//void AHunter::MulticastRPC_CommandInputReset_Implementation(){WeaponComp->IsCommandInputReset();}

void AHunter::ServerRPC_SetIsAttacking_Implementation(bool IsAttack) { MulticasrRPC_SetIsAttacking(IsAttack); }


void AHunter::MulticasrRPC_SetIsAttacking_Implementation(bool IsAttack)
{
	if (!WeaponComp) return;

	WeaponComp->SetIsAttacking(IsAttack);
}

void AHunter::ServerRPC_SetIsQuickAttack_Implementation(bool IsQuick) { WeaponComp->SetIsQuickAttack(IsQuick); }

void AHunter::ServerRPC_SetIsHeavyAttack_Implementation(bool IsHeavy) { WeaponComp->SetIsHeavyAttack(IsHeavy); }

void AHunter::ServerRPC_SetIsUniqueAttack_Implementation(bool IsUnique) { WeaponComp->SetIsUniqueAttack(IsUnique); }

void AHunter::ServerRPC_SetIsTacle_Implementation(bool IsTacle) { WeaponComp->SetIsTacle(IsTacle); }

void AHunter::ServerRPC_SetIsHolding_Implementation(bool IsHolding) { WeaponComp->SetIsHolding(IsHolding); }

void AHunter::ServerRPC_SetIsJumpDelay_Implementation(bool IsJumpDelay){WeaponComp->SetIsJumpDelay(IsJumpDelay);}

void AHunter::ServerRPC_SetAllowRoll_Implementation(bool AllowRoll) { WeaponComp->SetAllowRoll(AllowRoll); }
//무기 손에 붙이기
void AHunter::ServerRPC_AttachWeaponToHand_Implementation()
{

		MulticastRPC_AttachWeaponToHand();
}

void AHunter::MulticastRPC_AttachWeaponToHand_Implementation()
{
	WeaponComp->AttachWeaponToHand();
}
//무기 등에 붙이기
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
	if (WeaponComp-> IsQuickAttack) return;
	if (WeaponComp->FCommandInput[1] <= 0)return;
	MulticastRPC_HeavyStrikeNext(WeaponComp->GetCurrentWeaponData());
}

void AHunter::MulticastRPC_HeavyStrikeNext_Implementation(const struct FWeaponDataTable& CurrentData)
{
	WeaponComp->HeavyStrikeNext(CurrentData);
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
	if (WeaponComp->isCommandInput[0])return;
	NetMulticastRPC_JumpToNextCombo();

}

void AHunter::NetMulticastRPC_JumpToNextCombo_Implementation()
{
	WeaponComp->JumpToNextCombo();
}

void AHunter::HitEvent(UPrimitiveComponent* DamageCauserComponent, float Damage)
{
	HP -= Damage;

	MoveComp->MoveState = EMoveState::HIT;
	isHit = true;
	MoveComp->InputOff();
	GetCapsuleComponent()->SetCollisionProfileName(FName("Pawn"));

	MoveComp->KnockBack(DamageCauserComponent);
	WeaponComp->ResetCombo();
	FTimerHandle Handler;
	auto OnInput = [this]()
		{
			if (HP <= 0) {
				MoveComp->MoveState = EMoveState::DIE;
				AHunterController* PC = Cast<AHunterController>(GetController());
				if (IsLocallyControlled()) {
					PC->ServerRPC_RespawnPlayer();
					WeaponComp->DestroyEquippedWeapon();
				}
				return;
			}
			MoveComp->InputOn(); MoveComp->MoveState = EMoveState::IDLE;
			isHit = false;
			GetCapsuleComponent()->SetCollisionProfileName(FName("Pawn2"));
			if (!IsLocallyControlled())return;

			ServerRPC_SetIsJumpDelay(false);
			ServerRPC_SetIsQuickAttack(false);
			ServerRPC_SetIsHeavyAttack(false);
			ServerRPC_SetIsUniqueAttack(false);
			ServerRPC_SetIsTacle(false);
			ServerRPC_SetAllowRoll(true);
			ServerRPC_SetIsAttacking(false);
			StaminaDelay = false;
		};
	GetWorld()->GetTimerManager().SetTimer(Handler, OnInput, 2.3, false);
}

void AHunter::ServerRPC_OnWeaponComp_Implementation() {
	WeaponComp->WeaponCollitionOn();
}
void AHunter::ServerRPC_OffWeaponComp_Implementation() {
	WeaponComp->WeaponCollitionOff();
}

void AHunter::ServerRPC_CancelHandler_Implementation()
{
	NetMulticastRPC_CancelHandler(WeaponComp->GetCurrentWeaponData());
}

void AHunter::NetMulticastRPC_CancelHandler_Implementation(const struct FWeaponDataTable& CurrentData)
{
	WeaponComp->CancelHandler(CurrentData);
}

void AHunter::PossessedBy(AController* NewController)
{
	PRINTLOG_NET(TEXT("Begin"));

	Super::PossessedBy(NewController);

	if (IsLocallyControlled())
	{
		InitUIWidget();
		hp = MaxHP;
		OnRep_HP();
	}
	PRINTLOG_NET(TEXT("End"));

}

void AHunter::InitUIWidget()
{
	FString LevelName = GetWorld()->GetMapName();
	auto PC = Cast<AHunterController>(Controller);
	PC->bShowMouseCursor = false;
	if (LevelName == TEXT("LobbyMap")) {
		PC->BoardWidget=Cast<UQuestBoardWidget>(CreateWidget(GetWorld(), PC->QuestBoardWidget));
		QuestBoardWidget = PC->BoardWidget;
		QuestBoardWidget->AddToViewport();
		return;
	}
	PC->SetInputMode(FInputModeGameOnly());
	if (PC == nullptr)
	{
		PRINTLOG_NET(TEXT("PlayerController is null"));
		return;
	}

	if (PC->mainUIWidget == nullptr)
	{
		PRINTLOG_NET(TEXT("mainUIWidget is null"));
		// mainUIWidget이 null이면 클래스 로드 시도
		static ConstructorHelpers::FClassFinder<UHunterMainWidget> WidgetClassFinder(AssetPaths::HUNTER_MAINWIDGET);
		if (WidgetClassFinder.Succeeded())
		{
			PC->mainUIWidget = WidgetClassFinder.Class;
		}
		else
		{
			PRINTLOG_NET(TEXT("Failed to load mainUIWidget class"));
			return;
		}
	}

	if (PC->mainUI == nullptr)
	{
		PRINTLOG_NET(TEXT("Creating new mainUI"));
		PC->mainUI = Cast<UHunterMainWidget>(CreateWidget(GetWorld(), PC->mainUIWidget));
		if (PC->mainUI == nullptr)
		{
			PRINTLOG_NET(TEXT("Failed to create mainUI"));
			return;
		}
	}

	MainWidget = PC->mainUI;
	if (MainWidget)
	{
		PRINTLOG_NET(TEXT("MainWidget initialized successfully"));
		MainWidget->AddToViewport();
		hp = MaxHP;
		SetStatus();
	}
	else
	{
		PRINTLOG_NET(TEXT("MainWidget is null after initialization"));
	}
}

void AHunter::SetStatus()
{
	float h = FMath::Clamp(MaxHP / 200, 0.0f, 1.0f);
	float s = FMath::Clamp(MaxStamina / 200, 0.0f, 1.0f);
	MainWidget->uiHp = 1.0f;
	MainWidget->uiSp = 1.0f;
}

