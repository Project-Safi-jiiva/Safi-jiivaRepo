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
#include "SafiJiiva/CSafiAnimInstance.h"

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

		//SafiComponent->SetRelativeScale3D(FVector(0.45f));

		SafiComponent->SetCollisionObjectType(ECC_GameTraceChannel1);
		
		// 훈타 몇채널인지 보기
		// SafiComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Overlap);
	}
	
	FireArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("FireArrowComp"));
	FireArrowComp->SetupAttachment(SafiComponent,TEXT("Socket_FirePos") /*TEXT("Socket_BiteDMGBox")*/);
	FireArrowComp->SetRelativeLocation(FVector(0.f, 75.f, 220.f));
	FireArrowComp->SetRelativeRotation(FRotator( 90.f , 0.f, 0.f ));

	LineArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("LineArrowComp"));
	LineArrowComp->SetupAttachment(SafiComponent /*, TEXT("Socket_Nose")*/);
	LineArrowComp->SetRelativeLocation(FVector(0.f, 1600.f, 250.f));
	LineArrowComp->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	

	FSM = CreateDefaultSubobject<UCSafiFSM>(TEXT("FSM"));
	Anim = Cast<UCSafiAnimInstance>(GetMesh()->GetAnimInstance());
	USkeletalMeshComponent* SkeletalMeshComp = GetMesh();
	if (SkeletalMeshComp)
	{
		SkeletalMeshComp->bEnablePerPolyCollision = true;
		SkeletalMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		SkeletalMeshComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	}

#pragma endregion Components

//========================= 콜리전 세팅 파트

#pragma  region Collision
// =================== 공격용 콜리전 ===================	

	AttCollisionBite = CreateDefaultSubobject<UBoxComponent>(TEXT("AttCollisionBite"));	// 머리
	AttCollisionBite->SetupAttachment(SafiComponent, TEXT("Socket_BiteDMGBox"));
	AttCollisionBite->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttCollisionBite->SetCollisionResponseToAllChannels(ECR_Overlap);

	AttCollisionLF = CreateDefaultSubobject<UBoxComponent>(TEXT("AttCollisionLF"));		// 왼손
	AttCollisionLF->SetupAttachment(SafiComponent, TEXT("Socket_LF"));
	AttCollisionLF->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttCollisionLF->SetCollisionResponseToAllChannels(ECR_Overlap);

	AttCollisionRF = CreateDefaultSubobject<UBoxComponent>(TEXT("AttCollisionRF"));		// 오른손
	AttCollisionRF->SetupAttachment(SafiComponent, TEXT("Socket_RF"));
	AttCollisionRF->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttCollisionRF->SetCollisionResponseToAllChannels(ECR_Overlap);

	AttCollisionLB = CreateDefaultSubobject<UBoxComponent>(TEXT("AttCollisionLB"));		// 왼발
	AttCollisionLB->SetupAttachment(SafiComponent, TEXT("Socket_LB"));
	AttCollisionLB->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttCollisionLB->SetCollisionResponseToAllChannels(ECR_Overlap);

	AttCollisionRB = CreateDefaultSubobject<UBoxComponent>(TEXT("AttCollisionRB"));		// 오른발
	AttCollisionRB->SetupAttachment(SafiComponent, TEXT("Socket_RB"));
	AttCollisionRB->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttCollisionRB->SetCollisionResponseToAllChannels(ECR_Overlap);



// =================== 공격 위치 판정용 콜리전===================

	AttPosLF = CreateDefaultSubobject<UBoxComponent>(TEXT("AttPosLF"));
	AttPosLF->SetupAttachment(SafiComponent);
	AttPosLF->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttPosLF->SetCollisionResponseToAllChannels(ECR_Overlap);


	AttPosRF = CreateDefaultSubobject<UBoxComponent>(TEXT("AttPosRF"));
	AttPosRF->SetupAttachment(SafiComponent);
	AttPosRF->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttPosRF->SetCollisionResponseToAllChannels(ECR_Overlap);


	AttPosLB = CreateDefaultSubobject<UBoxComponent>(TEXT("AttPosLB"));
	AttPosLB->SetupAttachment(SafiComponent);
	AttPosLB->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttPosLB->SetCollisionResponseToAllChannels(ECR_Overlap);

	AttPosRB = CreateDefaultSubobject<UBoxComponent>(TEXT("AttPosRB"));
	AttPosRB->SetupAttachment(SafiComponent);
	AttPosRB->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttPosRB->SetCollisionResponseToAllChannels(ECR_Overlap);
#pragma  endregion Collision


#pragma region Location&Extent
// ========================= 박스 위치 설정 파트

	AttCollisionLF->SetRelativeLocation(FVector(5.f, 0.f, 110.f));
	AttCollisionRF->SetRelativeLocation(FVector(5.f, 0.f, 110.f));
	AttCollisionLB->SetRelativeLocation(FVector(25.f, 0.f, 130.f));
	AttCollisionRB->SetRelativeLocation(FVector(-40.f, 0.f, 130.f));

	AttPosLF->SetRelativeLocation(FVector(650.f, 630.f, 160.f));
	AttPosRF->SetRelativeLocation(FVector(-650.f, 630.f, 160.f));
	AttPosLB->SetRelativeLocation(FVector(650.f, -350.f, 160.f));
	AttPosRB->SetRelativeLocation(FVector(-650.f, -350.f, 160.f));

// ========================= 박스 크기 설정 파트

	AttCollisionBite->SetBoxExtent(FVector(70.f, 80.f, 150.f));

	AttCollisionLF->SetBoxExtent(FVector(180.f, 70.f, 170.f));
	AttCollisionRF->SetBoxExtent(FVector(180.f, 70.f, 170.f));
	AttCollisionLB->SetBoxExtent(FVector(140.f, 70.f, 180.f));
	AttCollisionRB->SetBoxExtent(FVector(140.f, 70.f, 180.f));

	AttPosLF->SetBoxExtent(FVector(200.f));
	AttPosRF->SetBoxExtent(FVector(200.f));
	AttPosLB->SetBoxExtent(FVector(200.f));
	AttPosRB->SetBoxExtent(FVector(200.f));


#pragma endregion Location&Extent

	// ========================= 콜리전 충돌 체크
	AttCollisionBite->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);

	AttCollisionLF->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	AttCollisionRF->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	AttCollisionLB->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	AttCollisionRB->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	
	AttPosLF->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	AttPosRF->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	AttPosLB->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	AttPosRB->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);

	SkeletalMeshComp->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
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
		FSM->OnDisturbedProcess();
	}

	DrawLineTrace();

	// DrawDebugSphere(GetWorld(), this->GetActorLocation(), MeleeAttRange, 12, FColor::Green, true, -1, 0, 0);


	// 콜리전 활성화, 비활성화 파트
	
// 머리공격 콜리전 활성화 / 비활성화
	if (isOnAttBite == true) { AttCollisionBite->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); }
	else { AttCollisionBite->SetCollisionEnabled(ECollisionEnabled::NoCollision); }

// 다리공격 콜리전 활성화 / 비활성화
	if (isFootAttack == true)
	{
		switch (attackPos)
		{
		case AttMELEE_LF: 
			AttCollisionLF->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			break;

		case AttMELEE_RF:
			AttCollisionRF->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			break;

		case AttMELEE_LB:
			AttCollisionLB->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			break;

		case AttMELEE_RB:
			AttCollisionRB->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			break;
		}
	}

	else
	{
		AttCollisionLF->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AttCollisionRF->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AttCollisionLB->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AttCollisionRB->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

// 색적 콜리전 활성화/비활성화
	if (isOnSearch == true)
	{
		AttPosLF->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AttPosRF->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AttPosLB->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AttPosRB->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	else
	{
		AttPosLF->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AttPosRF->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AttPosLB->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AttPosRB->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

	if ( bHit && HitInfo.GetActor()->GetActorNameOrLabel().Contains("Hunter"))
	{
		// 충돌체크
		UE_LOG(LogTemp, Error, TEXT("TEST"));
	}

	return bHit;
}

void ACSafiJiiva::SetNormal()
{

	isOnBreath = false;
	isOnAttBite= false;
	isFootAttack = false;

	//isRepelled = false;
	isDisturbed = false;

	isOnSearch = false;
}

void ACSafiJiiva::SetSpeed(float _value)
{
	this->GetCharacterMovement()->MaxWalkSpeed = _value;
}

void ACSafiJiiva::OnDamageSafi(float _value)
{
	hp -= _value;
	RepellCount += 1;

	if(hp > 0)
	{
		if (RepellCount >= MAXRepellCount)
		{
			isKnockBack = true;

			// Disturbed 상태가 걸림
			SetNormal();
			isDisturbed = true;		// AnimInstance 에서 연동되는중
			
			RepellCount = 0;
		}

		return;
	}


	hp = 0;
	//뭔가 사망처리 해주기
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

	if (OverlappedComp == AttCollisionBite)
	{
		//target->SetDamage(MeleeBiteDMG);
		UE_LOG(LogTemp,Warning,TEXT("Bite_Test"));
	}

	if (OverlappedComp == AttPosLF)		// 왼쪽
	{
		attackPos = AttMELEE_LF;
		UE_LOG(LogTemp, Warning, TEXT("Hit LF"));
	}

	if (OverlappedComp == AttPosRF)		// 오른쪽
	{
		attackPos = AttMELEE_RF;
		UE_LOG(LogTemp, Warning, TEXT("Hit RF"));
	}

	if (OverlappedComp == AttPosLB)		// 왼쪽 뒤
	{
		attackPos = AttMELEE_LB;
		UE_LOG(LogTemp, Warning, TEXT("Hit LB"));
	}

	if (OverlappedComp == AttPosRB)		// 오른쪽 뒤
	{
		attackPos = AttMELEE_RB;
		UE_LOG(LogTemp, Warning, TEXT("Hit RB"));
	}

	if (isOnBodyPress == true)
	{
		//헌터에 데미지 주기
	}


	/* 헌터쪽에서 처리하는게 맞을듯?
	if (target->태클상태)
	{
		this->OnDamageSafi(태클데미지)
	}
	*/


	// Tick에서 스테이터스 체크해서 isDisturbed 체크		- AnimInstance쪽 isDisturbedA와 연동 완료

}

