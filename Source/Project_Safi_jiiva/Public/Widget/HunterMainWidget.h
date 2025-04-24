// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HunterMainWidget.generated.h"

/**
 *
 */
UCLASS()
class PROJECT_SAFI_JIIVA_API UHunterMainWidget : public UUserWidget
{
	GENERATED_BODY()
	virtual void NativeConstruct() override;
private:
	class AHunter* Owner;
public:
	UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidget))
    class UProgressBar* HPProgressBar;

	UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidget))
    class UProgressBar* SPProgressBar;

	UPROPERTY(BlueprintReadOnly, Category = "UI", meta = (BindWidget))
	class UTextBlock* userName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	float uiHp=1.0;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
	float uiSp=1.0;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
    class AHunter* OwningActor;

};
