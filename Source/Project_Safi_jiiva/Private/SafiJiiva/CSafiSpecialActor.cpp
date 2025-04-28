// Fill out your copyright notice in the Description page of Project Settings.


#include "SafiJiiva/CSafiSpecialActor.h"

// Sets default values
ACSafiSpecialActor::ACSafiSpecialActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACSafiSpecialActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACSafiSpecialActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bOnSpawn)
	{
		currentTime += DeltaTime;
		FVector P = GetActorLocation();
		
		speed += PlusSpeed;
		P.Z -= speed * DeltaTime;  // 또는 원하는 만큼 값을 설정
		this->SetActorLocation(P);
		if (currentTime > MaxTime)
		{
			ReturnToBase();
		}
	}

}

void ACSafiSpecialActor::SetbOnSpawn()
{
	bOnSpawn = true;
}

void ACSafiSpecialActor::ReturnToBase()
{
	speed = 800.f;
	bOnSpawn = false;
	SetActorLocation(FVector(8000.f));
}

