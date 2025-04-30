// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSafiSpecialActor2.generated.h"

UCLASS()
class ACSafiSpecialActor2 : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACSafiSpecialActor2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:

	UPROPERTY(VisibleAnywhere)
    UParticleSystemComponent* ParticleComponent;

	class ACSafiJiiva* me;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float curretTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Particle)
	float DestroyDelay = 3.5f;

	void SetbOnSpawn();
	void ReturnToBase();

	float currentTime = 0.f;

};
