// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon/IWeaponActor.h"
#include "SafiJiiva/CSafiJiiva.h"
#include "GreatSwordActor.generated.h"

UCLASS()
class PROJECT_SAFI_JIIVA_API AGreatSwordActor : public AActor , public IIWeaponActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGreatSwordActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual float GetBaseDamage_Implementation() const override;
	virtual void SetBaseDamage_Implementation(float NewDamage) override;
	virtual void ApplyDamage_Implementation(AActor* HitActor, float DamageMultiplier) override;
	virtual void SetOwnerComponent_Implementation(UActorComponent* Component) override;
	UFUNCTION()
    void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UPROPERTY()
	TArray<ACSafiJiiva*> HitPawn;
	UPROPERTY(VisibleAnywhere, Category = "Mesh")
	class UStaticMeshComponent* SwordMesh;

	UPROPERTY(BlueprintReadWrite, Category = "Particle")
    class UParticleSystem* ParticleSystem;

private:
	float Damage=0.0f;
	UPROPERTY()
	class UWeaponComponent* OwnerWeaponComp;
	UPROPERTY()
	class AHunter* Hunter;

	UFUNCTION(Client, Reliable)
	void ShowEffectClient(AActor* OtherActor,UPrimitiveComponent* OtherComp, bool bFromSweep, const FHitResult& SweepResult, UPrimitiveComponent* OverlappedComp);

	//사운드
	class USoundCue* HitSound;

	//타격감 함수
	void ApplyHitStop();
	void ResetHitStop();

	FTimerHandle HitStopTimerHandle;

};
