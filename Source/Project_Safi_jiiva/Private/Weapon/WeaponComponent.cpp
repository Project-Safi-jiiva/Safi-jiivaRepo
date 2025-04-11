// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponComponent.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputAction.h"
#include "AssetPath.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "Project_Safi_jiiva.h"
#include "Hunter/Hunter.h"
#include "Weapon/IWeaponActor.h"

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
	ConstructorHelpers::FObjectFinder<UWeaponDataAsset> WeaponDataTableTool(AssetPaths::WeaponDataAsset);
	WeaponDataTable = WeaponDataTableTool.Object;

	WeaponType = EWeaponType::GREATSWORD;
	// ...
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

}

void UWeaponComponent::SetupInputBinding(class UEnhancedInputComponent* InputComponent)
{
	Super::SetupInputBinding(InputComponent);

	if (InputComponent)
	{
		InputComponent->BindAction(IA_Dash, ETriggerEvent::Triggered, this, &UWeaponComponent::Dash);
		InputComponent->BindAction(IA_Dash, ETriggerEvent::Completed, this, &UWeaponComponent::DashEnd);
		InputComponent->BindAction(IA_QuickStrike, ETriggerEvent::Started, this, &UWeaponComponent::QuickStrikeStart);
		InputComponent->BindAction(IA_QuickStrike, ETriggerEvent::Completed, this, &UWeaponComponent::QuickStrikeEnd);
		InputComponent->BindAction(IA_HeavyStrike, ETriggerEvent::Started, this, &UWeaponComponent::HeavyStrikeStart);
		InputComponent->BindAction(IA_HeavyStrike, ETriggerEvent::Completed, this, &UWeaponComponent::HeavyStrikeEnd);
		InputComponent->BindAction(IA_UniqueStrike, ETriggerEvent::Started, this, &UWeaponComponent::UniqueStrikeStart);
		InputComponent->BindAction(IA_UniqueStrike, ETriggerEvent::Completed, this, &UWeaponComponent::UniqueStrikeEnd);
	}
}

void UWeaponComponent::QuickStrikeStart(){}

void UWeaponComponent::QuickStrikeEnd(){}

void UWeaponComponent::HeavyStrikeStart(){}

void UWeaponComponent::HeavyStrikeEnd(){}

void UWeaponComponent::UniqueStrikeStart() {}

void UWeaponComponent::UniqueStrikeEnd(){}

void UWeaponComponent::ResetCombo() {
	SetQuickStrikeComboIndex(0);
	SetHeavyStrikeComboIndex(0);
	SetUniqueStrikeComboIndex(0);
	bNextAttackQueued = false;
}
void UWeaponComponent::SetWeaponType(EWeaponType NewWeaponType){WeaponType= NewWeaponType;}
void UWeaponComponent::Dash()
{
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

void UWeaponComponent::LoadWeaponData()
{
	if (!WeaponDataTable)return;
	WeaponDataMap.Empty();
	WeaponDataMap = WeaponDataTable->WeaponDataMap;
}

FWeaponDataTable UWeaponComponent::GetCurrentWeaponData() const
{
	const FWeaponDataTable* FoundData = WeaponDataMap.Find(WeaponType);
	if (FoundData) return *FoundData;
	return FWeaponDataTable();
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
		AttachWeaponToOwner(EquippedWeapon);
	}
}

// ÇïÆÛ ÇÔ¼öµé
void UWeaponComponent::DestroyEquippedWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;
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

void UWeaponComponent::InitializeWeaponActor(AActor* NewWeapon, const FWeaponDataTable& WeaponData)
{
	if (!NewWeapon) return;

	if (NewWeapon->Implements<UIWeaponActor>())
	{
		IIWeaponActor::Execute_SetBaseDamage(NewWeapon, WeaponData.BaseDamage);
	}
}

void UWeaponComponent::AttachWeaponToOwner(AActor* Weapon)
{
	if (Weapon && Owner)
	{
		Weapon->AttachToComponent(
			Owner->GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			TEXT("hand_rSocket")
		);
	}
}