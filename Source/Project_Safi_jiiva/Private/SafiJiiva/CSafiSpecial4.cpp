// Fill out your copyright notice in the Description page of Project Settings.


#include "SafiJiiva/CSafiSpecial4.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"

// Sets default values
ACSafiSpecial4::ACSafiSpecial4()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComponent"));
	RootComponent = ParticleComponent;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("/Game/KJY/AdvancedMagicFX05/Particles/P_AMFX05_auraPillar3.P_AMFX05_auraPillar3"));

	if (ParticleAsset.Succeeded())
	{
		ParticleComponent->SetTemplate(ParticleAsset.Object);
	}

	ParticleComponent->bAutoActivate = false;
	ParticleComponent->bAutoDestroy = false; // 풀링에서는 자동 제거 비활성화

}

// Called when the game starts or when spawned
void ACSafiSpecial4::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACSafiSpecial4::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
		FVector CurrentScale = ParticleComponent->GetComponentScale();
		FVector TargetScale = FVector(30.0f); // 원하는 최종 크기
		float GrowSpeed = 0.6f;              // 초당 성장 속도

		// 선형 보간 (Lerp)
		FVector NewScale = FMath::VInterpTo(CurrentScale, TargetScale, DeltaTime, GrowSpeed);
		ParticleComponent->SetWorldScale3D(NewScale);


}

void ACSafiSpecial4::SetbOnSpawn()
{
	ParticleComponent->ActivateSystem(true);
	isBlow = true;
}

void ACSafiSpecial4::ReturnToBase()
{
	//UE_LOG(LogTemp, Error, TEXT("Why Not"));
	ParticleComponent->DeactivateSystem();
	SetActorLocation(FVector(0, 2000, -8000)); // 안 보이는 위치로
}
