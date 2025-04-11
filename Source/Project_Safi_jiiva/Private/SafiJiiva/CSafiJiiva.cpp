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

// 컴포넌트 생성 / 설정파트
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
	FireArrowComp->SetupAttachment(SafiComponent, TEXT("Socket_BiteDMGBox"));
	FireArrowComp->SetRelativeLocation(FVector());
	FireArrowComp->SetRelativeRotation(FRotator( 90.f , 0.f, 0.f ));

	LineArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("LineArrowComp"));
	LineArrowComp->SetupAttachment(SafiComponent /*, TEXT("Socket_Nose")*/);
	LineArrowComp->SetRelativeLocation(FVector(0.f, 1580.f, 350.f));
	LineArrowComp->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	

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

	Collision_1 = CreateDefaultSubobject<UBoxComponent>(TEXT("BiteDMGBox"));
	Collision_1->SetupAttachment(SafiComponent, TEXT("Socket_BiteDMGBox"));
	Collision_1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Collision_1->SetCollisionResponseToAllChannels(ECR_Block);


	Collision_2 = CreateDefaultSubobject<UBoxComponent>(TEXT("MeleeAttLBBox"));
	Collision_2->SetupAttachment(SafiComponent);
	Collision_2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Collision_2->SetCollisionResponseToAllChannels(ECR_Block);

	Collision_3 = CreateDefaultSubobject<UBoxComponent>(TEXT("MeleeAttLFBox"));
	Collision_3->SetupAttachment(SafiComponent);
	Collision_3->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Collision_3->SetCollisionResponseToAllChannels(ECR_Block);

	Collision_4 = CreateDefaultSubobject<UBoxComponent>(TEXT("MeleeAttRFBox"));
	Collision_4->SetupAttachment(SafiComponent);
	Collision_4->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Collision_4->SetCollisionResponseToAllChannels(ECR_Block);
#pragma  endregion Collision

	Collision_2->SetRelativeLocation(FVector(650.f, -350.f, 160.f));
	Collision_3->SetRelativeLocation(FVector(-650.f, 630.f, 160.f));
	Collision_4->SetRelativeLocation(FVector(-650.f, 630.f, 160.f));

	// ========================= 박스 크기 설정 파트


#pragma region SetExtentBox

	Collision_1->SetBoxExtent(FVector(70.f, 80.f, 150.f));
	Collision_2->SetBoxExtent(FVector(200.f));     
	Collision_3->SetBoxExtent(FVector(200.f));
	Collision_4->SetBoxExtent(FVector(200.f));

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

	DrawLineTrace();

	// DrawDebugSphere(GetWorld(), this->GetActorLocation(), MeleeAttRange, 12, FColor::Green, true, -1, 0, 0);


	// 콜리전 활성화, 비활성화 파트

	if (isOnAttBite == true) { Collision_1->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); }
	else { Collision_1->SetCollisionEnabled(ECollisionEnabled::NoCollision); }


	if (isOnSearch == true)
	{
		Collision_2->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Collision_3->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Collision_4->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	else
	{
		Collision_2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Collision_3->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Collision_4->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}


	// 원거리 공격범위 체크
	//DrawDebugSphere(GetWorld(), this->GetActorLocation(), SearchRange, 12, FColor::Blue, true, -1, 0, 0);
}

/*
void ACSafiJiiva::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
*/

void ACSafiJiiva::DrawLineTrace()
{
	FVector StartPos	= LineArrowComp->GetComponentLocation();
	FVector EndPos		=  StartPos + LineArrowComp->GetForwardVector() * 1000;

	bool bHit = CheckHitLineTrace(StartPos, EndPos);

	DrawDebugLine(GetWorld(), StartPos, EndPos, FColor::Red, false, -1, 0, 3.f);
}

bool ACSafiJiiva::CheckHitLineTrace(FVector _startPos, FVector& _curPos)
{
	FHitResult HitInfo;
	FCollisionQueryParams params;
	// TArray<AActor*> ignoreActor;
	params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitInfo, _startPos, _curPos, ECC_Visibility, params);

	if (bHit && HitInfo.GetActor()->GetActorNameOrLabel().Contains("헌터이려나"))
	{
		// 충돌체크

	}

	return bHit;
}

// 브레스, 
void ACSafiJiiva::SetNormal()
{

	isBreath = false;
	isOnAttBite= false;

	isRepelled = false;
	isOnAttBite = false;

	isDisturbed = false;

	isOnSearch = false;
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
	UE_LOG(LogTemp, Warning, TEXT("Bite_Test"));


	AHunter* target = Cast<AHunter>(OtherActor);
	if (!target) { return; }

	if (OverlappedComp == Collision_1)
	{
		//target->SetDamage(MeleeBiteDMG);
		UE_LOG(LogTemp,Warning,TEXT("Bite_Test"));
	}

	if (OverlappedComp == Collision_2)		// 왼쪽
	{
		attackPos = 2;
		UE_LOG(LogTemp, Warning, TEXT("Hit LF"));
	}

	if (OverlappedComp == Collision_3)		// 오른쪽
	{
		attackPos = 3;
		UE_LOG(LogTemp, Warning, TEXT("Hit RF"));
	}

	if (OverlappedComp == Collision_4)		// 오른쪽 뒤
	{
		attackPos = 4;
		UE_LOG(LogTemp, Warning, TEXT("Hit RB"));
	}


	/*
	if (target->태클상태)
	{
		this->OnDamageSafi(태클데미지)
	}
	*/

	// 데미지 처리는 FSM ,or AnimInstance 쪽에서 처리
	// Tick에서 스테이터스 체크해서 isDisturbed 체크
	// 스턴 / 경직 등이 들어왔다면 다른 bool형 변수들 false로 해주는 처리 필요
}

