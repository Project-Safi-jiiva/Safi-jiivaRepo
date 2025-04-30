// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Hunter/Hunter.h"
#include "CSafiSpecialActor.generated.h"

UCLASS()
class PROJECT_SAFI_JIIVA_API ACSafiSpecialActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACSafiSpecialActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY()
	class ACSafiJiiva* me;

	float currentTime = 0.f;
	float defaultSpeed = 350.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
	float MaxTime = 1.9f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
	float speed = defaultSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
	float PlusSpeed = 40.f;

public:
	bool bOnSpawn = false;
	void KillingTime();
	void SetbOnSpawn();
	void ReturnToBase();

public:
	UPROPERTY()
	TArray<AHunter*>HunterList;

	void UpdateHunterList();
};
