// Fill out your copyright notice in the Description page of Project Settings.


#include "GreatSwordActor.h"
#include "UObject/ConstructorHelpers.h"
#include "AssetPath.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SafiJiiva/CSafiJiiva.h"
#include "Project_Safi_jiiva.h"

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
		SwordMesh->SetRelativeScale3D(FVector(0.7));
		//SwordMesh->SetRelativeLocationAndRotation(FVector(0, -20, -20), FRotator(20, 0, 0));
	}
}

// Called when the game starts or when spawned
void AGreatSwordActor::BeginPlay()
{
	Super::BeginPlay();
	SwordMesh->SetCollisionProfileName(FName("Attack"));
	SwordMesh->OnComponentBeginOverlap.AddDynamic(this, &AGreatSwordActor::OnBeginOverlap);
}

// Called every frame
void AGreatSwordActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGreatSwordActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACSafiJiiva* Ch = Cast<ACSafiJiiva>(OtherActor);
	if(HitPawn.Num()<=0){
		UGameplayStatics::ApplyDamage(OtherActor, 100, nullptr, this, nullptr);
	PRINT_LOG(TEXT("hit"));
	}
	HitPawn.AddUnique(Ch);
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
