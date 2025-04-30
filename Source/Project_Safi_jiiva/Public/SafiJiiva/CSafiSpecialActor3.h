// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSafiSpecialActor3.generated.h"

UCLASS()
class PROJECT_SAFI_JIIVA_API ACSafiSpecialActor3 : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACSafiSpecialActor3();

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Particle)
	float DestroyDelay = 1.0f;

	void SetbOnSpawn();

	void ReturnToBase();

	float currentTime = 0.f;

	bool isBlow = false;


};
