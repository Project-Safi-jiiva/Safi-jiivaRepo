// Fill out your copyright notice in the Description page of Project Settings.
// 애가 월보를 한다... Location z -90 / 비행상태는 isFly로 체크


#include "SafiJiiva/CSafiJiiva.h"
#include "Engine/SkeletalMesh.h"
#include "Components/ArrowComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "SafiJiiva/CSafiFSM.h"
#include "Hunter/Hunter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ACSafiJiiva::ACSafiJiiva()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

#pragma region Components
	SafiComponent = GetMesh();
	ConstructorHelpers::FObjectFinder<USkeletalMesh>TmpBody(TEXT("/Script/Engine.SkeletalMesh'/Game/KJY/Safi_Jiiva/Meshes/Safi_jiiva__FULL_ANIMATION_.Safi_jiiva__FULL_ANIMATION_'"));

	if (TmpBody.Succeeded())
	{
		SafiComponent->SetSkeletalMesh(TmpBody.Object);
		SafiComponent->SetRelativeLocation(FVector(0.f, 0.f, -90.f));
		SafiComponent->SetRelativeRotation(FRotator( 0.f, -90.f, 0.f));

		SafiComponent->SetRelativeScale3D(FVector(0.45f));
	}
	
	FireArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("FireArrowComp"));
	FireArrowComp->SetupAttachment(SafiComponent/*, TEXT("FirePosition")*/);
	FireArrowComp->SetRelativeLocation(FVector());
	FireArrowComp->SetRelativeRotation(FRotator( 0.f , 90.f, 0.f ));


	FSM = CreateDefaultSubobject<UCSafiFSM>(TEXT("FSM"));
	USkeletalMeshComponent* SkeletalMeshComp = GetMesh();
	if (SkeletalMeshComp)
	{
		//SkeletalMeshComp->bEnablePerPolyCollision = true;
		SkeletalMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SkeletalMeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);

		SkeletalMeshComp->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	}

	
#pragma endregion Components

	//========================= 콜리전 세팅 파트

#pragma  region Collision
	Collision_1 = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision_1"));
	Collision_1->SetupAttachment(SafiComponent, TEXT("Collision_1"));
	Collision_1->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);


#pragma  endregion Collision


	// ========================= 박스 크기 설정 파트


#pragma region SetExtentBox

	Collision_1->SetBoxExtent(FVector(50.f, 50.f, 50.f));

#pragma endregion


}

// Called when the game starts or when spawned
void ACSafiJiiva::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACSafiJiiva::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 노티파이 제어시 생길 문제들 없애는 용도 :D...
	if (isDisturbed == true)
	{
		SetNormal();
	}


}

// Called to bind functionality to input
void ACSafiJiiva::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ACSafiJiiva::SetNormal()
{
	isBreath = false;
	isRepelled = false;

	// 제일 마지막에
	isDisturbed = false;
}


void ACSafiJiiva::SetSpeed(float _value)
{
	this->GetCharacterMovement()->MaxWalkSpeed = _value;
}

void ACSafiJiiva::OnDamageSafi(float _value)
{
	this->hp -= _value;
	if (hp <= 0)
	{
		hp = 0;
		//뭔가 사망처리 해주기
	}
}

void ACSafiJiiva::KillSafi_Test()
{
	this->OnDamageSafi(MAXHP);
}

void ACSafiJiiva::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AHunter* target = Cast<AHunter>(OtherActor);
	
	/*
	if (target) 
	{
		if(target->태클상태)
		{
			this->OnDamageSafi(태클데미지)
		}
	}
	*/

	// 데미지 처리는 FSM ,or AnimInstance 쪽에서 처리
	// Tick에서 스테이터스 체크해서 isDisturbed 체크
	// 스턴 / 경직 등이 들어왔다면 다른 bool형 변수들 false로 해주는 처리 필요
}

