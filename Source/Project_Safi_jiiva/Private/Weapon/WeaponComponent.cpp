// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponComponent.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputAction.h"
#include "AssetPath.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "Project_Safi_jiiva.h"
#include "Hunter/Hunter.h"

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
// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	LoadWeaponData();
}
// Called every frame
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

void UWeaponComponent::QuickStrikeStart(){

	PRINT_LOG(TEXT("QuickStrike"));

}

void UWeaponComponent::QuickStrikeEnd()
{

}

void UWeaponComponent::HeavyStrikeStart(){
	PRINT_LOG(TEXT("HeavyStrike"));

}

void UWeaponComponent::HeavyStrikeEnd()
{

}

void UWeaponComponent::UniqueStrikeStart() {
	PRINT_LOG(TEXT("UniqueStrike"));

}

void UWeaponComponent::UniqueStrikeEnd()
{

}

void UWeaponComponent::ResetCombo() {
	SetQuickStrikeComboIndex(0);
	SetHeavyStrikeComboIndex(0);
	SetUniqueStrikeComboIndex(0);
	bNextAttackQueued = false;
}



void UWeaponComponent::SetWeaponType(EWeaponType NewWeaponType)
{
	WeaponType= NewWeaponType;
}

void UWeaponComponent::Dash()
{
	Owner->isRun = true;
	PRINT_LOG(TEXT("%d"), Owner->isRun);
}

void UWeaponComponent::DashEnd()
{
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
	if (FoundData)
	{
		return *FoundData;
	}

	UE_LOG(LogTemp, Warning, TEXT("No data found for CurrentWeaponType: %d"), (uint8)WeaponType);
	return FWeaponDataTable();
}


