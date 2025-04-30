// Fill out your copyright notice in the Description page of Project Settings.


#include "SafiJiiva/CSafiSpecialActor.h"
#include "Kismet/GameplayStatics.h"
#include "SafiJiiva/CSafiJiiva.h"

// Sets default values
ACSafiSpecialActor::ACSafiSpecialActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACSafiSpecialActor::BeginPlay()
{
	Super::BeginPlay();

	me = Cast<ACSafiJiiva>(UGameplayStatics::GetActorOfClass(GetWorld(), ACSafiJiiva::StaticClass()));
	UpdateHunterList();
}

// Called every frame
void ACSafiSpecialActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bOnSpawn)
	{
		currentTime += DeltaTime;
		FVector P = GetActorLocation();

		speed += PlusSpeed;
		P.Z -= speed * DeltaTime;		// 수직 하강하도록 하기.

		this->SetActorLocation(P);


		if (currentTime > MaxTime)
		{
			KillingTime();
			ReturnToBase();
			currentTime = 0.f;
		}

	}
}

void ACSafiSpecialActor::KillingTime()
{
	FVector Start = FVector(GetActorLocation().X, GetActorLocation().Y, 400.f);

	UpdateHunterList();

	for (AHunter* Hunter : HunterList)
	{
		if (!IsValid(Hunter)){ continue; }

		FVector End = Hunter->GetActorLocation();

		//FHitResult Hit;
		FCollisionQueryParams Params;

		Params.AddIgnoredActor(this);

		TArray<FHitResult> Hits;

		bool bHit = GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, ECC_Pawn, FCollisionShape::MakeSphere(10.f), Params);

		DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Red : FColor::Blue, false, 2.0f, 0, 3.0f);
		//

		for (const FHitResult& HitResult : Hits)
		{
			AActor* HitActor = HitResult.GetActor();
			if (HitActor)
			{
				UE_LOG(LogTemp, Warning, TEXT("Hit Actor22222: %s"), *HitResult.GetActor()->GetName());

				AActor* Hunters = Cast<AHunter>(HitActor);

				// 헌터가 아닌게 맞으면 처리중지
				if (HitActor != Hunters)
				{
					//UE_LOG(LogTemp, Warning, TEXT("Hit something else: %s"), *Hits.GetActor()->GetName());
					continue;
				}
				// 아니면 헌터일테니 데미지
				else
				{
					UE_LOG(LogTemp, Log, TEXT("Successfully hit hunter: %s"), *Hunters->GetName());
					UGameplayStatics::ApplyDamage(Hunters, 200, nullptr, me, nullptr);
				}

			}
		}
		/*
		if (bHit)
		{
			// 헌터가 아닌게 맞으면 처리중지
			if (Hit.GetActor() != Hunter)
			{
				UE_LOG(LogTemp, Warning, TEXT("Hit something else: %s"), *Hit.GetActor()->GetName());
				continue;
			}
			// 아니면 헌터일테니 데미지
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Successfully hit hunter: %s"), *Hunter->GetName());
				UGameplayStatics::ApplyDamage(Hunter, 200, nullptr, me, nullptr);
			}

		}
		*/

		/*
		bool bHit = GetWorld()->LineTraceSingleByChannel( Hit, Start, End, ECC_Pawn, Params );

		DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Red : FColor::Blue, false, 2.0f, 0, 3.0f);

		if (bHit)
		{
			// 헌터가 아닌게 맞으면 처리중지
			if (Hit.GetActor() != Hunter)
			{
				UE_LOG(LogTemp, Warning, TEXT("Hit something else: %s"), *Hit.GetActor()->GetName());
				continue;
			}
			// 아니면 헌터일테니 데미지
			else
			{
				UE_LOG(LogTemp, Log, TEXT("Successfully hit hunter: %s"), *Hunter->GetName());
				UGameplayStatics::ApplyDamage(Hunter, 200, nullptr, me, nullptr);
			}

		}
		*/
	}

}

void ACSafiSpecialActor::SetbOnSpawn()
{
	bOnSpawn = true;
}

void ACSafiSpecialActor::ReturnToBase()
{
	speed = defaultSpeed;
	bOnSpawn = false;
	SetActorLocation(FVector(8000.f));
}

void ACSafiSpecialActor::UpdateHunterList()
{
	HunterList.Empty();

	TArray<AActor*> joinedPlayers;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHunter::StaticClass(), joinedPlayers);

	for (AActor* player : joinedPlayers)
	{
		AHunter* hunter = Cast<AHunter>(player);

		if (IsValid(hunter))
		{
			UE_LOG(LogTemp, Warning, TEXT("Hunter Found: %s"), *hunter->GetName());
			HunterList.Add(hunter);
		}

	}
}

