// Fill out your copyright notice in the Description page of Project Settings.
// 애가 월보를 한다... Location z -90 / 비행상태는 isFly로 체크

// GameTarceChannel 1 -> Safi
// GameTarceChannel 2 -> Test
// GameTarceChannel 3 -> Attack
// GameTarceChannel 4 -> SafiAttack

#include "SafiJiiva/CSafiJiiva.h"
#include "Engine/SkeletalMesh.h"
#include "Components/ArrowComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "SafiJiiva/CSafiFSM.h"
#include "Hunter/Hunter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "SafiJiiva/CSafiAnimInstance.h"
#include "Kismet/GameplayStatics.h"

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

		SafiComponent->SetRelativeScale3D(FVector(1.f));
		//SafiComponent->SetCollisionObjectType(ECC_GameTraceChannel1);
		SafiComponent->SetCollisionProfileName(TEXT("SafiBody"));

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
		SkeletalMeshComp->SetCollisionProfileName(TEXT("SafiBody"));
	}

#pragma endregion Components

//========================= 콜리전 세팅 파트

// =================== 피격용 콜리전 ===================
#pragma region BodyCollision

	// 머리
	BodyColHead = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyColHead"));
	BodyColHead->SetupAttachment(SafiComponent, TEXT("Socket_BiteDMGBox"));
	BodyColHead->SetCollisionProfileName(TEXT("SafiBody"));

	BodyColNeck_1 = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyColNeck_1"));
	BodyColNeck_1->SetupAttachment(SafiComponent, TEXT("Socket_Neck_1"));
	BodyColNeck_1->SetCollisionProfileName(TEXT("SafiBody"));

	BodyColNeck_2 = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyColNeck_2"));
	BodyColNeck_2->SetupAttachment(SafiComponent, TEXT("Socket_Neck_2"));
	BodyColNeck_2->SetCollisionProfileName(TEXT("SafiBody"));

	BodyCol_1 = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyCol_1"));
	BodyCol_1->SetupAttachment(SafiComponent, TEXT("Socket_Body_1"));
	BodyCol_1->SetCollisionProfileName(TEXT("SafiBody"));

	// 꼬리 콜리전
	for (int32 i = 1; i <= 3; ++i)
	{
		FString CompName = FString::Printf(TEXT("BodyColTail_%d"), i);
		UBoxComponent* TailBox = CreateDefaultSubobject<UBoxComponent>(*CompName);

		FString SocketName = FString::Printf(TEXT("Socket_Tail_%d"), i);
		TailBox->SetupAttachment(SafiComponent, FName(*SocketName));

		TailBox->SetCollisionProfileName(TEXT("SafiBody"));

		TailBox->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
		TailBox->SetBoxExtent(FVector(150.f, 150.f, 400.f));

		TailCollisionBoxes.Add(TailBox);

		// 태그 붙이기 (데미지 분기용)
		TailBox->ComponentTags.Add(FName("Tail"));
	}



#pragma endregion BodyCollision

#pragma region LegCollision
	// 왼쪽 앞다리 (Left Front)
	BodyColLF_1 = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyColLF_1"));
	BodyColLF_1->SetupAttachment(SafiComponent, TEXT("Socket_LegLF_1"));
	BodyColLF_1->SetCollisionProfileName(TEXT("SafiBody"));

	BodyColLF_2 = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyColLF_2"));
	BodyColLF_2->SetupAttachment(SafiComponent, TEXT("Socket_LegLF_2"));
	BodyColLF_2->SetCollisionProfileName(TEXT("SafiBody"));

	BodyColLF_3 = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyColLF_3"));
	BodyColLF_3->SetupAttachment(SafiComponent, TEXT("Socket_LegLF_3"));
	BodyColLF_3->SetCollisionProfileName(TEXT("SafiBody"));


	// 오른쪽 앞다리 (Right Front)
	BodyColRF_1 = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyColRF_1"));
	BodyColRF_1->SetupAttachment(SafiComponent, TEXT("Socket_LegRF_1"));
	BodyColRF_1->SetCollisionProfileName(TEXT("SafiBody"));


	BodyColRF_2 = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyColRF_2"));
	BodyColRF_2->SetupAttachment(SafiComponent, TEXT("Socket_LegRF_2"));
	BodyColRF_2->SetCollisionProfileName(TEXT("SafiBody"));

	BodyColRF_3 = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyColRF_3"));
	BodyColRF_3->SetupAttachment(SafiComponent, TEXT("Socket_LegRF_3"));
	BodyColRF_3->SetCollisionProfileName(TEXT("SafiBody"));

	// 왼쪽 뒷다리 (Left Back)
	BodyColLB_1 = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyColLB_1"));
	BodyColLB_1->SetupAttachment(SafiComponent, TEXT("Socket_LegLB_1"));
	BodyColLB_1->SetCollisionProfileName(TEXT("SafiBody"));

	BodyColLB_2 = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyColLB_2"));
	BodyColLB_2->SetupAttachment(SafiComponent, TEXT("Socket_LegLB_2"));
	BodyColLB_2->SetCollisionProfileName(TEXT("SafiBody"));

	BodyColLB_3 = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyColLB_3"));
	BodyColLB_3->SetupAttachment(SafiComponent, TEXT("Socket_LegLB_3"));
	BodyColLB_3->SetCollisionProfileName(TEXT("SafiBody"));

	BodyColLB_4 = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyColLB_4"));
	BodyColLB_4->SetupAttachment(SafiComponent, TEXT("Socket_LegLB_4"));
	BodyColLB_4->SetCollisionProfileName(TEXT("SafiBody"));

	// 오른쪽 뒷다리 (Right Back)
	BodyColRB_1 = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyColRB_1"));
	BodyColRB_1->SetupAttachment(SafiComponent, TEXT("Socket_LegRB_1"));
	BodyColRB_1->SetCollisionProfileName(TEXT("SafiBody"));

	BodyColRB_2 = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyColRB_2"));
	BodyColRB_2->SetupAttachment(SafiComponent, TEXT("Socket_LegRB_2"));
	BodyColRB_2->SetCollisionProfileName(TEXT("SafiBody"));

	BodyColRB_3 = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyColRB_3"));
	BodyColRB_3->SetupAttachment(SafiComponent, TEXT("Socket_LegRB_3"));
	BodyColRB_3->SetCollisionProfileName(TEXT("SafiBody"));

	BodyColRB_4 = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyColRB_4"));
	BodyColRB_4->SetupAttachment(SafiComponent, TEXT("Socket_LegRB_4"));
	BodyColRB_4->SetCollisionProfileName(TEXT("SafiBody"));

#pragma endregion LegCollision

// 공격 콜리전 위치/사이즈
#pragma region AttCollision
// =================== 공격용 콜리전 ===================

	AttCollisionBite = CreateDefaultSubobject<UBoxComponent>(TEXT("AttCollisionBite"));	// 머리
	AttCollisionBite->SetupAttachment(SafiComponent, TEXT("Socket_BiteDMGBox"));
	AttCollisionBite->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttCollisionBite->SetCollisionResponseToAllChannels(ECR_Overlap);
	AttCollisionBite->SetCollisionProfileName(TEXT("SafiAttack"));
	AttCollisionBite->SetCollisionObjectType(ECC_Pawn);

	AttCollisionLF = CreateDefaultSubobject<UBoxComponent>(TEXT("AttCollisionLF"));		// 왼손
	AttCollisionLF->SetupAttachment(SafiComponent, TEXT("Socket_LegLF_3"));
	AttCollisionLF->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttCollisionLF->SetCollisionResponseToAllChannels(ECR_Overlap);
	AttCollisionLF->SetCollisionProfileName(TEXT("SafiAttack"));
	AttCollisionLF->SetCollisionObjectType(ECC_Pawn);

	AttCollisionRF = CreateDefaultSubobject<UBoxComponent>(TEXT("AttCollisionRF"));		// 오른손
	AttCollisionRF->SetupAttachment(SafiComponent, TEXT("Socket_LegRF_3"));
	AttCollisionRF->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttCollisionRF->SetCollisionResponseToAllChannels(ECR_Overlap);
	AttCollisionRF->SetCollisionProfileName(TEXT("SafiAttack"));
	AttCollisionRF->SetCollisionObjectType(ECC_Pawn);

	AttCollisionLB = CreateDefaultSubobject<UBoxComponent>(TEXT("AttCollisionLB"));		// 왼발
	AttCollisionLB->SetupAttachment(SafiComponent, TEXT("Socket_LegLB_4"));
	AttCollisionLB->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttCollisionLB->SetCollisionResponseToAllChannels(ECR_Overlap);
	AttCollisionLB->SetCollisionProfileName(TEXT("SafiAttack"));
	AttCollisionLB->SetCollisionObjectType(ECC_Pawn);

	AttCollisionRB = CreateDefaultSubobject<UBoxComponent>(TEXT("AttCollisionRB"));		// 오른발
	AttCollisionRB->SetupAttachment(SafiComponent, TEXT("Socket_LegRB_4"));
	AttCollisionRB->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttCollisionRB->SetCollisionResponseToAllChannels(ECR_Overlap);
	AttCollisionRB->SetCollisionProfileName(TEXT("SafiAttack"));
	AttCollisionRB->SetCollisionObjectType(ECC_Pawn);


	BodyCol_2 = CreateDefaultSubobject<UBoxComponent>(TEXT("BodyCol_2"));
	BodyCol_2->SetupAttachment(SafiComponent, TEXT("Socket_Body_1"));
	BodyCol_2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyCol_2->SetCollisionResponseToAllChannels(ECR_Overlap);
	BodyCol_2->SetCollisionProfileName(TEXT("SafiBody"));
	BodyCol_2->SetCollisionObjectType(ECC_Pawn);



// =================== 공격 위치 판정용 콜리전===================

	AttPosLF = CreateDefaultSubobject<UBoxComponent>(TEXT("AttPosLF"));
	AttPosLF->SetupAttachment(SafiComponent);
	AttPosLF->SetCollisionResponseToAllChannels(ECR_Overlap);
	AttPosLF->SetCollisionEnabled(ECollisionEnabled::NoCollision);



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
#pragma endregion AttCollision

// 데미지 콜리전 위치/사이즈
#pragma region Location&Extent_DMG

	// =================== 공격용 위치===================

	AttCollisionLF->SetRelativeLocation(FVector(5.f, 0.f, 110.f));
	AttCollisionRF->SetRelativeLocation(FVector(5.f, 0.f, 110.f));
	AttCollisionLB->SetRelativeLocation(FVector(25.f, 0.f, 130.f));
	AttCollisionRB->SetRelativeLocation(FVector(-40.f, 0.f, 130.f));

	AttPosLF->SetRelativeLocation(FVector(650.f, 630.f, 160.f));
	AttPosRF->SetRelativeLocation(FVector(-650.f, 630.f, 160.f));
	AttPosLB->SetRelativeLocation(FVector(650.f, -350.f, 160.f));
	AttPosRB->SetRelativeLocation(FVector(-650.f, -350.f, 160.f));

	BodyCol_2->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	BodyCol_2->SetBoxExtent(FVector(350.f, 350.f, 500.f));


	// =================== 공격용 박스 사이즈===================
	AttCollisionBite->SetBoxExtent(FVector(70.f, 80.f, 150.f));
	AttCollisionLF->SetBoxExtent(FVector(180.f));
	AttCollisionRF->SetBoxExtent(FVector(180.f));
	AttCollisionLB->SetBoxExtent(FVector(180.f));
	AttCollisionRB->SetBoxExtent(FVector(180.f));


	AttPosLF->SetBoxExtent(FVector(200.f));
	AttPosRF->SetBoxExtent(FVector(200.f));
	AttPosLB->SetBoxExtent(FVector(200.f));
	AttPosRB->SetBoxExtent(FVector(200.f));

#pragma endregion Location&Extent_DMG

// 몸 콜리전 위치/사이즈
#pragma region Location&Extent_Body
// ========================= 박스 크기& 위치 설정 파트
	BodyColHead->SetRelativeLocation(FVector(0.f, -50.f, -60.f));
	BodyColHead->SetBoxExtent(FVector(130.f, 130.f, 210.f));

	BodyColNeck_1->SetRelativeLocation(FVector(0.f, -50.f, 0.f));
	BodyColNeck_1->SetBoxExtent(FVector(150.f, 140.f, 190.f));
	BodyColNeck_2->SetRelativeLocation(FVector(0.f, -30.f, 0.f));
	BodyColNeck_2->SetBoxExtent(FVector(130.f, 120.f, 190.f));

	BodyCol_1->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	BodyCol_1->SetBoxExtent(FVector(300.f, 300.f, 400.f));


#pragma endregion Location&Extent_Body


// 다리 콜리전 위치/사이즈
#pragma region Location&Extent_LEG
// ========================= 박스 크기& 위치 설정 파트 (Body)
	BodyColLF_1->SetRelativeLocation(FVector(0.f, 100.f, 00.f));
	BodyColLF_1->SetBoxExtent(FVector(150.f, 250.f, 150.f));
	BodyColLF_2->SetRelativeLocation(FVector(0.f, 160.f, 0.f));
	BodyColLF_2->SetBoxExtent(FVector(100.f, 250.f, 100.f));
	BodyColLF_3->SetRelativeLocation(FVector(5.f, 0.f, 110.f));
	BodyColLF_3->SetBoxExtent(FVector(180.f, 70.f, 170.f));

	BodyColRF_1->SetRelativeLocation(FVector(0.f, 100.f, 00.f));
	BodyColRF_1->SetBoxExtent(FVector(150.f, 250.f, 150.f));
	BodyColRF_2->SetRelativeLocation(FVector(0.f, 160.f, 0.f));
	BodyColRF_2->SetBoxExtent(FVector(100.f, 250.f, 100.f));
	BodyColRF_3->SetRelativeLocation(FVector(5.f, 0.f, 110.f));
	BodyColRF_3->SetBoxExtent(FVector(180.f, 70.f, 170.f));

	BodyColLB_1->SetRelativeLocation(FVector(0.f, 150.f, 30.f));
	BodyColLB_1->SetBoxExtent(FVector(150.f, 250.f, 150.f));
	BodyColLB_2->SetRelativeLocation(FVector(0.f, 100.f, 0.f));
	BodyColLB_2->SetBoxExtent(FVector(130.f, 180.f, 100.f));
	BodyColLB_3->SetRelativeLocation(FVector(0.f, 100.f, 0.f));
	BodyColLB_3->SetBoxExtent(FVector(80.f, 190.f, 80.f));
	BodyColLB_4->SetRelativeLocation(FVector(25.f, 0.f, 130.f));
	BodyColLB_4->SetBoxExtent(FVector(140.f, 70.f, 180.f));

	BodyColRB_1->SetRelativeLocation(FVector(0.f, 150.f, -40.f));
	BodyColRB_1->SetBoxExtent(FVector(150.f, 250.f, 150.f));
	BodyColRB_2->SetRelativeLocation(FVector(0.f, 100.f, 0.f));
	BodyColRB_2->SetBoxExtent(FVector(130.f, 180.f, 100.f));
	BodyColRB_3->SetRelativeLocation(FVector(0.f, 100.f, 0.f));
	BodyColRB_3->SetBoxExtent(FVector(80.f, 190.f, 80.f));
	BodyColRB_4->SetRelativeLocation(FVector(-40.f, 0.f, 130.f));
	BodyColRB_4->SetBoxExtent(FVector(140.f, 70.f, 180.f));

#pragma endregion Location&Extent_LEG
// =====================================================================

//콜리전 충돌 파트
#pragma region BeginOverlap

/*
	BodyColHead->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	BodyColLF_3->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	BodyColRF_3->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	BodyColLB_4->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	BodyColRB_4->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
*/

	AttCollisionBite->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnAttackOverlapBegin);
	AttCollisionLF->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnAttackOverlapBegin);
	AttCollisionRF->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnAttackOverlapBegin);
	AttCollisionLB->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnAttackOverlapBegin);
	AttCollisionRB->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnAttackOverlapBegin);
	BodyCol_2->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnAttackOverlapBegin);



//==============================================================================

	BodyColHead->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	BodyColNeck_1->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	BodyColNeck_2->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);

	AttPosLF->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	AttPosRF->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	AttPosLB->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	AttPosRB->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	BodyCol_1->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);

	BodyColLF_1->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	BodyColLF_2->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	BodyColLF_3->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);

	BodyColRF_1->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	BodyColRF_2->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	BodyColRF_3->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);

	BodyColLB_1->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	BodyColLB_2->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	BodyColLB_3->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	BodyColLB_4->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);

	BodyColRB_1->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	BodyColRB_2->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	BodyColRB_3->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);
	BodyColRB_4->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);


	SkeletalMeshComp->OnComponentBeginOverlap.AddDynamic(this, &ACSafiJiiva::OnOverlapBegin);

#pragma endregion BeginOverlap
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
// ============================= 테스트용 Tick 데미지 =============================
//
//	currentTime += DeltaTime;
//	if (currentTime > 1.f)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("DamageCount : %d"), RepellCount);
//
//		UGameplayStatics::ApplyDamage(this, 100.f, nullptr, this, nullptr);
//		//OnDamageSafi(105.f);
//		currentTime = 0.f;
//	}
//
//==================================================================================
// 
	// 노티파이 제어시 생길 문제들 없애는 용도 :D...
	if (isDisturbed == true)
	{
		FSM->OnDisturbedProcess();
	}

	DrawLineTrace();

	// DrawDebugSphere(GetWorld(), this->GetActorLocation(), MeleeAttRange, 12, FColor::Green, true, -1, 0, 0);


	// 콜리전 활성화, 비활성화 파트
#pragma region CollisionEnable

// 몸통공격 콜리전 활성화 / 비활성화
	if (isOnBodyPress == true) { BodyCol_2->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); }
	else
	{
		BodyCol_2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}


// 머리공격 콜리전 활성화 / 비활성화
	if (isOnBodyPress == true || isOnAttBite == true) 
	{ AttCollisionBite->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); }
	else 
	{ 
		AttCollisionBite->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

// 다리공격 콜리전 활성화 / 비활성화
	if (isOnBodyPress == true || isFootAttack == true)
	{
		AttCollisionLF->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AttCollisionRF->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AttCollisionLB->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AttCollisionRB->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		/*
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
		*/
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
#pragma endregion CollisionEnable

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
/*
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
	isDead = true;
	FSM->OnDisturbedProcess();
}
*/

float ACSafiJiiva::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	hp -= DamageAmount;
	RepellCount += 1;

	if (hp > 0)
	{
		if (RepellCount >= MAXRepellCount)
		{
			isKnockBack = true;

			// Disturbed 상태가 걸림
			SetNormal();
			isDisturbed = true;		// AnimInstance 에서 연동되는중

			RepellCount = 0;
		}
		return DamageAmount;
	}


	hp = 0;
	//뭔가 사망처리 해주기
	isDead = true;
	FSM->OnDisturbedProcess();

	return DamageAmount;
}


void ACSafiJiiva::KillSafi_Test()
{
	//this->OnDamageSafi(MAXHP);
	UGameplayStatics::ApplyDamage(this, MAXHP, nullptr, this, nullptr);
}


void ACSafiJiiva::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

#pragma region Hand

	AHunter* target = Cast<AHunter>(OtherActor);
	if (!target) { return; }

	if (OverlappedComp == AttCollisionBite)
	{
		//target->SetDamage(MeleeBiteDMG);
		UE_LOG(LogTemp,Warning,TEXT("Bite_Test"));
	}

	else if (OverlappedComp == AttPosLF)		// 왼쪽
	{
		attackPos = AttMELEE_LF;
		UE_LOG(LogTemp, Warning, TEXT("Hit LF"));
	}

	else if (OverlappedComp == AttPosRF)		// 오른쪽
	{
		attackPos = AttMELEE_RF;
		UE_LOG(LogTemp, Warning, TEXT("Hit RF"));
	}

	else if (OverlappedComp == AttPosLB)		// 왼쪽 뒤
	{
		attackPos = AttMELEE_LB;
		UE_LOG(LogTemp, Warning, TEXT("Hit LB"));
	}

	else if (OverlappedComp == AttPosRB)		// 오른쪽 뒤
	{
		attackPos = AttMELEE_RB;
		UE_LOG(LogTemp, Warning, TEXT("Hit RB"));
	}
#pragma endregion Hand


}


void ACSafiJiiva::OnAttackOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AHunter* target = Cast<AHunter>(OtherActor);
	if (!target || HitPawn.Contains(target)) { return; }

	// 공격 콜리전 켜주는 녀석들이 켜져 있을 경우엔
	if (isOnAttBite == true || isFootAttack == true || isOnBreath == true || isOnBodyPress == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("DMG_Tes1t"));
		if (HitPawn.Num() <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("DMG_Test"));
			// 임시 데미지 MeleeBiteDMG
			UGameplayStatics::ApplyDamage(OtherActor, MeleeBiteDMG, nullptr, this, nullptr);
			HitPawn.AddUnique(target);
		}
	}

}
