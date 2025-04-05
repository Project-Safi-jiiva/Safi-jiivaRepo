// Fill out your copyright notice in the Description page of Project Settings.

#include "AssetPath.h"
#include "Hunter/Hunter.h/"
#include "Project_Safi_jiiva.h"
#include "Hunter/MoveComponent.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/InputMappingContext.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "Hunter/HunterAnim.h"

// Sets default values
AHunter::AHunter()
{
	PrimaryActorTick.bCanEverTick = true;

	//컴포넌트 추가 부분
	MoveComp = CreateDefaultSubobject<UMoveComponent>(TEXT("MoveComponent"));
	//스켈레탈 메쉬 추가
	ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Hunter(AssetPaths::HUNTER_MESH);
	if (SK_Hunter.Succeeded()) GetMesh()->SetSkeletalMesh(SK_Hunter.Object);
	//애니메이션 블루프린트 추가
	ConstructorHelpers::FClassFinder<UHunterAnim> AB_Hunter(AssetPaths::HUNTER_ANIM);

	ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_HunterTool(AssetPaths::HUNTER_IMC);


	if (AB_Hunter.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(AB_Hunter.Class);
		GetMesh()->SetRelativeLocationAndRotation(FVector(0,0,-90), FRotator(0,-90,0));
		Anim = Cast<UHunterAnim>(GetMesh()->GetAnimInstance());
		IMC_Hunter = IMC_HunterTool.Object;
	}



}

void AHunter::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PC = Cast<APlayerController>(GetController());
	UEnhancedInputLocalPlayerSubsystem* subSys = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());

	if (subSys)
		subSys->AddMappingContext(IMC_Hunter, 0);

}

void AHunter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHunter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto PlayerInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if(InputBindingDeleagate.IsBound())
		InputBindingDeleagate.Broadcast(PlayerInput);

}

