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

}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	LoadWeaponData();
	SpawnWeaponActor();
}

void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	checkCommand(DeltaTime);
}

void UWeaponComponent::ResetCombo() {
	SetQuickStrikeComboIndex(0);
	SetHeavyStrikeComboIndex(0);
	SetUniqueStrikeComboIndex(0);
	bNextAttackQueued = false;
	IsAttacking = false;
	isJumpDelay = false;
	isHolding = false;
}

void UWeaponComponent::QuickInputStart() {
	isCommandInput[0] = false;
	FCommandInput[0] = 0;
}

void UWeaponComponent::QuickInputHolding()
{
	FCommandInput[0] += GetWorld()->DeltaTimeSeconds;
	//PRINT_LOG(TEXT("%f"),FCommandInput[0]);
	isCommandInput[0] = true;
}

void UWeaponComponent::QuickInputEnd() {
	isCommandInput[0] = false;
	FCommandInput[0] = 0;
}

void UWeaponComponent::HeavyInputStart(){
}

void UWeaponComponent::HeavyInputHolding() {
	FCommandInput[1] += GetWorld()->DeltaTimeSeconds;
	isCommandInput[1] = true;
}

void UWeaponComponent::HeavyInputEnd() {
	FCommandInput[1] = 0;
	isCommandInput[1] = false;
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

void UWeaponComponent::SetupInputBinding(class UEnhancedInputComponent* InputComponent)
{
	Super::SetupInputBinding(InputComponent);

	if (InputComponent)
	{

		// 입력 액션에 델리게이트 연결
		InputComponent->BindAction(IA_Dash, ETriggerEvent::Triggered, this, &UWeaponComponent::ServerRPC_Dash);
		InputComponent->BindAction(IA_Dash, ETriggerEvent::Completed, this, &UWeaponComponent::DashEnd);
		InputComponent->BindAction(IA_QuickStrike, ETriggerEvent::Started, this, &UWeaponComponent::QuickInputStart);
		InputComponent->BindAction(IA_QuickStrike, ETriggerEvent::Triggered, this, &UWeaponComponent::QuickInputHolding);
		InputComponent->BindAction(IA_QuickStrike, ETriggerEvent::Completed, this, &UWeaponComponent::QuickInputEnd);
		InputComponent->BindAction(IA_HeavyStrike, ETriggerEvent::Started, this, &UWeaponComponent::HeavyInputStart);
		InputComponent->BindAction(IA_HeavyStrike, ETriggerEvent::Triggered, this, &UWeaponComponent::HeavyInputHolding);
		InputComponent->BindAction(IA_HeavyStrike, ETriggerEvent::Completed, this, &UWeaponComponent::HeavyInputEnd);
		InputComponent->BindAction(IA_UniqueStrike, ETriggerEvent::Started, this, &UWeaponComponent::UniqueInputStart);
		InputComponent->BindAction(IA_UniqueStrike, ETriggerEvent::Triggered, this, &UWeaponComponent::UniqueInputHolding);
		InputComponent->BindAction(IA_UniqueStrike, ETriggerEvent::Completed, this, &UWeaponComponent::UniqueInputEnd);
		InputComponent->BindAction(IA_Roll, ETriggerEvent::Started, this, &UWeaponComponent::Roll);

		// 입력 액션에 델리게이트 연결
	}
}

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

		AttachWeaponToOwner();
	}
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

// 헬퍼 함수들
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

void UWeaponComponent::Roll(){

}

void UWeaponComponent::LoadWeaponData()
{
	if (!WeaponDataTable)return;
	WeaponDataMap.Empty();
	WeaponDataMap = WeaponDataTable->WeaponDataMap;
}

void UWeaponComponent::Dash()
{
	PRINTLOG_NET(TEXT("sadsadasdsad"));

}

void UWeaponComponent::DashEnd()
{
	if (isWeaponEquipped) return;
	Owner->isRun = false;
}

void UWeaponComponent::ServerRPC_Dash_Implementation()
{
	//Dash();
	PRINTLOG_NET(TEXT("UWeaponComponent ServerRPC_Dash_Implementation"));
	if (!Owner)return;
	if (isWeaponEquipped) {
		Owner->isRun = false;
		return;
	}
	Owner->isRun = true;

}

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

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(UWeaponComponent, isWeaponEquipped);
}
