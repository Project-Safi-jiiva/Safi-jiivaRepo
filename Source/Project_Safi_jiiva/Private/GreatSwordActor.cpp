// Fill out your copyright notice in the Description page of Project Settings.


#include "GreatSwordActor.h"
#include "UObject/ConstructorHelpers.h"
#include "AssetPath.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SafiJiiva/CSafiJiiva.h"
#include "Weapon/WeaponComponent.h"
#include "Project_Safi_jiiva.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "Sound/SoundCue.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"

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
		SetRootComponent(SwordMesh);
	}
	ConstructorHelpers::FObjectFinder<UParticleSystem>Particle(AssetPaths::GREATSWORDMESHPARTICLE);
    ConstructorHelpers::FObjectFinder<USoundCue>Sound(AssetPaths::HIT_SOUND);
    HitSound = Sound.Object;

	ParticleSystem = Particle.Object;
	bReplicates = true;
    SetReplicates(true);
	SetReplicateMovement(true);

}

void AGreatSwordActor::BeginPlay()
{
	Super::BeginPlay();
	SwordMesh->SetCollisionProfileName(FName("Attack"));
	SwordMesh->OnComponentBeginOverlap.AddDynamic(this, &AGreatSwordActor::OnBeginOverlap);
	Hunter = Cast<AHunter>(GetOwner());

}

// Called every frame
void AGreatSwordActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGreatSwordActor::SetOwnerComponent_Implementation(UActorComponent* Component)
{
	OwnerWeaponComp = Cast<UWeaponComponent>(Component);
}

void AGreatSwordActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
        ACSafiJiiva* Ch = Cast<ACSafiJiiva>(OtherActor);
        if (HitPawn.Num() <= 0)
        {
            ShowEffectClient(OtherActor,OtherComp, bFromSweep, SweepResult, OverlappedComp);
            Damage = OwnerWeaponComp->SetDamage();
            UGameplayStatics::ApplyDamage(OtherActor, Damage, nullptr, this, nullptr);
        }
        HitPawn.AddUnique(Ch);
}

void AGreatSwordActor::ShowEffectClient_Implementation(AActor* OtherActor,UPrimitiveComponent* OtherComp, bool bFromSweep, const FHitResult& SweepResult, UPrimitiveComponent* OverlappedComp)
{
    FTransform ParticleTransform;
    UBoxComponent* BoxComp = Cast<UBoxComponent>(OtherComp);

    if (bFromSweep && BoxComp)
    {
        // Sweep이 발생한 경우: 정확한 충돌 지점 사용
        ParticleTransform.SetLocation(SweepResult.ImpactPoint);
        ParticleTransform.SetRotation(SweepResult.ImpactNormal.Rotation().Quaternion()); // 충돌 표면의 법선 방향으로 회전
        ParticleTransform.SetScale3D(FVector(1.0f));
    }
    else if (BoxComp)
    {
        // Sweep이 없는 경우: BoxComponent 표면에 가까운 위치 추정
        FVector BoxCenter = BoxComp->GetComponentLocation();
        FVector BoxExtent = BoxComp->GetScaledBoxExtent(); // 스케일이 적용된 Extent 가져오기

        // OverlappedComp의 위치를 기준으로 가장 가까운 표면 지점 계산
        FVector OverlapLocation = OverlappedComp->GetComponentLocation();
        FVector Direction = (OverlapLocation - BoxCenter).GetSafeNormal();
        FVector SurfacePoint = BoxCenter + Direction * BoxExtent;

        ParticleTransform.SetLocation(SurfacePoint);
        ParticleTransform.SetRotation(Direction.Rotation().Quaternion()); // 표면 방향으로 회전
        ParticleTransform.SetScale3D(FVector(1.0f));
        UGameplayStatics::PlaySoundAtLocation(this, HitSound, SurfacePoint);
        ApplyHitStop();
    }
    else
    {
        // BoxComponent가 아닌 경우: OtherActor의 위치 사용
        ParticleTransform.SetLocation(OtherActor->GetActorLocation());
        ParticleTransform.SetScale3D(FVector(1.0f));
    }

    // 파티클 스폰
    UParticleSystemComponent* ParticleComp = UGameplayStatics::SpawnEmitterAtLocation(
        GetWorld(),
        ParticleSystem,
        ParticleTransform
    );

    if (ParticleComp)
    {
        // 크기 조절
        ParticleComp->SetWorldScale3D(FVector(1.5f, 1.5f, 1.5f)); // 1.5배 크기로 설정
    }

}

void AGreatSwordActor::ApplyHitStop()
{
    CustomTimeDilation = 0.1f;
    if (Hunter->IsLocallyControlled())
    {
        // 로컬 클라이언트에서만 시간 멈춤 적용
        // 캐릭터와 카메라에 CustomTimeDilation 설정
        CustomTimeDilation = 0.1f;
        if (Hunter->CameraComponent)
        {
           Hunter->CustomTimeDilation = 0.1f;
        }
        if (Hunter->GetMesh())
        {
            Hunter->CustomTimeDilation = 0.1f;
        }

        // 사운드 재생 (느린 피치로)
        if (HitSound)
        {
            UGameplayStatics::PlaySound2D(GetWorld(), HitSound, 1.0f, 0.5f); // 피치 0.5로 느리게
        }

        // 타이머로 일정 시간 후 복구
        GetWorld()->GetTimerManager().SetTimer(
            HitStopTimerHandle,
            this,
            &AGreatSwordActor::ResetHitStop,
            Hunter->DelayTime,
            false
        );
    }
}

void AGreatSwordActor::ResetHitStop()
{
    if (Hunter->IsLocallyControlled())
    {
        // 시간 속도 복구
        CustomTimeDilation = 1.0f;
        if (Hunter->CameraComponent)
        {
            Hunter->CustomTimeDilation = 1.0f;
        }
        if (Hunter->GetMesh())
        {
            Hunter->CustomTimeDilation = 1.0f;
        }
    }
}

void AGreatSwordActor::ApplyDamage_Implementation(AActor* HitActor, float DamageMultiplier)
{

}

void AGreatSwordActor::SetBaseDamage_Implementation(float NewDamage)
{
	Damage = NewDamage;
}

float AGreatSwordActor::GetBaseDamage_Implementation() const
{
	return 0;
}
