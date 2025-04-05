// Fill out your copyright notice in the Description page of Project Settings.


#include "Hunter/BaseComponent.h"
#include "Hunter/Hunter.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"

// Sets default values for this component's properties
UBaseComponent::UBaseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;

	// ...
}


void UBaseComponent::InitializeComponent()
{
	Super::InitializeComponent();


	Owner = Cast<AHunter>(GetOwner());
	if (Owner)
		Owner->InputBindingDeleagate.AddUObject(this, &UBaseComponent::SetupInputBinding);

}

void UBaseComponent::BeginPlay()
{
	Super::BeginPlay();

}


void UBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UBaseComponent::SetupInputBinding(class UEnhancedInputComponent* InputComponent){}

