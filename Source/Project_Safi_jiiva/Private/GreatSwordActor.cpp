// Fill out your copyright notice in the Description page of Project Settings.


#include "GreatSwordActor.h"
#include "UObject/ConstructorHelpers.h"
#include "AssetPath.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AGreatSwordActor::AGreatSwordActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ConstructorHelpers::FObjectFinder<UStaticMesh> SwordMeshTool(AssetPaths::GREATSWORDMESH);
	SwordMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwordMesh"));
	if (SwordMeshTool.Succeeded())
	{
		SwordMesh->SetStaticMesh(SwordMeshTool.Object);
		SwordMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SwordMesh->SetRelativeScale3D(FVector(0.8));
	}
}

// Called when the game starts or when spawned
void AGreatSwordActor::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AGreatSwordActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGreatSwordActor::ApplyDamage_Implementation(AActor* HitActor, float DamageMultiplier)
{

}

void AGreatSwordActor::SetBaseDamage_Implementation(float NewDamage)
{

}

float AGreatSwordActor::GetBaseDamage_Implementation() const
{
	return 0;
}
