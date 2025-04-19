// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DT_SafiBodyDMG.h"
#include "DA_SafiBodyDMG.generated.h"

/**
 * 
 */
UCLASS()
class PROJECT_SAFI_JIIVA_API UDA_SafiBodyDMG : public UDataAsset
{
	GENERATED_BODY()
	public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SafiBodyDMG")
	TArray<FDT_SafiBodyDMG> DT_SafiBodyDMG;
};
