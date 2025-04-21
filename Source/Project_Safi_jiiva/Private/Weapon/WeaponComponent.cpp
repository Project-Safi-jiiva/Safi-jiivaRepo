// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponComponent.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputAction.h"
#include "AssetPath.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "Project_Safi_jiiva.h"
#include "Hunter/Hunter.h"
#include "Weapon/IWeaponActor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Hunter/HunterAnim.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent(){
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UInputAction> IA_DashTool(AssetPaths::IA_Dash);
	IA_Dash = IA_DashTool.Object;
	ConstructorHelpers::FObjectFinder<UInputAction> IA_QuickStrikeTool(AssetPaths::IA_QuickStrike);
	IA_QuickStrike = IA_QuickStrikeTool.Object;
	ConstructorHelpers::FObjectFinder<UInputAction> IA_HeavyStrikeTool(AssetPaths::IA_HeavyStrike);
	IA_HeavyStrike = IA_HeavyStrikeTool.Object;
	ConstructorHelpers::FObjectFinder<UInputAction> IA_UniqueStrikeTool(AssetPaths::IA_UniqueStrike);
	IA_UniqueStrike = IA_UniqueStrikeTool.Object;
	ConstructorHelpers::FObjectFinder<UInputAction> IA_RollTool(AssetPaths::IA_ROLL);
	IA_Roll = IA_RollTool.Object;
	ConstructorHelpers::FObjectFinder<UWeaponDataAsset> WeaponDataTableTool(AssetPaths::WeaponDataAsset);
	WeaponDataTable = WeaponDataTableTool.Object;
	SetIsReplicatedByDefault(true);

}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	LoadWeaponData();
	if(Owner&&Owner->HasAuthority())
		SpawnWeaponActor();
}

void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if(Owner&&Owner->HasAuthority()&&!IsAttacking)
		checkCommand(DeltaTime);
}
void UWeaponComponent::SetupInputBinding(class UEnhancedInputComponent* InputComponent)
{
	Super::SetupInputBinding(InputComponent);

	if (InputComponent)
	{
		// 입력 액션에 델리게이트 연결
		InputComponent->BindAction(IA_Dash, ETriggerEvent::Triggered, this, &UWeaponComponent::InputDash);
		InputComponent->BindAction(IA_Dash, ETriggerEvent::Completed, this, &UWeaponComponent::InputDashEnd);
		InputComponent->BindAction(IA_QuickStrike, ETriggerEvent::Started, this, &UWeaponComponent::InputQuickStart);
		InputComponent->BindAction(IA_QuickStrike, ETriggerEvent::Triggered, this, &UWeaponComponent::InputQuickHolding);
		InputComponent->BindAction(IA_QuickStrike, ETriggerEvent::Completed, this, &UWeaponComponent::InputQuickEnd);
		InputComponent->BindAction(IA_HeavyStrike, ETriggerEvent::Started, this, &UWeaponComponent::InputHeavyStart);
		InputComponent->BindAction(IA_HeavyStrike, ETriggerEvent::Triggered, this, &UWeaponComponent::InputHeavyHolding);
		InputComponent->BindAction(IA_HeavyStrike, ETriggerEvent::Completed, this, &UWeaponComponent::InputHeavyEnd);
		InputComponent->BindAction(IA_UniqueStrike, ETriggerEvent::Started, this, &UWeaponComponent::UniqueInputStart);
		InputComponent->BindAction(IA_UniqueStrike, ETriggerEvent::Triggered, this, &UWeaponComponent::UniqueInputHolding);
		InputComponent->BindAction(IA_UniqueStrike, ETriggerEvent::Completed, this, &UWeaponComponent::UniqueInputEnd);
		InputComponent->BindAction(IA_Roll, ETriggerEvent::Started, this, &UWeaponComponent::InputRoll);
	}
}
// <summary>
/// 인풋 함수 모음
/// </summary>
void UWeaponComponent::InputDash()
{
	Owner->ServerRPC_Dash();
}

void UWeaponComponent::InputDashEnd()
{
	Owner->ServerRPC_QuickStart();
}

void UWeaponComponent::InputQuickStart()
{
	Owner->ServerRPC_QuickStart();
}

void UWeaponComponent::InputQuickHolding()
{
	Owner->ServerRPC_QuickHolding();
}
void UWeaponComponent::InputQuickEnd()
{
	if (isJumpDelay) return;
	Owner->ServerRPC_QuickEnd();
}

void UWeaponComponent::InputHeavyStart()
{
	Owner->ServerRPC_HeavyStart();
}

void UWeaponComponent::InputHeavyHolding()
{
	Owner->ServerRPC_HeavyHolding();
}

void UWeaponComponent::InputHeavyEnd()
{
	Owner->ServerRPC_HeavyEnd();
}

void UWeaponComponent::InputUniqueStart()
{

}

void UWeaponComponent::InputUniqueHolding()
{

}

void UWeaponComponent::InputUniqueEnd()
{

}

void UWeaponComponent::InputRoll()
{
	if(Owner&&Owner->IsLocallyControlled()&&AllowRoll)
			Owner->ServerRPC_Roll();

}

void UWeaponComponent::LoadWeaponData()
{
	if (!WeaponDataTable)return;
	WeaponDataMap.Empty();
	WeaponDataMap = WeaponDataTable->WeaponDataMap;
}



/// <summary>
/// 기능 구현 함수 모음
/// </summary>

void UWeaponComponent::Dash()
{
	if (!Owner)return;
	if (isWeaponEquipped) {
		Owner->isRun = false;
		return;
	}
	Owner->isRun = true;
}

void UWeaponComponent::DashEnd()
{
	if (isWeaponEquipped) return;
	Owner->isRun = false;
}
void UWeaponComponent::QuickInputStart() {
	isCommandInput[0] = false;
	FCommandInput[0] = 0;
}

void UWeaponComponent::QuickInputHolding()
{
	FCommandInput[0] += GetWorld()->DeltaTimeSeconds;
	isCommandInput[0] = true;
}

void UWeaponComponent::QuickInputEnd() {
	isCommandInput[0] = false;
	FCommandInput[0] = 0;
}

void UWeaponComponent::HeavyInputStart() {
	FCommandInput[1] = 0;
	isCommandInput[1] = false;
}

void UWeaponComponent::HeavyInputHolding() {
	FCommandInput[1] += GetWorld()->DeltaTimeSeconds;
	isCommandInput[1] = true;
}

void UWeaponComponent::HeavyInputEnd() {
	FCommandInput[1] = 0;
	isCommandInput[1] = false;
}
/// <summary>
/// //////////////////////////////////////
/// </summary>

void UWeaponComponent::ResetCombo() {
	SetQuickStrikeComboIndex(0);
	SetHeavyStrikeComboIndex(0);
	SetUniqueStrikeComboIndex(0);
	bNextAttackQueued = false;
	IsAttacking = false;
	isJumpDelay = false;
	isHolding = false;
}



void UWeaponComponent::UniqueInputStart() {
}

void UWeaponComponent::UniqueInputHolding() {
	FCommandInput[2] += GetWorld()->DeltaTimeSeconds;
	isCommandInput[2] = true;
}

void UWeaponComponent::UniqueInputEnd(){
	FCommandInput[2] = 0;
	isCommandInput[2] = false;
}

void UWeaponComponent::checkCommand(float DeltaTime) {}

void UWeaponComponent::QuickStrikeNext(){}

void UWeaponComponent::HeavyStrikeNext(){}

void UWeaponComponent::JumpToNextCombo(){}

void UWeaponComponent::CancelHandler(){}


void UWeaponComponent::ModifyWeaponMoveSpeed()
{
	Super::ModifyWeaponMoveSpeed();
}

void UWeaponComponent::SpawnWeaponActor()
{
	FWeaponDataTable WeaponData = GetCurrentWeaponData();
	if (!WeaponData.WeaponActorClass) return;

	DestroyEquippedWeapon();

	Owner = Cast<AHunter>(GetOwner());
	if (!Owner) return;

	if (SpawnNewWeaponActor(WeaponData))
	{
		InitializeWeaponActor(EquippedWeapon, WeaponData);

		Owner->ServerRPC_AttachWeaponToOwner();	}
}

bool UWeaponComponent::SpawnNewWeaponActor(const FWeaponDataTable& WeaponData)
{
	UWorld* World = GetWorld();
	if (!World) return false;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = Owner;

	FVector SpawnLocation = Owner->GetActorLocation();
	FRotator SpawnRotation = Owner->GetActorRotation();

	EquippedWeapon = World->SpawnActor<AActor>(
		WeaponData.WeaponActorClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);

	return EquippedWeapon != nullptr;
}

void UWeaponComponent::DestroyEquippedWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;
	}
}

void UWeaponComponent::AttachWeaponToOwner()
{

	if (EquippedWeapon && Owner)
	{
		USkeletalMeshComponent* MeshComp = Owner->GetMesh();

		EquippedWeapon->AttachToComponent(
			MeshComp,
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			TEXT("Weapon")
		);
	}
}

void UWeaponComponent::AttachWeaponToHand()
{
	if (EquippedWeapon && Owner)
	{
		EquippedWeapon->AttachToComponent(
			Owner->GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			TEXT("Hand")
		);
	}
}

void UWeaponComponent::InitializeWeaponActor(AActor* NewWeapon, const FWeaponDataTable& WeaponData)
{
	if (!NewWeapon) return;
	if (NewWeapon->Implements<UIWeaponActor>())
	{
		IIWeaponActor::Execute_SetOwnerComponent(NewWeapon,this);
	}
}

void UWeaponComponent::Roll(){}

FWeaponDataTable UWeaponComponent::GetCurrentWeaponData() const
{
	const FWeaponDataTable* FoundData = WeaponDataMap.Find(WeaponType);
	if (FoundData) return *FoundData;
	return FWeaponDataTable();
}

void UWeaponComponent::IsCommandInputReset()
{
	for (int i = 0; i < isCommandInput.Num(); i++)
	{
		isCommandInput[i] = false;
	}
	CommandInputTime = 0.0f;
}

void UWeaponComponent::WeaponCollitionOn()
{

}

void UWeaponComponent::WeaponCollitionOff()
{

}

float UWeaponComponent::SetDamage()
{

	FWeaponDataTable WeaponData = GetCurrentWeaponData();
	//if (IsQuickAttack) {
	Damage = (WeaponData.BaseDamage)*(WeaponData.QuickStrikeDamageMultipliers[GetQuickStrikeComboIndex()]);
	if (IsHeavyAttack) {
		Damage =(WeaponData.BaseDamage) * (WeaponData.HeavyStrikeDamageMultipliers[GetHeavyStrikeComboIndex()]);
	}if (IsUniqueAttack) {
		Damage = (WeaponData.BaseDamage) * (WeaponData.HeavyStrikeDamageMultipliers[GetUniqueStrikeComboIndex()]);
	}
	return Damage;
}

void UWeaponComponent::OnRep_IsWeaponEquipped()
{
	PRINTLOG_NET(TEXT("test %d"), isWeaponEquipped);
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponComponent, isWeaponEquipped);
	DOREPLIFETIME(UWeaponComponent, CommandInputTime);
	DOREPLIFETIME(UWeaponComponent, WeaponType);
	DOREPLIFETIME(UWeaponComponent, FCommandInput);
	DOREPLIFETIME(UWeaponComponent, isCommandInput);
	DOREPLIFETIME(UWeaponComponent, QuickStrikeComboIndex);
	DOREPLIFETIME(UWeaponComponent, HeavyStrikeComboIndex);
	DOREPLIFETIME(UWeaponComponent, UniqueStrikeComboIndex);
	DOREPLIFETIME(UWeaponComponent, IsAttacking);
	DOREPLIFETIME(UWeaponComponent, isJumpDelay);
	DOREPLIFETIME(UWeaponComponent, isHolding);
	DOREPLIFETIME(UWeaponComponent, iscancel);
	DOREPLIFETIME(UWeaponComponent, IsQuickAttack);
	DOREPLIFETIME(UWeaponComponent, IsHeavyAttack);
	DOREPLIFETIME(UWeaponComponent, IsUniqueAttack);
	DOREPLIFETIME(UWeaponComponent, AllowRoll);
	DOREPLIFETIME(UWeaponComponent, isTacle);
}
