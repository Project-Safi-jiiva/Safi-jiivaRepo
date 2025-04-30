// Fill out your copyright notice in the Description page of Project Settings.


#include "SafiJiiva/CSafiSpecialActor2.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SafiJiiva/CSafiJiiva.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ACSafiSpecialActor2::ACSafiSpecialActor2()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComponent"));
	RootComponent = ParticleComponent;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/KJY/AdvancedMagicFX05/Particles/P_AMFX05_auraPillar.P_AMFX05_auraPillar"));

	if (ParticleAsset.Succeeded())
	{
		ParticleComponent->SetTemplate(ParticleAsset.Object);
	}
	
	ParticleComponent->bAutoActivate = false;
	ParticleComponent->bAutoDestroy = false; // 풀링에서는 자동 제거 비활성화

}

// Called when the game starts or when spawned
void ACSafiSpecialActor2::BeginPlay()
{
	Super::BeginPlay();

	me = Cast<ACSafiJiiva>(UGameplayStatics::GetActorOfClass(GetWorld(), ACSafiJiiva::StaticClass()));
	

}

// Called every frame
void ACSafiSpecialActor2::Tick(float DeltaTime)
{
	if (currentTime > DestroyDelay)
	{
		ReturnToBase();
		currentTime = 0.f;
	}
}

void ACSafiSpecialActor2::SetbOnSpawn()
{
	ParticleComponent->ActivateSystem(true);
}

void ACSafiSpecialActor2::ReturnToBase()
{
	//UE_LOG(LogTemp, Error, TEXT("Why Not"));
	ParticleComponent->DeactivateSystem();
	SetActorLocation(FVector(0, 0, -10000)); // 안 보이는 위치로
}


