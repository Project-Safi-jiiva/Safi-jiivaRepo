// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
	float currentTime = 0.f;
	float defaultSpeed = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
	float MaxTime = 2.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
	float speed = defaultSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Status)
	float PlusSpeed = 80.f;

public:
	bool bOnSpawn = false;
	void SetbOnSpawn();
	void ReturnToBase();
};
