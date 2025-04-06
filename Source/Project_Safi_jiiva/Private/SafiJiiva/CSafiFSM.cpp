// Fill out your copyright notice in the Description page of Project Settings.


#include "SafiJiiva/CSafiFSM.h"
#include "Kismet/GameplayStatics.h"
#include "Hunter/Hunter.h"

#include "SafiJiiva/CSafiAnimInstance.h"
#include "SafiJiiva/CSafiJiiva.h"

// Sets default values for this component's properties
UCSafiFSM::UCSafiFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCSafiFSM::BeginPlay()
{
	Super::BeginPlay();

	AActor* actor = UGameplayStatics::GetActorOfClass(GetWorld(), AHunter::StaticClass());
	if(!actor) { return; }
	target = Cast<AHunter>(actor);

	me = Cast<ACSafiJiiva>(GetOwner());
	if(!me) { return; }

	Anim = Cast<UCSafiAnimInstance>(me->GetMesh()->GetAnimInstance());
}


// Called every frame
void UCSafiFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#pragma region LogMessageState
	FString logMsgState = UEnum::GetValueAsString(mState);
	GEngine->AddOnScreenDebugMessage(0, 1, FColor::Red, logMsgState);

	FString logMsgAtt = UEnum::GetValueAsString(mAttState);
	GEngine->AddOnScreenDebugMessage(1, 1, FColor::Green, logMsgAtt);
#pragma endregion

	switch(mState)
	{
		case ESafiState::Idle		: { IdleState(); }	break;
	
	}
	

}

void UCSafiFSM::IdleState()
{

}

void UCSafiFSM::TargetRotation()
{
}

FVector UCSafiFSM::SearchTarget()
{
	return FVector();
}

